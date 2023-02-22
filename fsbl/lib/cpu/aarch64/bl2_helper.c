#include <debug.h>
#include <console.h>
#include <platform.h>
#include <string.h>
#include <aarch64/bl2_helper.h>
#include <bl2.h>
#include <delay_timer.h>

void switch_el3_to_el2_ns(uintptr_t bl2_entry)
{
	uint64_t spsr, mdcr_el2;

	NOTICE("Switch to EL2-NS at 0x%lx\n", bl2_entry);

	spsr = SPSR_64(MODE_EL2, 0, DISABLE_ALL_EXCEPTIONS);
	write_spsr_el3(spsr);

	/*
	 * Set the next EL to be AArch64.
	 * Switch to non-secure state.
	 */
	write_scr_el3(read_scr_el3() | SCR_RW_BIT | SCR_NS_BIT);

	/*
	 * EL2 present but unused, need to disable safely.
	 * SCTLR_EL2 can be ignored in this case.
	 *
	 * Initialise all fields in HCR_EL2, except HCR_EL2.RW,
	 * to zero so that Non-secure operations do not trap to
	 * EL2.
	 *
	 * HCR_EL2.RW: Set this field to match SCR_EL3.RW
	 */
	write_hcr_el2(read_hcr_el2() | HCR_RW_BIT);

	/*
	 * Initialise CPTR_EL2 setting all fields rather than
	 * relying on the hw. All fields have architecturally
	 * UNKNOWN reset values.
	 *
	 * CPTR_EL2.TCPAC: Set to zero so that Non-secure EL1
	 *  accesses to the CPACR_EL1 or CPACR from both
	 *  Execution states do not trap to EL2.
	 *
	 * CPTR_EL2.TTA: Set to zero so that Non-secure System
	 *  register accesses to the trace registers from both
	 *  Execution states do not trap to EL2.
	 *
	 * CPTR_EL2.TFP: Set to zero so that Non-secure accesses
	 *  to SIMD and floating-point functionality from both
	 *  Execution states do not trap to EL2.
	 */
	write_cptr_el2(CPTR_EL2_RESET_VAL & ~(CPTR_EL2_TCPAC_BIT | CPTR_EL2_TTA_BIT | CPTR_EL2_TFP_BIT));

	/*
	 * Initiliase CNTHCTL_EL2. All fields are
	 * architecturally UNKNOWN on reset and are set to zero
	 * except for field(s) listed below.
	 *
	 * CNTHCTL_EL2.EL1PCEN: Set to one to disable traps to
	 *  Hyp mode of Non-secure EL0 and EL1 accesses to the
	 *  physical timer registers.
	 *
	 * CNTHCTL_EL2.EL1PCTEN: Set to one to disable traps to
	 *  Hyp mode of  Non-secure EL0 and EL1 accesses to the
	 *  physical counter registers.
	 */
	write_cnthctl_el2(CNTHCTL_RESET_VAL | EL1PCEN_BIT | EL1PCTEN_BIT);

	/*
	 * Initialise CNTVOFF_EL2 to zero as it resets to an
	 * architecturally UNKNOWN value.
	 */
	write_cntvoff_el2(0);

	/*
	 * Set VPIDR_EL2 and VMPIDR_EL2 to match MIDR_EL1 and
	 * MPIDR_EL1 respectively.
	 */
	write_vpidr_el2(read_midr_el1());
	write_vmpidr_el2(read_mpidr_el1());

	/*
	 * Initialise VTTBR_EL2. All fields are architecturally
	 * UNKNOWN on reset.
	 *
	 * VTTBR_EL2.VMID: Set to zero. Even though EL1&0 stage
	 *  2 address translation is disabled, cache maintenance
	 *  operations depend on the VMID.
	 *
	 * VTTBR_EL2.BADDR: Set to zero as EL1&0 stage 2 address
	 *  translation is disabled.
	 */
	write_vttbr_el2(VTTBR_RESET_VAL &
			~((VTTBR_VMID_MASK << VTTBR_VMID_SHIFT) | (VTTBR_BADDR_MASK << VTTBR_BADDR_SHIFT)));

	/*
	 * Initialise MDCR_EL2, setting all fields rather than
	 * relying on hw. Some fields are architecturally
	 * UNKNOWN on reset.
	 *
	 * MDCR_EL2.TPMS (ARM v8.2): Do not trap statistical
	 * profiling controls to EL2.
	 *
	 * MDCR_EL2.E2PB (ARM v8.2): SPE enabled in non-secure
	 * state. Accesses to profiling buffer controls at
	 * non-secure EL1 are not trapped to EL2.
	 *
	 * MDCR_EL2.TDRA: Set to zero so that Non-secure EL0 and
	 *  EL1 System register accesses to the Debug ROM
	 *  registers are not trapped to EL2.
	 *
	 * MDCR_EL2.TDOSA: Set to zero so that Non-secure EL1
	 *  System register accesses to the powerdown debug
	 *  registers are not trapped to EL2.
	 *
	 * MDCR_EL2.TDA: Set to zero so that System register
	 *  accesses to the debug registers do not trap to EL2.
	 *
	 * MDCR_EL2.TDE: Set to zero so that debug exceptions
	 *  are not routed to EL2.
	 *
	 * MDCR_EL2.HPME: Set to zero to disable EL2 Performance
	 *  Monitors.
	 *
	 * MDCR_EL2.TPM: Set to zero so that Non-secure EL0 and
	 *  EL1 accesses to all Performance Monitors registers
	 *  are not trapped to EL2.
	 *
	 * MDCR_EL2.TPMCR: Set to zero so that Non-secure EL0
	 *  and EL1 accesses to the PMCR_EL0 or PMCR are not
	 *  trapped to EL2.
	 *
	 * MDCR_EL2.HPMN: Set to value of PMCR_EL0.N which is the
	 *  architecturally-defined reset value.
	 */
	mdcr_el2 = ((MDCR_EL2_RESET_VAL | ((read_pmcr_el0() & PMCR_EL0_N_BITS) >> PMCR_EL0_N_SHIFT)) &
		    ~(MDCR_EL2_TDRA_BIT | MDCR_EL2_TDOSA_BIT | MDCR_EL2_TDA_BIT | MDCR_EL2_TDE_BIT | MDCR_EL2_HPME_BIT |
		      MDCR_EL2_TPM_BIT | MDCR_EL2_TPMCR_BIT));

	write_mdcr_el2(mdcr_el2);

	/*
	 * Initialise HSTR_EL2. All fields are architecturally
	 * UNKNOWN on reset.
	 *
	 * HSTR_EL2.T<n>: Set all these fields to zero so that
	 *  Non-secure EL0 or EL1 accesses to System registers
	 *  do not trap to EL2.
	 */
	write_hstr_el2(HSTR_EL2_RESET_VAL & ~(HSTR_EL2_T_MASK));
	/*
	 * Initialise CNTHP_CTL_EL2. All fields are
	 * architecturally UNKNOWN on reset.
	 *
	 * CNTHP_CTL_EL2:ENABLE: Set to zero to disable the EL2
	 *  physical timer and prevent timer interrupts.
	 */
	write_cnthp_ctl_el2(CNTHP_CTL_RESET_VAL & ~(CNTHP_CTL_ENABLE_BIT));

	ATF_STATE = ATF_STATE_BL2_MAIN + 1;
	time_records->fsbl_exit = read_time_ms();

	call_with_eret(bl2_entry);

	ATF_STATE = ATF_STATE_BL2_MAIN + 2;
}

