/**
 * @file sys_comm.h
 *
 */

#ifndef G_SYS_COMM_H
#define G_SYS_COMM_H

#include <stddef.h>

int init_event_file();
void event_set(int evfd, uint64_t code);
uint64_t event_get(int evfd);

// fs_comm
int fs_write_file(const char *path, const char *data, size_t len);
int fs_append_file(const char *path, const char *data, size_t len);
char *fs_read_file(const char *path, size_t *out_len);
int fs_file_exists(const char *path);

// proc_comm
int exec_cmd_with_interact(char *cmd, char *input);


#endif /* G_SYS_COMM_H */
