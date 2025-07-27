    
#include <log.h>
#include <task_handler.h>

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
