
/**
 * @file internal_comm.c
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

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/eventfd.h>

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
int32_t event_fd = -1;

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
int32_t event_set(int32_t evfd, uint64_t code)
{
    ssize_t ret;
    uint64_t val;

    val = code;
    ret = write(evfd, &val, sizeof(val));
    if (ret != sizeof(val)) {
        LOG_TRACE("event_set failed: evfd=%d, ret=%zd, err=%d(%s)", \
                  evfd, ret, errno, strerror(errno));
        return -EIO;
    }

    return 0;
}

int32_t event_get(int32_t evfd, uint64_t *out_val)
{
    ssize_t ret;
    uint64_t val;

    ret = read(evfd, &val, sizeof(val));
    if (ret != sizeof(val)) {
        if (ret < 0 && errno == EAGAIN)
            return -EAGAIN;

        LOG_TRACE("event_get failed: evfd=%d, ret=%zd, err=%d(%s)", \
                  evfd, ret, errno, strerror(errno));
        return -EIO;
    }

    *out_val = val;
    return 0;
}

int32_t init_event_file(void)
{
    int32_t fd;

    fd = eventfd(0, EFD_NONBLOCK);
    if (fd == -1) {
        LOG_TRACE("init_event_file failed: err=%d(%s)", \
                  errno, strerror(errno));
        return -errno;
    }

    event_fd = fd;
    return 0;
}

int32_t cleanup_event_file(void)
{
    int32_t ret;

    if (event_fd == -1)
        return 0;

    ret = close(event_fd);
    if (ret == -1) {
        LOG_TRACE("cleanup_event_file failed: fd=%d, err=%d(%s)", \
                  event_fd, errno, strerror(errno));
        return -errno;
    }

    event_fd = -1;
    return 0;
}
