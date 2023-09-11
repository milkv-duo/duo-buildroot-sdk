#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "sample_vio.h"
#include "cvi_sys.h"
#include <linux/cvi_type.h>

void SAMPLE_VIO_HandleSig(CVI_S32 signo)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (SIGINT == signo || SIGTERM == signo) {
		//todo for release
		SAMPLE_PRT("Program termination abnormally\n");
	}
	exit(-1);
}

void SAMPLE_VIO_Usage(char *sPrgNm)
{
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");
	printf("\t 0)VI (Offline) - VPSS(Offline) - VO(Rotation).\n");
	printf("\t 1)VI (Offline) - VPSS(Offline,Keep Aspect Ratio) - VO.\n");
	printf("\t 2)VI (Offline, Rotation) - VPSS(Offline,Keep Aspect Ratio) - VO.\n");
	printf("\t 3)VI (Offline) - VPSS(Offline, Rotation) - VO.\n");
	printf("\t 4)VPSS(Offline, file read/write).\n");
	printf("\t 5)VI - VPSS SLT TEST. frm number only can be 1.\n");
	printf("\t 6)VI (Two devs) - VPSS - VO.\n");
	printf("\t 7)VI (3S Thermal sensor) - VI dump.\n");
	printf("\t 8)VI (MM308M2 YUV sensor) - VI dump.\n");
	printf("\t 9)VI (MCS369Q YUV sensor) - VI dump.\n");
	printf("\t10)VPSS(Offline, file read/write, combine 2 frame into 1).\n");
#ifdef ARCH_CV183X
	printf("\t11)VI (N5 YUV sensor) - VI dump.\n");
	printf("\t12)VI (MCS369 YUV sensor) - VI dump.\n");
	printf("\t13)VI (IMX327 + N5) offline VPSS - VO.\n");
	printf("\t14)VI (PR2100 2ch/4ch FHD) - VPSS - VO.\n");
	printf("\t15)VI (IMX327 + N5) online VPSS - VO.\n");
#endif
#ifdef ARCH_CV182X
	printf("\t16)VI (IMX335 + PR2020) - online/offline VPSS - VO.\n");
	printf("\t17)VI (IMX307 + PR2020) - online/offline VPSS - VO.\n");
	printf("\t18)VI (IMX307) - onthefly online to VPSS - VO.\n");
#endif
	printf("\t50)VO (OT07007) - lvds colorbar test.\n");
}

int main(int argc, char *argv[])
{
	CVI_S32 s32Ret = CVI_FAILURE;
	CVI_S32 s32Index;

	if (argc < 2) {
		SAMPLE_VIO_Usage(argv[0]);
		return CVI_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_VIO_Usage(argv[0]);
		return CVI_SUCCESS;
	}

	signal(SIGINT, SAMPLE_VIO_HandleSig);
	signal(SIGTERM, SAMPLE_VIO_HandleSig);

	s32Index = atoi(argv[1]);
	switch (s32Index) {
	case 0:
		s32Ret = SAMPLE_VIO_VoRotation();
		break;

	case 1:
		s32Ret = SAMPLE_VIO_ViVpssAspectRatio();
		break;

	case 2:
		s32Ret = SAMPLE_VIO_ViRotation();
		break;

	case 3:
		s32Ret = SAMPLE_VIO_VpssRotation();
		break;

	case 4: {
		SIZE_S stSize;

		printf("Please make sure test input file, input, is ready.\n");
		printf("Input file width: ");
		scanf("%u", &stSize.u32Width);
		printf("Input file height: ");
		scanf("%u", &stSize.u32Height);

		s32Ret = SAMPLE_VIO_VpssFileIO(stSize);
		break;
	}

	case 5:
		s32Ret = SAMPLE_VI_VPSS_SLT(atoi(argv[2]), argv[3]);
		break;
	case 6:
		s32Ret = SAMPLE_VIO_TWO_DEV_VO();
		break;
	case 7:
		s32Ret = SAMPLE_PICO640_TEST();
		break;
	case 8:
		s32Ret = SAMPLE_MM308M2_TEST();
		break;
	case 9:
		s32Ret = SAMPLE_MCS369Q_TEST();
		break;

	case 10: {
		SIZE_S stSize;

		printf("Please make sure test input file, input, is ready.\n");
		printf("Input file width: ");
		scanf("%u", &stSize.u32Width);
		printf("Input file height: ");
		scanf("%u", &stSize.u32Height);

		s32Ret = SAMPLE_VIO_VpssCombine2File(stSize);
		break;
	}
#ifdef ARCH_CV183X
	case 11:
		s32Ret = SAMPLE_N5_TEST();
		break;
	case 12:
		s32Ret = SAMPLE_MCS369_TEST();
		break;
	case 13:
		s32Ret = SAMPLE_IMX327_N5_TEST();
		break;
	case 14:
		s32Ret = SAMPLE_PR2100_TEST();
		break;
	case 15:
		s32Ret = SAMPLE_IMX327_N5_VI_VPSS_ONLINE_TEST();
		break;
#endif
#ifdef ARCH_CV182X
	case 16:
		s32Ret = SAMPLE_IMX335_PR2020_OFFLINE_ONLINE_TEST();
		break;
	case 17:
		s32Ret = SAMPLE_IMX307_PR2020_OFFLINE_ONLINE_TEST();
		break;
	case 18:
		s32Ret = SAMPLE_IMX307_ONTHEFLY_ONLINE_SC_TEST();
		break;
#endif
	case 50:
		s32Ret = SAMPLE_VO_LVDS_TEST();
		break;
	default:
		SAMPLE_PRT("the index %d is invaild!\n", s32Index);
		SAMPLE_VIO_Usage(argv[0]);
		return CVI_FAILURE;
	}

	if (s32Ret == CVI_SUCCESS)
		SAMPLE_PRT("sample_vio exit success!\n");
	else
		SAMPLE_PRT("sample_vio exit abnormally!\n");

	return s32Ret;
}

