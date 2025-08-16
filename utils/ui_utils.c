#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

struct wav_header {
	char riff_id[4];       /* "RIFF" */
	uint32_t riff_size;
	char wave_id[4];       /* "WAVE" */
	char fmt_id[4];        /* "fmt " */
	uint32_t fmt_size;
	uint16_t audio_format; /* 1 = PCM */
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	char data_id[4];       /* "data" */
	uint32_t data_size;
} __attribute__((packed));

int main(int argc, char *argv[])
{
	FILE *fp;
	struct wav_header hdr;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_format_t pcm_fmt;
	const char *dev_name;
	const char *file_path;
	int dir;
	int rc;
	size_t read_size;
	char *buffer;

	dev_name = "default";
	file_path = "/usr/share/sounds/alsa/Front_Left.wav";

	if (argc >= 2)
		dev_name = argv[1];

	fp = fopen(file_path, "rb");
	if (!fp) {
		perror("fopen");
		return 1;
	}

	if (fread(&hdr, sizeof(hdr), 1, fp) != 1) {
		fprintf(stderr, "failed to read wav header\n");
		fclose(fp);
		return 1;
	}

	if (memcmp(hdr.riff_id, "RIFF", 4) || memcmp(hdr.wave_id, "WAVE", 4)) {
		fprintf(stderr, "not a valid wav file\n");
		fclose(fp);
		return 1;
	}

	if (hdr.audio_format != 1) {
		fprintf(stderr, "unsupported format (only PCM)\n");
		fclose(fp);
		return 1;
	}

	switch (hdr.bits_per_sample) {
	case 8:
		pcm_fmt = SND_PCM_FORMAT_U8;
		break;
	case 16:
		pcm_fmt = SND_PCM_FORMAT_S16_LE;
		break;
	case 24:
		pcm_fmt = SND_PCM_FORMAT_S24_LE;
		break;
	case 32:
		pcm_fmt = SND_PCM_FORMAT_S32_LE;
		break;
	default:
		fprintf(stderr, "unsupported bits per sample: %u\n",
			hdr.bits_per_sample);
		fclose(fp);
		return 1;
	}

	rc = snd_pcm_open(&handle, dev_name, SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr, "snd_pcm_open: %s\n", snd_strerror(rc));
		fclose(fp);
		return 1;
	}

	snd_pcm_hw_params_malloc(&params);
	snd_pcm_hw_params_any(handle, params);
	snd_pcm_hw_params_set_access(handle, params,
				     SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(handle, params, pcm_fmt);
	snd_pcm_hw_params_set_channels(handle, params, hdr.num_channels);
	snd_pcm_hw_params_set_rate_near(handle, params,
					&hdr.sample_rate, 0);
	snd_pcm_hw_params(handle, params);
	snd_pcm_hw_params_free(params);
	snd_pcm_prepare(handle);

	buffer = malloc(hdr.block_align * 1024);
	if (!buffer) {
		perror("malloc");
		snd_pcm_close(handle);
		fclose(fp);
		return 1;
	}

	while ((read_size = fread(buffer, 1,
			hdr.block_align * 1024, fp)) > 0) {
		rc = snd_pcm_writei(handle, buffer,
				    read_size / hdr.block_align);
		if (rc == -EPIPE) {
			snd_pcm_prepare(handle);
		} else if (rc < 0) {
			fprintf(stderr, "snd_pcm_writei: %s\n",
				snd_strerror(rc));
			break;
		}
	}

	free(buffer);
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	fclose(fp);

	return 0;
}
