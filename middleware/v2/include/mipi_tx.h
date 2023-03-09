/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: mipi_tx.h
 * Description:
 */

#ifndef __MIPI_TX_H__
#define __MIPI_TX_H__

#include <linux/vo_mipi_tx.h>

int mipi_tx_cfg(int fd, struct combo_dev_cfg_s *dev_cfg);
int mipi_tx_send_cmd(int fd, struct cmd_info_s *cmd_info);
int mipi_tx_recv_cmd(int fd, struct get_cmd_info_s *cmd_info);
int mipi_tx_enable(int fd);
int mipi_tx_disable(int fd);
int mipi_tx_set_hs_settle(int fd, const struct hs_settle_s *hs_cfg);
int mipi_tx_get_hs_settle(int fd, struct hs_settle_s *hs_cfg);

#endif // __MIPI_TX_H__

