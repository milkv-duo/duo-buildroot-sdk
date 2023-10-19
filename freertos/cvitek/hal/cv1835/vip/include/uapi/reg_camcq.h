#ifndef _REG_CAMCQ_H_
#define _REG_CAMCQ_H_

/******************************************/
/*           Module Definition            */
/******************************************/
union REG_CMDQ_INT_EVENT {
	uint32_t raw;
	struct {
		uint32_t CMDQ_INT                        : 1;
		uint32_t CMDQ_END                        : 1;
		uint32_t CMDQ_WAIT                       : 1;
		uint32_t ISP_PSLVERR                     : 1;
	} bits;
};

union REG_CMDQ_INT_EN {
	uint32_t raw;
	struct {
		uint32_t CMDQ_INT_EN                     : 1;
		uint32_t CMDQ_END_EN                     : 1;
		uint32_t CMDQ_WAIT_EN                    : 1;
		uint32_t ISP_PSLVERR_EN                  : 1;
	} bits;
};

union REG_CMDQ_DMA_ADDR_L {
	uint32_t raw;
	struct {
		uint32_t DMA_ADDR_31_0                   : 32;
	} bits;
};

union REG_CMDQ_DMA_ADDR_H {
	uint32_t raw;
	struct {
		uint32_t DMA_ADDR_63_32                  : 32;
	} bits;
};

union REG_CMDQ_DMA_CNT {
	uint32_t raw;
	struct {
		uint32_t DMA_CNT                         : 32;
	} bits;
};

union REG_CMDQ_DMA_CONFIG {
	uint32_t raw;
	struct {
		uint32_t DMA_RSV                         : 1;
		uint32_t ADMA_EN                         : 1;
	} bits;
};

union REG_CMDQ_RSV_0 {
	uint32_t raw;
	struct {
		uint32_t RSV0                            : 10;
	} bits;
};

union REG_CMDQ_JOB_CTL {
	uint32_t raw;
	struct {
		uint32_t JOB_START                       : 1;
		uint32_t CMD_RESTART                     : 1;
		uint32_t RESTART_HW_MOD                  : 1;
	} bits;
};

union REG_CMDQ_STATUS {
	uint32_t raw;
	struct {
		uint32_t AXI_ERR                         : 1;
		uint32_t CMDQ_ERR                        : 1;
		uint32_t APB_ERR                         : 1;
	} bits;
};

union REG_CMDQ_APB_PARA {
	uint32_t raw;
	struct {
		uint32_t BASE_ADDR                       : 16;
		uint32_t APB_PPROT                       : 3;
	} bits;
};

union REG_CMDQ_DEBUG_BUS0 {
	uint32_t raw;
	struct {
		uint32_t DEBUS0                          : 32;
	} bits;
};

union REG_CMDQ_DEBUG_BUS1 {
	uint32_t raw;
	struct {
		uint32_t DEBUS1                          : 32;
	} bits;
};

union REG_CMDQ_DEBUG_BUS2 {
	uint32_t raw;
	struct {
		uint32_t DEBUS2                          : 32;
	} bits;
};

union REG_CMDQ_DEBUG_BUS3 {
	uint32_t raw;
	struct {
		uint32_t DEBUS3                          : 32;
	} bits;
};

union REG_CMDQ_DEBUG_BUS_SEL {
	uint32_t raw;
	struct {
		uint32_t DEBUS_SEL                       : 2;
	} bits;
};

union REG_CMDQ_DUMMY {
	uint32_t raw;
	struct {
		uint32_t DUMMY                           : 32;
	} bits;
};



/******************************************/
/*           Module Definition            */
/******************************************/
struct REG_CMDQ_T {
	union REG_CMDQ_INT_EVENT      CMDQ_INT_EVENT;           /* 0x0A07F000 */
	union REG_CMDQ_INT_EN         CMDQ_INT_EN;              /* 0x0A07F004 */
	union REG_CMDQ_DMA_ADDR_L     CMDQ_DMA_ADDR_L;          /* 0x0A07F008 */
	union REG_CMDQ_DMA_ADDR_H     CMDQ_DMA_ADDR_H;          /* 0x0A07F00C */
	union REG_CMDQ_DMA_CNT        CMDQ_DMA_CNT;             /* 0x0A07F010 */
	union REG_CMDQ_DMA_CONFIG     CMDQ_DMA_CONFIG;          /* 0x0A07F014 */
	union REG_CMDQ_RSV_0          CMDQ_RSV_0;               /* 0x0A07F018 */
	union REG_CMDQ_JOB_CTL        CMDQ_JOB_CTL;             /* 0x0A07F01C */
	union REG_CMDQ_STATUS         CMDQ_STATUS;              /* 0x0A07F020 */
	union REG_CMDQ_APB_PARA       CMDQ_APB_PARA;            /* 0x0A07F024 */
	union REG_CMDQ_DEBUG_BUS0     CMDQ_DEBUG_BUS0;          /* 0x0A07F028 */
	union REG_CMDQ_DEBUG_BUS1     CMDQ_DEBUG_BUS1;          /* 0x0A07F02C */
	union REG_CMDQ_DEBUG_BUS2     CMDQ_DEBUG_BUS2;          /* 0x0A07F030 */
	union REG_CMDQ_DEBUG_BUS3     CMDQ_DEBUG_BUS3;          /* 0x0A07F034 */
	union REG_CMDQ_DEBUG_BUS_SEL  CMDQ_DEBUG_BUS_SEL;       /* 0x0A07F038 */
	union REG_CMDQ_DUMMY          CMDQ_DUMMY;               /* 0x0A07F03C */
};


#endif // _REG_CAMCQ_H_
