/*
 * Copyright (c) 2020 iComm-semi Ltd.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file app_ctl.c
 * @brief application control functions.
 */


/*******************************************************************************
 *         Include Files
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "app_ctl.h"
#include "cli.h"
#include "ssv_netlink.h"


/*******************************************************************************
 *         Local Defines
 ******************************************************************************/


/*******************************************************************************
 *         Local Enumerations
 ******************************************************************************/


/*******************************************************************************
 *         Local Structures
 ******************************************************************************/


/*******************************************************************************
 *         Global Variables
 ******************************************************************************/


/*******************************************************************************
 *         Local Variables
 ******************************************************************************/
int _g_ctl_sig_pipefd[2];
int _g_cli_sig_pipefd[2];
extern int cli_isstop;

static pthread_t _g_thread_wifi_ctl = 0;
static pthread_t _g_thread_wifi_cli = 0;


/*******************************************************************************
 *         Local Functions
 ******************************************************************************/
extern void *cmd_excu(void *argv);
extern void cli_set_sig_pipefd(int fd);
extern void ctl_set_sig_pipefd(int fd);


/*******************************************************************************
 *         Global Functions
 ******************************************************************************/
int app_ctl_start(void)
{
    int nl_fd = 0;

    nl_fd = ssv_netlink_start();

    if(nl_fd == 0)
    {
        printf("please install driver \n");
        return -1;
    }

    pipe(_g_ctl_sig_pipefd);
    pipe(_g_cli_sig_pipefd);
    cli_set_sig_pipefd(_g_cli_sig_pipefd[0]);
    ctl_set_sig_pipefd(_g_ctl_sig_pipefd[0]);

    pthread_create(&_g_thread_wifi_ctl, NULL, cmd_excu, &nl_fd);
    pthread_create(&_g_thread_wifi_cli, NULL, Cli_Init, NULL);

    pthread_join(_g_thread_wifi_cli, NULL);
    pthread_join(_g_thread_wifi_ctl, NULL);

    return 0;
}

void app_ctl_stop(int signum)
{
    cli_isstop = 1;
    write(_g_ctl_sig_pipefd[1], &signum, sizeof(signum));
    write(_g_cli_sig_pipefd[1], &signum, sizeof(signum));
    close(_g_ctl_sig_pipefd[1]);
    close(_g_cli_sig_pipefd[1]);
    ssv_netlink_stop();
}
