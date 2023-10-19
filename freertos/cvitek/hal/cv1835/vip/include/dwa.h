#ifndef _CVI_DWA_H_
#define _CVI_DWA_H_

#include "dwa_cfg.h"

void dwa_set_base_addr(void *base);

/**
 * dwa_disable - disable dwa engine.
 *
 */
void dwa_disable(void);

/**
 * dwa_init - setup dwa, mainly interpolation settings.
 *
 */
void dwa_init(void);

/**
 * dwa_reset - do reset. This can be activated only if dma stop to avoid hang
 *	       fabric.
 *
 */
void dwa_reset(void);

/**
 * dwa_engine - start a dwa operation, wait frame_done intr after this.
 *              If output target is scaler, scaler's config should be done
 *              before this.
 *
 * @param cfg: settings for this dwa's operation
 */
void dwa_engine(struct dwa_cfg *cfg);

/**
 * dwa_engine_cmdq - start multiple dwa operation based on given cfgs, wait
 *		     cmdq-end intr after this. If output target is scaler,
 *		     scaler's config should be done before this.
 *
 * @param cfgs: settings for these dwa's operations
 * @param cnt: number of dwa operations
 */
void dwa_engine_cmdq(struct dwa_cfg *cfgs, u8 cnt);

/**
 * dwa_intr_ctrl - dwa's interrupt on(1)/off(0)
 *                 bit0: frame_done, bit1: mesh_id axi read err,
 *                 bit2: mesh_table axi read err
 *
 * @param intr_mask: On/Off ctrl of the interrupt.
 */
void dwa_intr_ctrl(u8 intr_mask);

/**
 * dwa_intr_ctrl - clear dwa's interrupt
 *                 bit0: frame_done, bit1: mesh_id axi read err,
 *                 bit2: mesh_table axi read err
 *
 * @param intr_mask: On/Off ctrl of the interrupt.
 */
void dwa_intr_clr(u8 intr_mask);

/**
 * dwa_intr_status - dwa's interrupt status
 *                 bit0: frame_done, bit1: mesh_id axi read err,
 *                 bit2: mesh_table axi read err
 *
 * @return: The interrupt's status. 1 if active.
 */
u8 dwa_intr_status(void);

/**
 * dwa_is_busy - check if dwa's operation is finished.
 *              dwa_start can only be toggled if only dma done(frame_done intr),
 *              ow dma won't finished.
 */
bool dwa_is_busy(void);

#endif // _CVI_DWA_H_
