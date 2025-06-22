/**
 * @file proc_comm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdbool.h>

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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int cmd_executor(char *cmd, bool interact, char *input, char *output)
{
    FILE *pfd;
    int ch;
    int cnt = 0;

    if (!interact) {
        pfd = popen(cmd, "r");
        if (pfd == NULL) {
            LOG_ERROR("Unable to open process");
            return 1;
        }
        while((ch = fgetc(pfd)) != EOF)
            output[cnt++] = ch;
    } else {
        pfd = popen(cmd, "w");
        if (pfd == NULL) {
            LOG_ERROR("Unable to open process");
            return 1;
        }
        fprintf(pfd, "%s", input);
    }

    pclose(pfd);
    return cnt;
}

int exec_cmd_with_interact(char *cmd, char *input)
{
    return cmd_executor(cmd, true, input, NULL);
}

int exec_cmd_get_output(char *cmd, char *output)
{
    return cmd_executor(cmd, false, NULL, output);
}
