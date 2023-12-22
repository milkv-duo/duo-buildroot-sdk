/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#include <stdlib.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>

#include "../../ssv_ctl_common.h"
#include "../../wifi_app_common.h"
#include "../../app_ctl.h"

#include "ssv_netlink.h"
#include "../cli.h"
#include "cli_cmd.h"


int cli_isstop = 0;

static int _wifi_ctl_app_create_drvcmdbuf(unsigned short cmdval, char *inbuf, int inbuflen, char **outbuf, int *outbuflen)
{
    ST_WIFI_DRV_CMD *cmd = NULL;
    int cmdlen = 0;

    cmd = (ST_WIFI_DRV_CMD *)malloc(WIFI_DRV_CMD_HDR_LEN+inbuflen);
    if(cmd == NULL)
    {
        printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, (unsigned int)(WIFI_APP_CMD_HDR_LEN+inbuflen));
        return -1;
    }

    cmd->cmdid = cmdval;
    cmd->datalen = inbuflen;
    memcpy(cmd->data, inbuf, inbuflen);
    cmdlen = WIFI_DRV_CMD_HDR_LEN+inbuflen;

    *outbuf = (char *)cmd;
    *outbuflen = cmdlen;

    return 0;
}

static int _wifi_ctl_app_create_appcmdbuf(unsigned short cmdval, char *inbuf, int inbuflen, char **outbuf, int *outbuflen)
{
    ST_WIFI_APP_CMD *cmd = NULL;
    int cmdlen = 0;

    cmd = (ST_WIFI_APP_CMD *)malloc(WIFI_APP_CMD_HDR_LEN+inbuflen);
    if(cmd == NULL)
    {
        printf("[%s] malloc(%u) failed!!\n", __FUNCTION__, (unsigned int)(WIFI_APP_CMD_HDR_LEN+inbuflen));
        return -1;
    }

    cmd->cmdid = cmdval;
    cmd->datalen = inbuflen;
    memcpy(cmd->data, inbuf, inbuflen);
    cmdlen = WIFI_APP_CMD_HDR_LEN+inbuflen;
    printf("inbuf len is %d\n",inbuflen);
    *outbuf = (char *)cmd;
    *outbuflen = cmdlen;

    return 0;
}

static void _wifi_ctl_app_release_cmdbuf(char **cmdbuf)
{
    if(NULL != *cmdbuf)
    {
        free(*cmdbuf);
        *cmdbuf = NULL;
    }
}

static int cmd_excu_operation_cmd(struct netlink_msg *msg)
{
    struct nlattr *na;
    ST_WIFI_REG_PARAM *reg = NULL;

    na = (struct nlattr *) GENLMSG_DATA(msg);
    //printf("nla_type : %d\n", na->nla_type);
    if(na->nla_type == E_SSV_CTL_ATTR_OPERACMD)
    {
        //printf("Terminate app, it only support this in currently.");
        return -1;
    }

    if (na->nla_type == E_SSV_CTL_ATTR_OPERACMD_RSP)
    {
        reg = (ST_WIFI_REG_PARAM *)NLA_DATA(na);
        printf("reg read:0x%08x = 0x%08x\n",reg->address,reg->value);
    }
    return 0;
}

static int _g_ctl_sig_pipefd = -1;
void ctl_set_sig_pipefd(int fd)
{
    _g_ctl_sig_pipefd = fd;
}

#define MAX_EVENTS 2
void *cmd_excu(void *argv)
{
    struct netlink_msg msg;
    int epollfd;
    struct epoll_event ev, events[MAX_EVENTS];
    int sock = *((int *) argv);
    int nfds, i, len;

    epollfd = epoll_create1(0);
    if(epollfd == -1)
    {
        perror("epoll_create\n");
        return NULL;
    }

    ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP | EPOLLPRI;
    ev.data.fd = sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) == -1)
    {
        perror("epoll_ctl: sock");
        goto exit;
    }

    ev.events = EPOLLIN;
    ev.data.fd = _g_ctl_sig_pipefd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, _g_ctl_sig_pipefd, &ev) == -1)
    {
        perror("epoll_ctl: ctl sig");
        goto exit;
    }

    while(1)
    {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            goto exit;
        }

        for(i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == _g_ctl_sig_pipefd)
            {
                int sig = 0;
                read(_g_ctl_sig_pipefd, &sig, sizeof(sig));
                goto exit;
            }
            if (events[i].data.fd == sock)
            {
                if(events[i].events | EPOLLIN)
                {
                    len = recv(sock, &msg, sizeof(msg), 0);
                    if(len > 0)
                    {
                        if (msg.n.nlmsg_type == NLMSG_ERROR)
                        {
                            printf("Error, receive NACK\n");
                            goto exit;
                        }
                        if (!NLMSG_OK((&msg.n), len))
                        {
                             printf("Invalid reply message received via Netlink\n");
                             goto exit;
                        }

                        switch (msg.g.cmd) {
                            case E_SSV_CTL_CMD_TO_DRV_OPERACMD:
                                if(-1 == cmd_excu_operation_cmd(&msg))
                                {
                                    goto exit;
                                }
                                break;
                            default:
                                printf("Unknown exce command %d\n", msg.g.cmd);
                                goto exit;
                        }
                    }
                    else
                    {
                        //printf("sock closed:%d\n", len);
                        break;
                    }
                }
            }
        }
    }

