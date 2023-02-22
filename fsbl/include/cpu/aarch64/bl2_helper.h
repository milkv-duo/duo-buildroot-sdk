#ifndef __BL2_HELPER_H__
#define __BL2_HELPER_H__

void call_with_eret_to_next(void);
void call_with_eret(uintptr_t bl2_entry);
void jump_bl31(uintptr_t monitor_entry, void *bl31_params);

#endif /* __BL2_HELPER_H__ */
