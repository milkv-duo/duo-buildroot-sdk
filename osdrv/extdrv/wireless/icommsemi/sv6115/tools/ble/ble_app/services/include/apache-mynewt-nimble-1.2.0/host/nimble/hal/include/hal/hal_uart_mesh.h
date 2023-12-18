/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_HAL_UART_MESH
#define H_HAL_UART_MESH

#include <stdio.h>
#include <stdint.h>

enum hal_uart_parity
{
    HAL_UART_PARITY_NONE=0,       
    HAL_UART_PARITY_EVEN=1,       
    HAL_UART_PARITY_ODD=2,        
    HAL_UART_PARITY_HIG=3,        
    HAL_UART_PARITY_LOW=4,        
};

enum hal_uart_flow_ctl
{
    HAL_UART_FLOW_CTL_NONE=0,       
    HAL_UART_FLOW_CTL_RTS_CTS =1,
};

typedef int(* hal_uart_tx_char)(void *arg);
typedef void(* hal_uart_tx_done)(void *arg);
typedef int(* hal_uart_rx_char)(void *arg, uint8_t byte);

struct hal_uart {
    //uint8_t u_open:1;
    //uint8_t u_rx_stall:1;
    //uint8_t u_tx_started:1;
    hal_uart_rx_char u_rx_func;
    hal_uart_tx_char u_tx_func;
    hal_uart_tx_done u_tx_done;
    void *u_func_arg;
    //uint32_t u_baudrate;
};

int hal_uart_close(int port);
int hal_uart_start_tx(int port);


int hal_uart_init_cbs(int uart, hal_uart_tx_char tx_func,
	hal_uart_tx_done tx_done, hal_uart_rx_char rx_func, void * arg);
int hal_uart_config(int uart, int32_t speed, uint8_t databits,
	uint8_t stopbits, enum hal_uart_parity parity, enum hal_uart_flow_ctl flow_ctl);

#endif