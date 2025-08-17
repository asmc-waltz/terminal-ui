/**
 * @file audio_mgr.c
 *
 * ALSA manager implementation.
 * - init/prepare/reinit/release
 * - mmap write with per-channel + master gain
 * - options: auto_reinit, skip_format_check
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <audio.h>

#include <log.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
/* helper: clamp float to [0..1] */
static float clamp01(float v)
{
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

/* helper: recover from xrun or suspend */
static int recover_xrun(snd_pcm_t *pcm, int err)
{
    int rc;

    if (err == -EPIPE) {
        rc = snd_pcm_prepare(pcm);
        return rc < 0 ? rc : 0;
    }
    if (err == -ESTRPIPE) {
        /* wait for resume */
        while ((rc = snd_pcm_resume(pcm)) == -EAGAIN)
            usleep(1000);
        if (rc < 0)
            rc = snd_pcm_prepare(pcm);
        return rc < 0 ? rc : 0;
    }
    return err;
}

/* configure hw params for mgr->fmt */
static int set_hw_params(struct audio_mgr *mgr,
             snd_pcm_hw_params_t *params,
             unsigned int buffer_time_us,
             unsigned int period_time_us)
{
    int rc;
    unsigned int rate = mgr->fmt.sample_rate;

    snd_pcm_hw_params_any(mgr->pcm, params);

    rc = snd_pcm_hw_params_set_access(mgr->pcm, params,
                     SND_PCM_ACCESS_MMAP_INTERLEAVED);
    if (rc < 0) return rc;

    rc = snd_pcm_hw_params_set_format(mgr->pcm, params, mgr->fmt.pcm_format);
    if (rc < 0) return rc;

    rc = snd_pcm_hw_params_set_channels(mgr->pcm, params, mgr->fmt.channels);
    if (rc < 0) return rc;

    rc = snd_pcm_hw_params_set_rate_near(mgr->pcm, params, &rate, 0);
    if (rc < 0) return rc;

    if (buffer_time_us) {
        rc = snd_pcm_hw_params_set_buffer_time_near(mgr->pcm, params, &buffer_time_us, 0);
        if (rc < 0) return rc;
    }
    if (period_time_us) {
        rc = snd_pcm_hw_params_set_period_time_near(mgr->pcm, params, &period_time_us, 0);
        if (rc < 0) return rc;
    }

    return snd_pcm_hw_params(mgr->pcm, params);
}

/* internal init helper used by init and reinit */
static int internal_open_and_config(struct audio_mgr *mgr,
                    unsigned int buffer_time_us,
                    unsigned int period_time_us)
{
    snd_pcm_hw_params_t *params;
    int rc;

    if (!mgr || !mgr->device_name) return AUDIO_E_INVAL;

    rc = snd_pcm_open(&mgr->pcm, mgr->device_name, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        LOG_ERROR("snd_pcm_open(%s): %s", mgr->device_name, snd_strerror(rc));
        return AUDIO_ERR;
    }

    snd_pcm_hw_params_malloc(&params);
    rc = set_hw_params(mgr, params, buffer_time_us, period_time_us);
    snd_pcm_hw_params_free(params);
    if (rc < 0) {
        LOG_ERROR("snd_pcm_hw_params: %s", snd_strerror(rc));
        snd_pcm_close(mgr->pcm);
        mgr->pcm = NULL;
        return AUDIO_ERR;
    }

    rc = snd_pcm_prepare(mgr->pcm);
    if (rc < 0) {
        LOG_ERROR("snd_pcm_prepare: %s", snd_strerror(rc));
        snd_pcm_close(mgr->pcm);
        mgr->pcm = NULL;
        return AUDIO_ERR;
    }

    /* compute frame size */
    mgr->frame_size = mgr->fmt.block_align;

    return AUDIO_OK;
}

/* copy + apply per-channel + master gain into ALSA mmap ring */
static int mmap_copy_apply_gain(struct audio_mgr *mgr,
                const void *src_void,
                snd_pcm_uframes_t frames,
                const snd_pcm_channel_area_t *areas,
                snd_pcm_uframes_t offset)
{
    const uint8_t *src = (const uint8_t *)src_void;
    unsigned ch = mgr->fmt.channels;
    unsigned bps = mgr->fmt.bits_per_sample;
    uint8_t *dst;
    snd_pcm_uframes_t i;

    /* compute dst pointer in mapped ALSA area */
    dst = (uint8_t *)areas[0].addr + (areas[0].first / 8) + offset * (areas[0].step / 8);

    /* handle common formats: U8, S16_LE, S24_LE (3 bytes), S32_LE */
    if (bps == 8) { /* unsigned 8-bit */
        for (i = 0; i < frames; ++i) {
            for (unsigned c = 0; c < ch; ++c) {
                float g = mgr->master_gain * mgr->ch_gain[c];
                int s = (int)src[c] - 128;
                int out = (int)(s * g) + 128;
                if (out < 0) out = 0; if (out > 255) out = 255;
                dst[c] = (uint8_t)out;
            }
            src += ch; dst += ch;
        }
        return AUDIO_OK;
    }

    if (bps == 16) {
        const int16_t *si = (const int16_t *)src;
        int16_t *di = (int16_t *)dst;
        for (i = 0; i < frames; ++i) {
            for (unsigned c = 0; c < ch; ++c) {
                float g = mgr->master_gain * mgr->ch_gain[c];
                int32_t v = (int32_t)((float)si[c] * g);
                if (v > 32767) v = 32767; if (v < -32768) v = -32768;
                di[c] = (int16_t)v;
            }
            si += ch; di += ch;
        }
        return AUDIO_OK;
    }

    if (bps == 24) {
        /* packed 3 bytes little endian */
        for (i = 0; i < frames; ++i) {
            for (unsigned c = 0; c < ch; ++c) {
                const uint8_t *s3 = src + c * 3;
                int32_t sv = s3[0] | (s3[1] << 8) | (s3[2] << 16);
                if (sv & 0x00800000) sv |= 0xFF000000; /* sign extend */
                float g = mgr->master_gain * mgr->ch_gain[c];
                int32_t v = (int32_t)((float)sv * g);
                if (v >  8388607) v =  8388607;
                if (v < -8388608) v = -8388608;
                uint8_t *d3 = dst + c * 3;
                d3[0] = (uint8_t)(v & 0xFF);
                d3[1] = (uint8_t)((v >> 8) & 0xFF);
                d3[2] = (uint8_t)((v >> 16) & 0xFF);
            }
            src += ch * 3; dst += ch * 3;
        }
        return AUDIO_OK;
    }

    if (bps == 32) {
        const int32_t *si = (const int32_t *)src;
        int32_t *di = (int32_t *)dst;
        for (i = 0; i < frames; ++i) {
            for (unsigned c = 0; c < ch; ++c) {
                double g = (double)mgr->master_gain * (double)mgr->ch_gain[c];
                long long v = (long long)((double)si[c] * g);
                if (v >  2147483647LL) v =  2147483647LL;
                if (v < -2147483648LL) v = -2147483648LL;
                di[c] = (int32_t)v;
            }
            si += ch; di += ch;
        }
        return AUDIO_OK;
    }

    return AUDIO_E_NOSUP;
}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/* Initialize the manager; device_name will be duplicated internally. */
int audio_mgr_init(struct audio_mgr *mgr, \
           const char *device_name, \
           snd_pcm_format_t pcm_format, \
           unsigned int channels, \
           unsigned int sample_rate, \
           unsigned int buffer_time_us, \
           unsigned int period_time_us)
{
    int rc;

    if (!mgr || !device_name) return AUDIO_E_INVAL;

    memset(mgr, 0, sizeof(*mgr));

    /* store device name copy for potential reinit */
    mgr->device_name = strdup(device_name);
    if (!mgr->device_name) return AUDIO_ERR;

    /* set desired format */
    mgr->fmt.pcm_format = pcm_format;
    mgr->fmt.channels = channels;
    mgr->fmt.sample_rate = sample_rate;
    mgr->fmt.bits_per_sample = snd_pcm_format_physical_width(pcm_format);
    mgr->fmt.block_align = (channels * mgr->fmt.bits_per_sample) / 8;

    mgr->master_gain = 1.0f;
    for (unsigned i = 0; i < AUDIO_MAX_CHANNELS; ++i) mgr->ch_gain[i] = 1.0f;

    /* default options */
    mgr->auto_reinit = 1;        /* default: allow auto reinit */
    mgr->skip_format_check = 0;  /* default: check formats */

    rc = internal_open_and_config(mgr, buffer_time_us, period_time_us);
    if (rc < 0) {
        free(mgr->device_name);
        mgr->device_name = NULL;
        return rc;
    }

    LOG_INFO("audio_mgr: opened device=%s format=%u ch=%u sr=%u bps=%u",
         mgr->device_name,
         (unsigned)mgr->fmt.pcm_format,
         mgr->fmt.channels,
         mgr->fmt.sample_rate,
         mgr->fmt.bits_per_sample);

    return AUDIO_OK;
}

/* Reinitialize manager with a new format. This closes existing handle and
 * configures ALSA again with same device_name saved from init.
 */
int audio_mgr_reinit(struct audio_mgr *mgr, \
             snd_pcm_format_t pcm_format, \
             unsigned int channels, \
             unsigned int sample_rate)
{
    if (!mgr || !mgr->device_name) return AUDIO_E_INVAL;

    /* close existing pcm if open */
    if (mgr->pcm) {
        snd_pcm_drain(mgr->pcm);
        snd_pcm_close(mgr->pcm);
        mgr->pcm = NULL;
    }

    /* update format fields */
    mgr->fmt.pcm_format = pcm_format;
    mgr->fmt.channels = channels;
    mgr->fmt.sample_rate = sample_rate;
    mgr->fmt.bits_per_sample = snd_pcm_format_physical_width(pcm_format);
    mgr->fmt.block_align = (channels * mgr->fmt.bits_per_sample) / 8;

    /* reopen/configure */
    return internal_open_and_config(mgr, 0, 0);
}

/* prepare (exposed) */
int audio_mgr_prepare(struct audio_mgr *mgr)
{
    if (!mgr || !mgr->pcm) return AUDIO_E_INVAL;
    int rc = snd_pcm_prepare(mgr->pcm);
    if (rc < 0) {
        LOG_ERROR("snd_pcm_prepare: %s", snd_strerror(rc));
        return AUDIO_ERR;
    }
    return AUDIO_OK;
}

/* release resources */
void audio_mgr_release(struct audio_mgr *mgr)
{
    if (!mgr) return;
    if (mgr->pcm) {
        snd_pcm_drain(mgr->pcm);
        snd_pcm_close(mgr->pcm);
        mgr->pcm = NULL;
    }
    if (mgr->device_name) {
        free(mgr->device_name);
        mgr->device_name = NULL;
    }
    memset(mgr, 0, sizeof(*mgr));
    LOG_INFO("Audio system has been released");
}

/* set options */
int audio_mgr_set_auto_reinit(struct audio_mgr *mgr, int enable)
{
    if (!mgr) return AUDIO_E_INVAL;
    mgr->auto_reinit = enable ? 1 : 0;
    LOG_DEBUG("auto_reinit=%d", mgr->auto_reinit);
    return AUDIO_OK;
}

int audio_mgr_set_skip_format_check(struct audio_mgr *mgr, int enable)
{
    if (!mgr) return AUDIO_E_INVAL;
    mgr->skip_format_check = enable ? 1 : 0;
    LOG_DEBUG("skip_format_check=%d", mgr->skip_format_check);
    return AUDIO_OK;
}

/* gain control */
int audio_mgr_set_master_gain(struct audio_mgr *mgr, float gain)
{
    if (!mgr) return AUDIO_E_INVAL;
    mgr->master_gain = clamp01(gain);
    LOG_DEBUG("master_gain=%.3f", mgr->master_gain);
    return AUDIO_OK;
}

int audio_mgr_set_channel_gain(struct audio_mgr *mgr, unsigned int ch, float gain)
{
    if (!mgr) return AUDIO_E_INVAL;
    if (ch >= mgr->fmt.channels || ch >= AUDIO_MAX_CHANNELS) return AUDIO_E_INVAL;
    mgr->ch_gain[ch] = clamp01(gain);
    LOG_DEBUG("channel[%u]_gain=%.3f", ch, mgr->ch_gain[ch]);
    return AUDIO_OK;
}

int audio_mgr_set_channel_gains(struct audio_mgr *mgr, const float *gains, unsigned int n)
{
    if (!mgr || !gains) return AUDIO_E_INVAL;
    if (n < mgr->fmt.channels) return AUDIO_E_INVAL;
    if (mgr->fmt.channels > AUDIO_MAX_CHANNELS) return AUDIO_E_INVAL;
    for (unsigned i = 0; i < mgr->fmt.channels; ++i) mgr->ch_gain[i] = clamp01(gains[i]);
    LOG_DEBUG("set_channel_gains: applied %u channels", mgr->fmt.channels);
    return AUDIO_OK;
}

/* Write 'frames' frames from src into ALSA ring using mmap interface.
 * src must be interleaved PCM matching mgr->fmt.
 */
int audio_mgr_write_mmap(struct audio_mgr *mgr, const void *src, size_t frames)
{
    const snd_pcm_channel_area_t *areas;
    snd_pcm_uframes_t offset, avail;
    snd_pcm_sframes_t rc;
    size_t left;

    if (!mgr || !mgr->pcm || !src) return AUDIO_E_INVAL;

    left = frames;

    while (left > 0) {
        rc = snd_pcm_avail_update(mgr->pcm);
        if (rc < 0) {
            rc = recover_xrun(mgr->pcm, rc);
            if (rc < 0) {
                LOG_ERROR("snd_pcm_avail_update error: %s", snd_strerror((int)rc));
                return AUDIO_ERR;
            }
            continue;
        }

        if (rc == 0) { /* nothing available: wait a bit */
            snd_pcm_wait(mgr->pcm, 1000);
            continue;
        }

        rc = snd_pcm_mmap_begin(mgr->pcm, &areas, &offset, &avail);
        if (rc < 0) {
            rc = recover_xrun(mgr->pcm, rc);
            if (rc < 0) {
                LOG_ERROR("snd_pcm_mmap_begin error: %s", snd_strerror((int)rc));
                return AUDIO_ERR;
            }
            continue;
        }

        if ((snd_pcm_uframes_t)left < avail) avail = (snd_pcm_uframes_t)left;

        /* apply gain while copying into ALSA buffer */
        int mrc = mmap_copy_apply_gain(mgr, src, avail, areas, offset);
        if (mrc < 0) {
            (void)snd_pcm_mmap_commit(mgr->pcm, offset, 0);
            LOG_ERROR("mmap copy apply gain failed");
            return mrc;
        }

        rc = snd_pcm_mmap_commit(mgr->pcm, offset, avail);
        if (rc < 0) {
            rc = recover_xrun(mgr->pcm, rc);
            if (rc < 0) {
                LOG_ERROR("snd_pcm_mmap_commit error: %s", snd_strerror((int)rc));
                return AUDIO_ERR;
            }
            continue;
        }

        src = (const uint8_t *)src + avail * mgr->frame_size;
        left -= avail;
    }

    return AUDIO_OK;
}