typedef struct param_header {
	uint8_t type; /* type of the structure */
	uint8_t version; /* version of this structure */
	uint16_t size; /* size of this structure in bytes */
	uint32_t attr; /* attributes: unused bits SBZ */
} param_header_t;

typedef struct image_info {
	param_header_t h;
	uintptr_t image_base; /* physical address of base of image */
	uint32_t image_size; /* bytes read from image file */
} image_info_t;

typedef struct aapcs64_params {
	u_register_t arg0;
	u_register_t arg1;
	u_register_t arg2;
	u_register_t arg3;
	u_register_t arg4;
	u_register_t arg5;
	u_register_t arg6;
	u_register_t arg7;
} aapcs64_params_t;

typedef struct entry_point_info {
	param_header_t h;
	uintptr_t pc;
	uint32_t spsr;
	aapcs64_params_t args;
} entry_point_info_t;

typedef struct bl31_params {
	param_header_t h;
	image_info_t *bl31_image_info;
	entry_point_info_t *bl32_ep_info;
	image_info_t *bl32_image_info;
	entry_point_info_t *bl33_ep_info;
	image_info_t *bl33_image_info;
} bl31_params_t;

#define PARAM_BL31 0x03
#define VERSION_1 0x01

#define MODE_EL2 U(0x2)
#define MODE_EL1 U(0x1)
#define NON_SECURE U(0x1)

#define PARAM_EP_SECURITY_MASK U(0x1)

#define SET_SECURITY_STATE(x, security) ((x) = ((x) & ~PARAM_EP_SECURITY_MASK) | (security))

struct {
	bl31_params_t bl31_params;
	entry_point_info_t bl33_ep_info;
} static next_info;

void jump_to_monitor(uintptr_t monitor_entry, uintptr_t next_addr)
{
	const char skip[] = "SKIP_LICENSE_CHECK\0";
	bl31_params_t *from_bl2 = &next_info.bl31_params;

	memcpy((void *)LICENSE_FILE_ADDR, skip, sizeof(skip));

	from_bl2->h.type = PARAM_BL31;
	from_bl2->h.version = VERSION_1;
	from_bl2->bl33_ep_info = &next_info.bl33_ep_info;

	SET_SECURITY_STATE(from_bl2->bl33_ep_info->h.attr, NON_SECURE);
	from_bl2->bl33_ep_info->pc = next_addr;
	from_bl2->bl33_ep_info->args.arg0 = 0;
	from_bl2->bl33_ep_info->spsr = SPSR_64(MODE_EL2, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);

	flush_dcache_range(LICENSE_FILE_ADDR, 64);
	flush_dcache_range((uintptr_t)&next_info, sizeof(next_info));

	time_records->fsbl_exit = read_time_ms();
	jump_bl31(monitor_entry, from_bl2);
}

void jump_to_loader_2nd(uintptr_t loader_2nd_entry)
{
	switch_el3_to_el2_ns(loader_2nd_entry);
}

void bl2_smc_handler(void)
{
	NOTICE("%s:%d\n", __func__, __LINE__);

	while (1)
		;
	__builtin_unreachable();
}
