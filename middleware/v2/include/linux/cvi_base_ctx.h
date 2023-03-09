#ifndef __CVI_BASE_CTX_H__
#define __CVI_BASE_CTX_H__

#include <linux/types.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/version.h>

#include <linux/cvi_common.h>
#include <linux/cvi_comm_sys.h>

#define BASE_LOG_LEVEL_OFFSET       (0x10)
#define BASE_BIND_INFO_OFFSET       (BASE_LOG_LEVEL_OFFSET + LOG_LEVEL_RSV_SIZE)
#define BASE_VERSION_INFO_OFFSET    (BASE_BIND_INFO_OFFSET + BIND_INFO_RSV_SIZE)

#define LOG_LEVEL_RSV_SIZE          (sizeof(CVI_S32) * CVI_ID_BUTT)
#define BIND_INFO_RSV_SIZE          (sizeof(BIND_NODE_S) * BIND_NODE_MAXNUM)
#define VERSION_INFO_RSV_SIZE       (sizeof(MMF_VERSION_S))

#define BASE_SHARE_MEM_SIZE         ALIGN(BASE_VERSION_INFO_OFFSET + VERSION_INFO_RSV_SIZE, 0x1000)
#define GDC_SHARE_MEM_SIZE          (0x8000)

#define NUM_OF_PLANES               3

#define CHN_MATCH(x, y) (((x)->enModId == (y)->enModId) && ((x)->s32DevId == (y)->s32DevId)             \
	&& ((x)->s32ChnId == (y)->s32ChnId))

enum CHN_TYPE_E {
	CHN_TYPE_IN = 0,
	CHN_TYPE_OUT,
	CHN_TYPE_MAX
};

// start point is included.
// end point is excluded.
struct crop_size {
	uint16_t  start_x;
	uint16_t  start_y;
	uint16_t  end_x;
	uint16_t  end_y;
};

struct cvi_gdc_mesh {
	CVI_U64 paddr;
	CVI_VOID *vaddr;
	CVI_U32 meshSize;
	struct mutex lock;
};

enum gdc_job_state {
	GDC_JOB_SUCCESS = 0,
	GDC_JOB_FAIL,
	GDC_JOB_WORKING,
};

struct gdc_job_info {
	int64_t hHandle;
	MOD_ID_E enModId; // the module submitted gdc job
	uint32_t u32TaskNum; // number of tasks
	enum gdc_job_state eState; // job state
	uint32_t u32InSize;
	uint32_t u32OutSize;
	uint32_t u32CostTime; // From job submitted to job done
	uint32_t u32HwTime; // HW cost time
	uint32_t u32BusyTime; // From job submitted to job commit to driver
	uint64_t u64SubmitTime; // us
};

struct gdc_job_status {
	uint32_t u32Success;
	uint32_t u32Fail;
	uint32_t u32Cancel;
	uint32_t u32BeginNum;
	uint32_t u32BusyNum;
	uint32_t u32ProcingNum;
};

struct gdc_task_status {
	uint32_t u32Success;
	uint32_t u32Fail;
	uint32_t u32Cancel;
	uint32_t u32BusyNum;
};

struct gdc_operation_status {
	uint32_t u32AddTaskSuc;
	uint32_t u32AddTaskFail;
	uint32_t u32EndSuc;
	uint32_t u32EndFail;
	uint32_t u32CbCnt;
};

struct cvi_gdc_proc_ctx {
	struct gdc_job_info stJobInfo[GDC_PROC_JOB_INFO_NUM];
	uint16_t u16JobInfoIdx; // latest job submitted
	struct gdc_job_status stJobStatus;
	struct gdc_task_status stTaskStatus;
	struct gdc_operation_status stFishEyeStatus;
};

#endif  /* __CVI_BASE_CTX_H__ */