exit:
    close(epollfd);
    return NULL;
}

void cmd_regr(int argc, char *argv[])
{
    char *cmd_buf = NULL;
    int cmd_buflen = 0;
    char *pkey = 0;
    char *pnum = 0;
    ST_WIFI_REG_PARAM *reg = NULL;
    if(1 < argc)
    {
        reg = (ST_WIFI_REG_PARAM *)malloc(sizeof(ST_WIFI_REG_PARAM));

        if(NULL == reg)
        {
            printf("regr malloc failed \n");
        }

        if(!strcmp(argv[1],"help"))
        {
            printf("cmd_regr usage:\n");
            printf("regr <address> [count]\n");
        }
        else if(argv[1] != NULL && argv[2] != NULL)
        {
            pkey = argv[1];
            pnum = argv[2];
            reg->address = strtoul(pkey,NULL,16);
            reg->value = strtoul(pnum,NULL,10);
            //printf("cmd regr address %x count %x",reg->address,reg->value);
            }
        else if(argv[1] != NULL && argv[2] == NULL )
        {
            pkey = argv[1];
            //pnum = argv[2];
            reg->address = strtoul(pkey,NULL,16);
            reg->value = 0;
            //printf("cmd regr address %x count %x",reg->address,reg->value);
        }
        _wifi_ctl_app_create_drvcmdbuf((unsigned short)E_SSV_OPER_REGR, (char *)reg, sizeof(ST_WIFI_REG_PARAM), &cmd_buf, &cmd_buflen);
        ssv_netlink_send_cmd(cmd_buf,cmd_buflen , (int)E_SSV_CTL_CMD_TO_DRV_OPERACMD, (int)E_SSV_CTL_ATTR_OPERACMD);
        _wifi_ctl_app_release_cmdbuf(&cmd_buf);

        free(reg);
    }
    else
    {
        printf("wrong parameter,please type register address and count number \n");
    }

    printf("\n");

}

void cmd_regw(int argc, char *argv[])
{
    char *cmd_buf = NULL;
    int cmd_buflen = 0;
    char *pkey = 0;
    char *pnum = 0;
    ST_WIFI_REG_PARAM *reg = NULL;
    if(2 < argc)
    {
        reg = (ST_WIFI_REG_PARAM *)malloc(sizeof(ST_WIFI_REG_PARAM));

        if(NULL == reg)
        {
            printf("regw malloc failed \n");
        }

        if(!strcmp(argv[1],"help"))
        {
            printf("cmd_regw usage:\n");
            //printf("reg.\n");
            printf("regw <address> [value]\n");
        }
        else if(argv[1] != NULL && argv[2] != NULL)
        {
            pkey = argv[1];
            pnum = argv[2];
            reg->address = strtoul(pkey,NULL,16);
            reg->value = strtoul(pnum,NULL,16);
            printf("regw write address:%x,value %x",reg->address,reg->value);
        }
        _wifi_ctl_app_create_drvcmdbuf((unsigned short)E_SSV_OPER_REGW, (char *)reg, sizeof(ST_WIFI_REG_PARAM), &cmd_buf, &cmd_buflen);
        ssv_netlink_send_cmd(cmd_buf,cmd_buflen , (int)E_SSV_CTL_CMD_TO_DRV_OPERACMD, (int)E_SSV_CTL_ATTR_OPERACMD);
        _wifi_ctl_app_release_cmdbuf(&cmd_buf);

        free(reg);
    }
    else
    {
        printf("wrong parameter,please type register address and value \n");
    }

    printf("\n");

}
void cmd_exit(int argc, char *argv[])
{
    app_ctl_stop(SIGTERM);
}


CLICmds gCliCmdTable[] =
{
    { "regr",    cmd_regr,    "REGR command"},
    { "regw",    cmd_regw,    "REGW command"},
    { "exit",    cmd_exit,    "exit"},
    { (const char *)NULL, (CliCmdFunc)NULL, (const char *)NULL},
};
