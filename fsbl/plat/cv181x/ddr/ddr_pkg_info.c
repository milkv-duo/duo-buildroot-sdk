#include <platform_def.h>
#include <ddr_pkg_info.h>
#include <mmio.h>
#include <bitwise_ops.h>
#include <debug.h>

uint8_t ddr_vendor;
uint8_t ddr_capacity;
uint8_t pkg;
uint8_t ddr_type;

void read_ddr_pkg_info(void)
{
	uint32_t conf_info = mmio_read_32(0x03000004);
	uint32_t efuse_leakage = mmio_read_32(0x03050108);
	uint32_t pkg_type;
	uint32_t chip_id;

#if defined(DBG_SHMOO) || defined(DBG_SHMOO_CA) || defined(DBG_SHMOO_CS)
	NOTICE("conf_info(0x03000004)=0x%08x\n", conf_info);
	NOTICE("efuse_leakage(0x03050108)=0x%08x\n", efuse_leakage);
	NOTICE("FTSN3=0x%08x\n", mmio_read_32(0x0305010C));
	NOTICE("FTSN4=0x%08x\n", mmio_read_32(0x03050110));
#endif // DBG_SHMOO || DBG_SHMOO_CA || DBG_SHMOO_CS

	/* External DDR detection (independent of pkg_type):
	 * capacity == 0 in efuse indicates no SIP DDR is present.
	 * DDR2/DDR3 cannot be distinguished at this point,
	 * default to DDR3 since there is no DDR2 requirement.
	 */
	if (FIELD_GET(efuse_leakage, 28, 26) == DDR_CAPACITY_UNKNOWN) {
		NOTICE("External DDR3 detected\n");
		pkg = FIELD_GET(efuse_leakage, 31, 29);
		ddr_vendor = DDR_EXTERN_DDR3;
		ddr_capacity = DDR_CAPACITY_UNKNOWN;
	} else {

		pkg_type = FIELD_GET(conf_info, 30, 28);
		NOTICE("pkg_type=%x\n", pkg_type);

		switch (pkg_type) {
		case 0x0: //BGA 10x10, SIP 2Gb DDR3
			ddr_vendor = DDR_VENDOR_NY_2G;
			ddr_capacity = DDR_CAPACITY_2G;
			pkg = PKG_BGA;
			break;
		case 0x1: //BGA 10x10, SIP 4Gb DDR3
			ddr_vendor = DDR_VENDOR_NY_4G;
			ddr_capacity = DDR_CAPACITY_4G;
			pkg = PKG_BGA;
			break;
		case 0x2: //BGA 10x10, SIP 1Gb DDR3
			ddr_vendor = DDR_VENDOR_ESMT_1G;
			ddr_capacity = DDR_CAPACITY_1G;
			pkg = PKG_BGA;
			break;
		case 0x4: //2nd src need to read from efuse
			NOTICE("2nd\n");
			ddr_vendor = FIELD_GET(efuse_leakage, 25, 21);
			ddr_capacity = FIELD_GET(efuse_leakage, 28, 26);
			pkg = FIELD_GET(efuse_leakage, 31, 29);
			break;
		case 0x5: //QFN9x9, SIP 2Gb DDR3
			ddr_vendor = DDR_VENDOR_NY_2G;
			ddr_capacity = DDR_CAPACITY_2G;
			pkg = PKG_QFN;
			break;
		case 0x6: //QFN9x9, SIP 1Gb DDR3
			ddr_vendor = DDR_VENDOR_ESMT_1G;
			ddr_capacity = DDR_CAPACITY_1G;
			pkg = PKG_QFN;
			break;
		case 0x7: //QFN9x9, SIP 512Mb DDR2
			ddr_vendor = DDR_VENDOR_ESMT_512M_DDR2;
			ddr_capacity = DDR_CAPACITY_512M;
			pkg = PKG_QFN;
			break;
		case 0x8: //QFN9x9, SIP 1Gb DDR3
			ddr_vendor = DDR_VENDOR_UNILC_N25_1G;
			ddr_capacity = DDR_CAPACITY_1G;
			pkg = PKG_QFN;
			break;
		case 0x9: //QFN9x9, SIP 2Gb DDR3
			ddr_vendor = DDR_VENDOR_UNILC_N21_2G;
			ddr_capacity = DDR_CAPACITY_2G;
			pkg = PKG_QFN;
			break;
		case 0xA: //QFN9x9, SIP 2Gb DDR3
			ddr_vendor = DDR_VENDOR_ESMT_N21_2G;
			ddr_capacity = DDR_CAPACITY_2G;
			pkg = PKG_QFN;
			break;
		case 0xB:
			ddr_vendor = DDR_VENDOR_ESMT_2G;
			ddr_capacity = DDR_CAPACITY_2G;
			pkg = PKG_BGA;
			break;
		default:
			NOTICE("unknown pkg_type=0x%x\n", pkg_type);
		}
	}

	NOTICE("D%x_%x_%x\n", pkg, ddr_capacity, ddr_vendor);

	// assign ddr_type
	switch (ddr_vendor) {
	case DDR_VENDOR_ESMT_512M_DDR2:
	case DDR_VENDOR_ETRON_512M_DDR2:
	case DDR_VENDOR_UNILC_N25_512M_DDR2:
	case DDR_EXTERN_DDR2:
		NOTICE("DDR2");
		ddr_type = DDR_TYPE_DDR2;
		ddr_data_rate = 1333;
		break;
	case DDR_VENDOR_NY_4G:
	case DDR_VENDOR_NY_2G:
	case DDR_VENDOR_ESMT_1G:
	case DDR_VENDOR_ETRON_1G:
	case DDR_VENDOR_ESMT_2G:
	case DDR_VENDOR_PM_2G:
	case DDR_VENDOR_PM_1G:
	case DDR_VENDOR_ESMT_N25_1G:
	case DDR_VENDOR_NY_N20_1G:
	case DDR_VENDOR_UNILC_N25_1G:
	case DDR_VENDOR_UNILC_N21_2G:
	case DDR_VENDOR_ESMT_N21_2G:
	case DDR_VENDOR_ESMT_N19_4G:
	case DDR_EXTERN_DDR3:
		NOTICE("DDR3");
		ddr_type = DDR_TYPE_DDR3;
		// ddr_data_rate = 800;
		// ddr_data_rate = 1333;
		// ddr_data_rate = 1600;
		break;
	default:
		NOTICE("unknown vendor=%d", ddr_vendor);
		ddr_type = DDR_TYPE_UNKNOWN;
	}

	chip_id = 0x0;
	switch (ddr_capacity) {
	case DDR_CAPACITY_512M:
		NOTICE("-512M");
		chip_id = (pkg == PKG_QFN ? 0x1810c : 0x1810f);
		break;
	case DDR_CAPACITY_1G:
		NOTICE("-1G");
		chip_id = (pkg == PKG_QFN ? 0x1811c : 0x1811f);
		break;
	case DDR_CAPACITY_2G:
		NOTICE("-2G");
		chip_id = (pkg == PKG_QFN ? 0x1812c : 0x1812f);
		break;
	case DDR_CAPACITY_4G:
		NOTICE("-4G");
		chip_id = 0x1813f;
		break;
	default:
		if (ddr_vendor != DDR_EXTERN_DDR2 && ddr_vendor != DDR_EXTERN_DDR3) {
			NOTICE("-unknown capacity=%d", ddr_capacity);
			chip_id = 0x0;
		}
	}
	mmio_write_32(REG_GP_REG3, chip_id);

	switch (pkg) {
	case PKG_QFN:
		NOTICE("-QFN\n");
		break;
	case PKG_BGA:
		NOTICE("-BGA\n");
		break;
	default:
		NOTICE("-unknown pkg=%d\n", pkg);
	}
}

uint8_t get_ddr_vendor(void)
{
	return ddr_vendor;
}

uint8_t get_ddr_capacity(void)
{
	return ddr_capacity;
}

uint8_t get_pkg(void)
{
	return pkg;
}

uint8_t get_ddr_type(void)
{
	return ddr_type;
}
