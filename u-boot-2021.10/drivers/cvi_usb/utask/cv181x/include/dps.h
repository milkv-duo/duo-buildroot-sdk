#ifndef DPS_H
#define DPS_H

#include "cvi_stdtypes.h"

/****************************************************************************
 * Prototypes
 ***************************************************************************/

/**
 * Read a (32-bit) word
 * @param[in] address the address
 * @return the word at the given address
 */
extern uint32_t cvi_read32(volatile uint32_t *address);

/**
 * Write a (32-bit) word to memory
 * @param[in] address the address
 * @param[in] value the word to write
 */
extern void cvi_write32(uint32_t value, volatile uint32_t *address);
/**
 * Read a byte, bypassing the cache
 * @param[in] address the address
 * @return the byte at the given address
 */
extern uint8_t cvi_uncached_read8(volatile uint8_t *address);

/**
 * Read a short, bypassing the cache
 * @param[in] address the address
 * @return the short at the given address
 */
extern uint16_t cvi_uncached_read16(volatile uint16_t *address);

/**
 * Read a (32-bit) word, bypassing the cache
 * @param[in] address the address
 * @return the word at the given address
 */
extern uint32_t cvi_uncached_read32(volatile uint32_t *address);

/**
 * Write a byte to memory, bypassing the cache
 * @param[in] address the address
 * @param[in] value the byte to write
 */
extern void cvi_uncached_write8(uint8_t value, volatile uint8_t *address);

/**
 * Write a short to memory, bypassing the cache
 * @param[in] address the address
 * @param[in] value the short to write
 */
extern void cvi_uncached_write16(uint16_t value, volatile uint16_t *address);
/**
 * Write a (32-bit) word to memory, bypassing the cache
 * @param[in] address the address
 * @param[in] value the word to write
 */
extern void cvi_uncached_write32(uint32_t value, volatile uint32_t *address);
/**
 * Write a (32-bit) address value to memory, bypassing the cache.
 * This function is for writing an address value, i.e. something that
 * will be treated as an address by hardware, and therefore might need
 * to be translated to a physical bus address.
 * @param[in] location the (CPU) location where to write the address value
 * @param[in] value the address value to write
 */
extern void cvi_write_phys32(uint32_t value, volatile uint32_t *location);

/**
 * Hardware specific memcpy.
 * @param[in] src  src address
 * @param[in] dst  destination address
 * @param[in] size size of the copy
 */
extern void cvi_buffer_copy(volatile uint8_t *dst, volatile uint8_t *src, uint32_t size);

/**
 * Invalidate the cache for the specified memory region.
 * This function may be stubbed out if caching is disabled for memory regions
 * as described in the driver documentation, or if the driver configuration does
 * not require this function.
 * @param[in] address Virtual address of memory region. (If an MMU is not in use,
 * this will be equivalent to the physical address.) This address should be
 * rounded down to the nearest cache line boundary.
 * @param[in] size  size of memory in bytes.  This size should be rounded up to
 * the nearest cache line boundary.  Use size UINTPTR_MAX to invalidate all
 * memory cache.  A size of 0 should be ignored and the function should return
 * immediately with no effect.
 * @param[in] devInfo   This parameter can be used to pass implementation specific
 * data to this function.  The content and use of this parameter is up to the
 * implementor of this function to determine, and if not required it may be ignored.
 *  For example, under Linux it can be used to pass a pointer to
 * the device struct to be used in a call to dma_sync_single_for_device().  If
 * used, the parameter should be passed to the core driver at initialisation as
 * part of the configurationInfo struct.  Please
 * see the core driver documentation for details of how to do this.
 */
extern void cvi_cache_invalidate(uintptr_t address, size_t size);

/**
 * Flush the cache for the specified memory region
 * This function may be stubbed out if caching is disabled for memory regions
 * as described in the driver documentation, or if the driver configuration does
 * not require this function.
 * @param[in] address Virtual address of memory region. (If an MMU is not in use,
 * this will be equivalent to the physical address.) This address should be
 * rounded down to the nearest cache line boundary.
 * @param[in] size  size of memory in bytes.  This size should be rounded up to
 * the nearest cache line boundary.  Use size UINTPTR_MAX to flush all
 * memory cache.  A size of 0 should be ignored and the function should return
 * immediately with no effect.
 * @param[in] devInfo   This parameter can be used to pass implementation specific
 * data to this function.  The content and use of this parameter is up to the
 * implementor of this function to determine, and if not required it may be ignored.
 *  For example, under Linux it can be used to pass a pointer to
 * the device struct to be used in a call to dma_sync_single_for_device().  If
 * used, the parameter should be passed to the core driver at initialisation as
 * part of the configurationInfo struct.  Please
 * see the core driver documentation for details of how to do this.
 */
extern void cvi_cache_flush(uintptr_t address, size_t size);

/**
 * Delay software execution by a number of nanoseconds
 * @param[in] ns number of nanoseconds to delay software execution
 */
extern void cvi_delay_ns(uint32_t ns);

#endif /* DPS_H */
