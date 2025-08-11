    
#include <log.h>
#include <task_handler.h>

#include <imu.h>

remote_cmd_t *create_remote_cmd(void)
{
	remote_cmd_t *cmd;

	cmd = calloc(1, sizeof(*cmd));
	if (!cmd) {
		return NULL;
	}

	return cmd;
}

void delete_remote_cmd(remote_cmd_t *cmd)
{
	if (!cmd) {
		LOG_WARN("Unable to delete cmd: null pointer");
		return;
	}

	free(cmd);
}

local_cmd_t *create_local_cmd()
{
    local_cmd_t *cmd = NULL;

    cmd = calloc(1, sizeof(*cmd));
    if (!cmd) {
		return NULL;
    }

    return cmd;
}


void delete_local_cmd(local_cmd_t *cmd)
{
	if (!cmd) {
		LOG_WARN("Unable to delete cmd: null pointer");
		return;
	}

	free(cmd);
}

int process_opcode(uint32_t opcode, void *data)
{
    switch (opcode) {
    case OP_ID_LEFT_VIBRATOR:
        rumble_trigger(2, 80, 150);
        break;
    case OP_ID_RIGHT_VIBRATOR:
        rumble_trigger(3, 80, 150);
        break;
    case OP_ID_START_IMU:
        imu_kalman_init("/sys/bus/iio/devices/iio:device2/", 100, 0.001f, 0.003f, 0.03f);
        imu_kalman_set_debug(1);
        imu_kalman_start();
        break;
    case OP_ID_STOP_IMU:
        imu_kalman_stop();
        break;
    case OP_ID_READ_IMU:
        struct imu_angles a = imu_kalman_get_angles();
        LOG_DEBUG("roll=%.2f pitch=%.2f yaw=%.2f\n", a.roll, a.pitch, a.yaw);
        break;
    default:
        LOG_ERROR("Opcode [%d] is invalid");
        break;
    }

}
