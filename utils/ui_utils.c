#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#define MAX_PATH_LEN 64

static int open_event_device(int id)
{
    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "/dev/input/event%d", id);
    return open(path, O_RDWR);
}

static void usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s <event_id> <ff_type> <duration_ms>\n"
        "  ff_type:\n"
        "    80: FF_RUMBLE\n"
        "    81: FF_PERIODIC (FF_SINE)\n"
        "    82: FF_CONSTANT\n"
        "    83: FF_SPRING\n"
        "    84: FF_FRICTION\n"
        "    88: FF_SQUARE\n"
        "    89: FF_TRIANGLE\n"
        "    90: FF_SINE\n"
        "    91: FF_SAW_UP\n"
        "    92: FF_SAW_DOWN\n"
        "\n", prog);
}

int main(int argc, char *argv[])
{
    int fd, event_id, ff_type, duration;
    struct ff_effect effect;
    struct input_event play;

    if (argc < 4) {
        usage(argv[0]);
        return 1;
    }

    event_id = atoi(argv[1]);
    ff_type = atoi(argv[2]);
    duration = atoi(argv[3]);

    fd = open_event_device(event_id);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    memset(&effect, 0, sizeof(effect));
    effect.type = ff_type;
    effect.id = -1;
    effect.replay.length = duration;
    effect.replay.delay = 0;

    switch (ff_type) {
    case FF_RUMBLE:
        effect.u.rumble.strong_magnitude = 0xffff;
        effect.u.rumble.weak_magnitude = 0x8000;
        break;

    case FF_PERIODIC:
    case FF_SINE:
    case FF_SQUARE:
    case FF_TRIANGLE:
    case FF_SAW_UP:
    case FF_SAW_DOWN:
        effect.u.periodic.waveform = ff_type;
        effect.u.periodic.magnitude = 0x7fff;
        effect.u.periodic.period = 100; // ms
        effect.u.periodic.offset = 0;
        effect.u.periodic.phase = 0;
        effect.u.periodic.envelope.attack_length = 50;
        effect.u.periodic.envelope.attack_level = 0x1000;
        effect.u.periodic.envelope.fade_length = 50;
        effect.u.periodic.envelope.fade_level = 0x1000;
        break;

    case FF_CONSTANT:
        effect.u.constant.level = 0x5000;
        effect.u.constant.envelope.attack_length = 100;
        effect.u.constant.envelope.attack_level = 0x2000;
        effect.u.constant.envelope.fade_length = 100;
        effect.u.constant.envelope.fade_level = 0x2000;
        break;

    default:
        fprintf(stderr, "Unsupported ff_type: %d\n", ff_type);
        close(fd);
        return 1;
    }

    if (ioctl(fd, EVIOCSFF, &effect) < 0) {
        perror("EVIOCSFF");
        close(fd);
        return 1;
    }

    play.type = EV_FF;
    play.code = effect.id;
    play.value = 1;

    if (write(fd, &play, sizeof(play)) < 0) {
        perror("write");
        close(fd);
        return 1;
    }

    usleep(duration * 1000);

    ioctl(fd, EVIOCRMFF, effect.id);

    close(fd);
    return 0;
}
