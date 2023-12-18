/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _CLI_CMD_H_
#define _CLI_CMD_H_

#include <linux/genetlink.h>

extern void show_invalid_para(char * cmd_name);
void *cmd_excu(void *argv);
void cli_set_sig_pipefd(int fd);
void ctl_set_sig_pipefd(int fd);

#endif /* _CLI_CMD_H_ */

