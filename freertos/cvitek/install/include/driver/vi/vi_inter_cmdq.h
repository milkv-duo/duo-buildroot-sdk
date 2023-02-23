#ifndef __VI_INTER_CMDQ_H__
#define __VI_INTER_CMDQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

enum VI_EVENT_E {
	VI_EV_SOF = 1,
	VI_EV_FE_DONE,
	VI_EV_BE_DONE,
	VI_EV_POST_SHAW_DONE,
	VI_EV_POST_DONE,
	VI_EV_MAX,
};

enum VI_INTER_CMDQ_E {
	VI_CMDQ_EV_TYPE,
	VI_CMDQ_MAX,
};

#ifdef __cplusplus
}
#endif

#endif /* __VI_INTER_CMDQ_H__ */
