/*
 * Copyright (c) 2019, Bitmain. All rights reserved.
 *
 */
#ifndef __CRYPTODMA_H
#define __CRYPTODMA_H

#include "platform.h"

// Register offset
#define CRYPTODMA_DMA_CTRL	0x0
#define CRYPTODMA_INT_MASK	0x4
#define CRYPTODMA_DES_BASE_L	0x8
#define CRYPTODMA_DES_BASE_H	0xC
#define CRYPTODMA_WR_INT	0x10
#define CRYPTODMA_DES_KEY	0x100
#define CRYPTODMA_DES_IV	0x180
#define CRYPTODMA_SHA_PAR	0x1C0

// Descriptor
#define CRYPTODMA_CTRL			0x00
#define CRYPTODMA_CIPHER		0x01
#define CRYPTODMA_SRC_ADDR_L		0x04
#define CRYPTODMA_SRC_ADDR_H		0x05
#define CRYPTODMA_DST_ADDR_L		0x06
#define CRYPTODMA_DST_ADDR_H		0x07
#define CRYPTODMA_DATA_AMOUNT_L		0x08
#define CRYPTODMA_DATA_AMOUNT_H		0x09
#define CRYPTODMA_KEY			0x0A
#define CRYPTODMA_IV			0x12

#define DES_USE_BYPASS		BIT(8)
#define DES_USE_AES		BIT(9)
#define DES_USE_DES		BIT(10)
#define DES_USE_SHA		BIT(12)
#define DES_USE_DESCRIPTOR_KEY	BIT(19)
#define DES_USE_DESCRIPTOR_IV	BIT(23)

// Cipher control for AES
#define DECRYPT_ENABLE	0x0
#define CBC_ENABLE	0x1
#define AES_KEY_MODE	0x4

// Cipher control for SHA
#define SHA_MODE_SHA256	(0x1 << 1)
#define SHA_LOAD_PARAM	(0x1)

// DMA control
#define DMA_ENABLE		1
#define DMA_DESCRIPTOR_MODE	1
#define DMA_READ_MAX_BURST	16
#define DMA_WRITE_MAX_BURST	16

#endif /*__CRYPTODMA_H*/
