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
 * @file ssv_ctl_common.h
 * @brief Common defines and declarations for host driver for all platforms.
 */


#ifndef __SSV_CTL_COMMON_H__
#define __SSV_CTL_COMMON_H__
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 *         Include Files
 ******************************************************************************/
#include <linux/genetlink.h>


/*******************************************************************************
 *         Defines
 ******************************************************************************/
/// SSV control netlink ID.
#define SSV_CTL_NL_ID                       (998)


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

enum {
    E_SSV_CTL_ATTR_UNSPEC,                  ///< Unspecified.
    E_SSV_CTL_ATTR_OPERACMD,                ///< Operation command.
    E_SSV_CTL_ATTR_OPERACMD_RSP,            ///< Operation response.
    E_SSV_CTL_ATTR_SSV_NIMBLE_ENABLE,         ///< NIMBLE
    E_SSV_CTL_ATTR_SSV_NIMBLE_DISABLE,        ///< NIMBLE
    E_SSV_CTL_ATTR_TO_SSV_NIMBLE,             ///< NIMBLE
    E_SSV_CTL_ATTR_FROM_SSV_NIMBLE,           ///< NIMBLE
    E_SSV_CTL_ATTR_MAX                      ///< Maximum.
};
#define SSV_CTL_ATTR_MAX (E_SSV_CTL_ATTR_MAX - 1)
enum {
    E_SSV_CTL_CMD_UNSPEC,                   ///< Unspecified.
    /** Linux APP process to Linux kernel driver */
    E_SSV_CTL_CMD_TO_DRV_OPERACMD,          ///< Operation command to drvier.
    E_SSV_CTL_CMD_SSV_NIMBLE_ENABLE,
    E_SSV_CTL_CMD_SSV_NIMBLE_DISABLE,
    E_SSV_CTL_CMD_TO_SSV_NIMBLE,
    E_SSV_CTL_CMD_FROM_SSV_NIMBLE,
    E_SSV_CTL_CMD_MAX                       ///< Maximum.
};
#define SSV_CTL_CMD_MAX (E_SSV_CTL_CMD_MAX - 1)



#ifdef __cplusplus
}
#endif
#endif /* __SSV_CTL_COMMON_H__ */
