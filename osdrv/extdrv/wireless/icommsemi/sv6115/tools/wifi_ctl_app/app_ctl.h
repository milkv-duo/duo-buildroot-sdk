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
 * @file app_ctl.h
 * @brief application control defines and declarations.
 */


#ifndef __APP_CTL_H__
#define __APP_CTL_H__
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 *         Include Files
 ******************************************************************************/


/*******************************************************************************
 *         Defines
 ******************************************************************************/
#ifndef _SSV_TYPES_H_
//SSV PACK Definition
#define SSV_PACKED_STRUCT_BEGIN
#define SSV_PACKED_STRUCT               //__attribute__ ((packed))
#define SSV_PACKED_STRUCT_END           //__attribute__((packed))
#define SSV_PACKED_STRUCT_STRUCT        __attribute__ ((packed))
#define SSV_PACKED_STRUCT_FIELD(x)      x
#endif


/*******************************************************************************
 *         Enumerations
 ******************************************************************************/


/*******************************************************************************
 *         Structures
 ******************************************************************************/


/*******************************************************************************
 *         Variables
 ******************************************************************************/


/*******************************************************************************
 *         Functions
 ******************************************************************************/
int app_ctl_start(void);
void app_ctl_stop(int signum);


#ifdef __cplusplus
}
#endif
#endif /* __APP_CTL_H__ */
