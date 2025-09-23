/**
 * @file fs_comm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL defined locally will override the global setting in this file"
#endif
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "comm/f_comm.h"

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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static int32_t __fs_write_internal(const char *path,  const char *data, \
                                  size_t len, int32_t append)
{
    int32_t fd, ret, flags;

    if (!path || !data || len == 0) {
        LOG_ERROR("invalid argument");
        return -EINVAL;
    }

    flags = O_WRONLY | O_CREAT;
    flags |= append ? O_APPEND : O_TRUNC;

    LOG_TRACE("%s to %s, len=%zu", \
         append ? "append" : "write", path, len);
    LOG_TRACE("write data:\n%.*s", (int)len, data);

    fd = open(path, flags, 0644);
    if (fd < 0) {
        LOG_ERROR("open failed: %s", strerror(errno));
        return -EIO;
    }

    ret = write(fd, data, len);
    if (ret < 0) {
        LOG_ERROR("write failed: %s", strerror(errno));
        close(fd);
        return ret;
    }

    close(fd);
    LOG_TRACE("%s success", append ? "append" : "write");
    return 0;
}

int32_t fs_write_file(const char *path, const char *data, size_t len)
{
    return __fs_write_internal(path, data, len, 0);
}

int32_t fs_append_file(const char *path, const char *data, size_t len)
{
    return __fs_write_internal(path, data, len, 1);
}

int32_t fs_read_file(const char *path, char *buf, size_t buf_len, \
                     size_t *out_len)
{
    int fd, ret;
    ssize_t size;

    if (!path || !buf || buf_len == 0 || !out_len)
        return -EINVAL;

    fd = open(path, O_RDONLY);
    if (fd < 0)
        return -errno;

    size = read(fd, buf, buf_len - 1);
    if (size < 0) {
        ret = -errno;
        close(fd);
        return ret;
    }

    buf[size] = '\0';
    *out_len = size;

    close(fd);
    return 0;
}

int32_t fs_file_exists(const char *path)
{
    struct stat st;

    if (!path) {
        LOG_ERROR("invalid argument");
        return -EINVAL;
    }

    if (stat(path, &st) == 0) {
        LOG_DEBUG("file exists: %s", path);
        return 1;
    }

    LOG_DEBUG("file not found: %s", path);
    return -ENOENT;
}
