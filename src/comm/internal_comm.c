
/**
 * @file internal_comm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
// #define LOG_LEVEL LOG_LEVEL_TRACE
#if defined(LOG_LEVEL)
#warning "LOG_LEVEL=" TOSTRING(LOG_LEVEL) ", will take precedence in this file."
#endif
#include <log.h>

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/eventfd.h>

#include <sys_comm.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
int32_t event_fd;

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
void event_set(int evfd, uint64_t code)
{
    uint64_t val = code;
    write(evfd, &val, sizeof(val));
}

uint64_t event_get(int evfd)
{
    uint64_t val;
    read(evfd, &val, sizeof(uint64_t));
    return val;
}

int init_event_file()
{
    event_fd = eventfd(0, EFD_NONBLOCK);
    if (event_fd == -1) {
        return -1;
    }
    return 0;
}

