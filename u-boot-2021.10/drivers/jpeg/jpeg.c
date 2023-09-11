#include "jpuconfig.h"
#include "regdefine.h"
#include "jpulog.h"
#include "jpurun.h"
#include "jpuhelper.h"
#include "jdi_osal.h"
#include <asm/io.h>

typedef struct v {
	int mode; // 1 = dec, 2 = enc
	int comparatorFlag;
	int packedFormat;
	int chroma_interleave;
	int usePartialMode;
	int partialBufNum;
	int rot_angle;
	int mirDir;
	int outNum;
	int roiEnable;
	int roiWidth;
	int roiHeight;
	int mjpgChromaFormat;
	int roiOffsetX;
	int roiOffsetY;
	int iHorScaleMode;
	int iVerScaleMode;
	int fileSize;
	void *bs_addr;
	void *yuv_addr;
} dec_cfg_t;

#define mmio_write_32(a, v) writel(v, a)
#define mmio_read_32(a) readl(a)

enum _mode_ {
	MODE_DEC = 1,
	MODE_ENC,
};

enum _packFormat_ {
	PACK_PLANAR = 0,
	PACK_YUYV,
	PACK_UYVY,
	PACK_YVYU,
	PACK_VYUY,
	PACK_YUV_444_PACKED,
} packFormat;

enum _partialMode_ {
	PARTIAL_MODE_DISABLE = 0,
	PARTIAL_MODE_ENABLE,
};

enum _rorateAngle_ {
	ROTATE_0 = 0,
	ROTATE_90 = 90,
	ROTATE_180 = 180,
	ROTATE_270 = 270,
};

enum _mirror_dir_ {
	MIRROR_NO = 0,
	MIRROR_VERTICAL,
	MIRROR_HORIZONTAL,
	MIRROR_BOTH,
};

int jpeg_dec(dec_cfg_t *cfg, void *bs_addr, void *yuv_addr, int size)
{
	int ret = 0;
	DecConfigParam  decConfig;

	memset(&decConfig, 0x00, sizeof(DecConfigParam));

	decConfig.bitstreamFileName = NULL;
	decConfig.StreamEndian = JPU_STREAM_ENDIAN;
	decConfig.FrameEndian = JPU_FRAME_ENDIAN;
	decConfig.yuvFileName = NULL;
	decConfig.comparatorFlag   = cfg->comparatorFlag;
	decConfig.packedFormat     = cfg->packedFormat;
	decConfig.chroma_interleave = cfg->chroma_interleave;
	decConfig.usePartialMode   = cfg->usePartialMode;
	decConfig.partialBufNum    = cfg->partialBufNum;
	decConfig.rot_angle         = cfg->rot_angle;
	decConfig.mirDir           = cfg->mirDir;
	decConfig.outNum           = cfg->outNum;
	decConfig.roiEnable        = cfg->roiEnable;
	decConfig.roiWidth         = cfg->roiWidth;
	decConfig.roiHeight        = cfg->roiHeight;
	decConfig.roiOffsetX       = cfg->roiOffsetX;
	decConfig.roiOffsetY       = cfg->roiOffsetY;
	decConfig.iHorScaleMode    = cfg->iHorScaleMode;
	decConfig.iVerScaleMode    = cfg->iVerScaleMode;
	decConfig.bs_addr          = bs_addr;
	decConfig.yuv_addr         = yuv_addr;
	decConfig.size             = size;

	if (!decConfig.usePartialMode) {
		if (decConfig.rot_angle != 0 && decConfig.rot_angle != 90 &&
		    decConfig.rot_angle != 180 && decConfig.rot_angle != 270) {
			JLOG(ERR, "Invalid rotation angle.\n");
			return 1;
		}

		if (decConfig.mirDir != 0 && decConfig.mirDir != 1 &&
		    decConfig.mirDir != 2 && decConfig.mirDir != 3) {
			JLOG(ERR, "Invalid mirror direction.\n");
			return 1;
		}

		if (decConfig.rot_angle != 0 || decConfig.mirDir != 0)
			decConfig.useRot = 1;
	}

	ret = jpeg_decode_helper(&decConfig);

	return 1 - ret;
}

int jpeg_decoder(void *bs_addr, void *yuv_addr, int size)
{
	dec_cfg_t allCfgs[] = {
	//        comp
	//        {MODE_ENC, 1, PACK_PLANAR, 0, PARTIAL_MODE_DISABLE, 2, ROTATE_0, MIRROR_NO, 1,
	//	 0, 3840, 2160, PACK_PLANAR, 0, 0, 0, 0, 40681 },
		{MODE_DEC, 1, PACK_PLANAR, 0, PARTIAL_MODE_DISABLE, 4, ROTATE_0, MIRROR_NO, 1,
		 0,   300,   300,    0,           50, 50, 0, 0, 0x23431},
	};

	int idx, ret = 0, all = 0;

	mmio_write_32((void *)TOP_DDR_ADDR_MODE_REG, (1 << DAMR_REG_VD_REMAP_ADDR_39_32_OFFSET));
	mmio_write_32((void *)VC_REG_BASE, (mmio_read_32((void *)VC_REG_BASE) | (0x1f)));

#ifdef SUPPORT_INTERRUPT
	request_irq(JPEG_CODEC_INTR_NUM, irq_handler_jpeg_codec, 0, "jpeg int", NULL);
	BM_DBG_TRACE("irq num = %d\n", JPEG_INTRPT_REQ);
#endif

	for (idx = 0; idx < sizeof(allCfgs) / sizeof(dec_cfg_t); idx++) {
		if (allCfgs[idx].mode == MODE_DEC)
			ret = jpeg_dec(&allCfgs[idx], bs_addr, yuv_addr, size);
//		else
//			ret = jpeg_enc_slt_test(&allCfgs[idx]);

		if (ret) {
			JLOG(NONE, "case %d, error\n", idx);
			all = 1;
		} else
			JLOG(NONE, "case %d, success\n", idx);
	}

	JLOG(NONE, "jpeg decode %s\n", all ? "failed" : "passed");
	return all;
}
