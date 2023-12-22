/*
 * Copyright (c) 2015 iComm-semi Ltd.
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

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/epoll.h>
#include "../ssv_ctl_common.h"
#include "ssv_netlink.h"
#include "../wifi_app_common.h"

static int gnl_id = SSV_CTL_NL_ID;
int g_gnl_fd = 0;

int genl_send_msg(int sock_fd, u_int16_t family_id, u_int32_t nlmsg_pid,
            u_int8_t genl_cmd, u_int8_t genl_version, u_int16_t nla_type,
            void *nla_data, int nla_len)
{
    struct nlattr *na;
    struct sockaddr_nl dst_addr;
    int r, buflen;
    char *buf;
    struct netlink_msg msg;
    
    if (0 == family_id)
    {
        return 0;
    }
    
    msg.n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
    msg.n.nlmsg_type = family_id;

    msg.n.nlmsg_flags = NLM_F_REQUEST;
    msg.n.nlmsg_seq = 0;
    msg.n.nlmsg_pid = nlmsg_pid;

    msg.g.cmd = genl_cmd;
    msg.g.version = genl_version;
    na = (struct nlattr *)GENLMSG_DATA(&msg);
    na->nla_type = nla_type;
    na->nla_len = nla_len + 1 + NLA_HDRLEN;
    memcpy(NLA_DATA(na), nla_data, nla_len);
    msg.n.nlmsg_len += NLMSG_ALIGN(na->nla_len);
    buf = (char *)&msg;
    buflen = msg.n.nlmsg_len ;

    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.nl_family = AF_NETLINK;
    dst_addr.nl_pid = 0; 
    dst_addr.nl_groups = 0; 
    while ((r = sendto(sock_fd, buf, buflen, 0, (struct sockaddr *)&dst_addr
            , sizeof(dst_addr))) < buflen)
    {
        if (0 < r)
        {
            buf += r;
            buflen -= r;
        }
        else if (EAGAIN != errno)
        {
            return -1;
        }
    }
    return 0;
}



static int genl_get_family_id(int sock_fd, char *family_name)
{
    struct netlink_msg ans;
    int id, rc;
    struct nlattr *na;
    int rep_len;
    
    rc = genl_send_msg(sock_fd, GENL_ID_CTRL, 0, CTRL_CMD_GETFAMILY, 1,
                      CTRL_ATTR_FAMILY_NAME, (void *)family_name,
                      strlen(family_name)+1);

    if(0 != rc)
    {
        printf("Fail to send msg to gset genl id!!\n");
    }
    
    rep_len = recv(sock_fd, &ans, sizeof(ans), 0);
    if (0 > rep_len)
    {
        return 1;
    }
    if ((NLMSG_ERROR == ans.n.nlmsg_type) || (!NLMSG_OK((&ans.n), rep_len)))
    {
            return 1;
    }
    na = (struct nlattr *)GENLMSG_DATA(&ans);
    na = (struct nlattr *)((char *)na + NLA_ALIGN(na->nla_len));
    if (CTRL_ATTR_FAMILY_ID == na->nla_type)
    {
        id = *(__u16 *) NLA_DATA(na);
    }
    else
    {
        id = 0;
    }
    return id;
}

static int _ssv_netlink_init(void)
{
    int fd;
    struct sockaddr_nl local;
    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    if (fd < 0)
    {
        printf("fail to create netlink socket\n");
        return -1;
    }
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_groups = 0;
    if (bind(fd, (struct sockaddr *) &local, sizeof(local)) < 0)
        goto error;

    gnl_id = genl_get_family_id(fd ,"SSV_CTL");

    return fd;
error:
    close(fd);
    return -1;
}

static int _ssv_netlink_close(int *psock_fd)
{
    if (psock_fd)
    {
        if (*psock_fd > 0)
        {
            close(*psock_fd);
        }
        *psock_fd = -1;
    }
    return 0;
}

static int _netlink_send_cmd(int sock, char *buf, int buflen, int cmdid, int cmdattr)
{
    struct netlink_msg msg;
    struct nlattr *na;
    int mlength, retval;
    struct sockaddr_nl nladdr;

    msg.n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
    msg.n.nlmsg_type = gnl_id;
    msg.n.nlmsg_flags = NLM_F_REQUEST;
    msg.n.nlmsg_seq = 0;
    msg.n.nlmsg_pid = getpid();

    msg.g.cmd = (__u8)cmdid;

    na = (struct nlattr *) GENLMSG_DATA(&msg);
    na->nla_type = (__u16)cmdattr;
    mlength = buflen;
    na->nla_len = (__u16)(mlength+NLA_HDRLEN);
    memcpy((void *)NLA_DATA(na), (const void *)buf, mlength);
    msg.n.nlmsg_len += NLMSG_ALIGN(na->nla_len);
    memset(&nladdr, 0, sizeof(nladdr));

    nladdr.nl_family = AF_NETLINK;
    retval = sendto(sock, (char *)&msg, msg.n.nlmsg_len, 0,
                    (struct sockaddr *) &nladdr, sizeof(nladdr));

    if (retval < 0)
    {
        printf("Fail to send message to kernel\n");
    }
    return retval;
}

int ssv_netlink_send_custom_cmd(char *buf, int buflen)
{
    //return _netlink_send_cmd(g_gnl_fd, buf, buflen, (int)E_SSV_CTL_CMD_TO_FW_CUSTCMD, (int)E_SSV_CTL_ATTR_CUSTCMD);
    return 0;
}

int ssv_netlink_send_cmd(char *buf, int buflen, int cmdid, int cmdattr)
{
    if (g_gnl_fd < 0)
    {
        return -1;
    }
    else
    {
        return _netlink_send_cmd(g_gnl_fd,buf,buflen,cmdid,cmdattr);
    }

}

int ssv_netlink_start(void)
{
    int ret=-1, val = 0;
    g_gnl_fd = _ssv_netlink_init();
    if (g_gnl_fd < 0)
    {
        ret = g_gnl_fd;
        goto out;
    }
    _netlink_send_cmd(g_gnl_fd, (char *)&val, (int)sizeof(val), (int)E_SSV_CTL_CMD_TO_DRV_OPERACMD, (int)E_SSV_CTL_ATTR_OPERACMD);

    ret = g_gnl_fd;
out:
    return ret;
}

int ssv_netlink_stop(void)
{
    int val = 1;
    _netlink_send_cmd(g_gnl_fd, (char *)&val, (int)sizeof(val), (int)E_SSV_CTL_CMD_TO_DRV_OPERACMD, (int)E_SSV_CTL_ATTR_OPERACMD);
    (void)_ssv_netlink_close(&g_gnl_fd);
    return 0;
}



