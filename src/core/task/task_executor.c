#include <stdlib.h>
#include <stdint.h>

#include <dbus_comm.h>

#include <workqueue.h>
#include <task.h>
#include <log.h>


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

int process_opcode_endless(uint32_t opcode, void *data)
{
    int rc = 0;

    switch (opcode) {
    case OP_ID_START_DBUS:
        rc = dbus_fn_thread_handler();
        break;
    case OP_ID_START_IMU:
        break;
    default:
        LOG_ERROR("Opcode [%d] is invalid");
        break;
    }

    return rc;
}

int process_opcode(uint32_t opcode, void *data)
{
    int rc = 0;

    switch (opcode) {
    case OP_ID_LEFT_VIBRATOR:
        break;
    case OP_ID_RIGHT_VIBRATOR:
        break;
    case OP_ID_STOP_IMU:
        break;
    case OP_ID_READ_IMU:
        break;
    default:
        LOG_ERROR("Opcode [%d] is invalid");
        break;
    }

    return rc;
}

int create_local_simple_task(uint8_t flow, uint8_t duration, uint32_t opcode)
{
    work_t *work = create_work(LOCAL, flow, duration, opcode, NULL);
    if (!work) {
        LOG_ERROR("Failed to create work from cmd");
        return EXIT_FAILURE;
    }

    push_work(work);

    return EXIT_SUCCESS;
}
