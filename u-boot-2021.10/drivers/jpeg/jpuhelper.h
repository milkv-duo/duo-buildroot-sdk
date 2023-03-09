#ifndef _JPU_HELPER_H_
#define _JPU_HELPER_H_

#include "jpuapi.h"
#include "jpurun.h"

typedef struct {
	char SrcFileName[256];
	int NumFrame;
	int PicX;
	int PicY;
	int FrameRate;

	// MPEG4 ONLY
	int VerId;
	int DataPartEn;
	int RevVlcEn;
	int ShortVideoHeader;
	int AnnexI;
	int AnnexJ;
	int AnnexK;
	int AnnexT;
	int IntraDcVlcThr;
	int VopQuant;

	// H.264 ONLY
	int ConstIntraPredFlag;
	int DisableDeblk;
	int DeblkOffsetA;
	int DeblkOffsetB;
	int ChromaQpOffset;
	int PicQpY;

	// MJPEG ONLY
	char HuffTabName[256];
	char QMatTabName[256];
	int VersionID;
	int FrmFormat;
	int SrcFormat;
	int RstIntval;
	int ThumbEnable;
	int ThumbSizeX;
	int ThumbSizeY;

	// COMMON
	int GopPicNum;
	int SliceMode;
	int SliceSizeMode;
	int SliceSizeNum;

	int IntraRefreshNum;

	int ConstantIntraQPEnable;
	int RCIntraQP;
	int MaxQpSetEnable;
	int MaxQp;
	int GammaSetEnable;
	int Gamma;
	int HecEnable;

	// RC
	int RcEnable;
	int RcBitRate;
	int RcInitDelay;
	int RcBufSize;

	// NEW RC Scheme
	int RcIntervalMode;
	int RcMBInterval;
	int IntraCostWeight;
	int SearchRange;
	int MeUseZeroPmv;
	int MeBlkModeEnable;

} ENC_CFG;

typedef struct {
	int sourceFormat;
	int restartInterval;
	BYTE huffVal[4][162];
	BYTE huffBits[4][256];
	BYTE qMatTab[4][64];
} EncMjpgParam;

#if defined(__cplusplus)
extern "C" {
#endif

int jpgGetHuffTable(char *huffFileName, EncMjpgParam *param);
int jpgGetQMatrix(char *qMatFileName, EncMjpgParam *param);

int getJpgEncOpenParamDefault(JpgEncOpenParam *pEncOP, EncConfigParam *pEncConfig);
int getJpgEncOpenParam(JpgEncOpenParam *pEncOP, EncConfigParam *pEncConfig, char *srcYuvFileName);
int parseJpgCfgFile(ENC_CFG *pEncCfg, char *FileName);

JpgRet WriteJpgBsBufHelper(JpgDecHandle handle, BufInfo *pBufInfo,
			   PhysicalAddress paBsBufStart,
			   PhysicalAddress paBsBufEnd, int defaultsize,
			   int checkeos, int *pstreameos, int endian);

int WriteBsBufFromBufHelper(JpgDecHandle handle, jpu_buffer_t *pJbStream,
			    BYTE *pChunk, int chunkSize, int endian);

JpgRet ReadJpgBsBufHelper(JpgEncHandle handle, osal_file_t *bsFp,
			  PhysicalAddress paBsBufStart,
			  PhysicalAddress paBsBufEnd, int encHeaderSize,
			  int endian);

int LoadYuvImageHelperFormat(osal_file_t *yuvFp, Uint8 *pYuv,
			     PhysicalAddress addrY, PhysicalAddress addrCb,
			     PhysicalAddress addrCr, int picWidth,
			     int picHeight, int stride, int interleave,
			     int format, int endian, int packed);

int LoadYuvPartialImageHelperFormat(osal_file_t *yuvFp, Uint8 *pYuv, PhysicalAddress addrY,
				    PhysicalAddress addrCb, PhysicalAddress addrCr, int picWidth, int picHeight,
				    int picHeightPartial, int stride, int interleave, int format, int endian,
				    int partPosIdx, int frameIdx, int packed);

int SaveYuvImageHelperFormat(osal_file_t *yuvFp, Uint8 *pYuv,
			     PhysicalAddress addrY, PhysicalAddress addrCb,
			     PhysicalAddress addrCr, int picWidth,
			     int picHeight, int stride, int interLeave,
			     int format, int endian, int packed);

int SaveYuvPartialImageHelperFormat(osal_file_t *yuvFp, Uint8 *pYuv, PhysicalAddress addrY,
				    PhysicalAddress addrCb, PhysicalAddress addrCr, int picWidth, int picHeight,
				    int picHeightPartial, int stride, int interLeave, int format, int endian,
				    int partPosIdx, int frameIdx, int packed);

unsigned int GetFrameBufSize(int framebufFormat, int picWidth, int picHeight);
void GetMcuUnitSize(int format, int *mcuWidth, int *mcuHeight);

typedef enum {
	YUV444,
	YUV422,
	YUV420,
	NV12,
	NV21,
	YUV400,
	YUYV,
	YVYU,
	UYVY,
	VYUY,
	YYY,
	RGB_PLANAR,
	RGB32,
	RGB24,
	RGB16
} yuv2rgb_color_format;

void jpu_yuv2rgb(int width, int height, yuv2rgb_color_format format,
		 unsigned char *src, unsigned char *rgba, int chroma_reverse);
yuv2rgb_color_format
convert_jpuapi_format_to_yuv2rgb_color_format(int planar_format,
					      int pack_format, int interleave);

int comparateYuv(Uint8 *pYuv, Uint8 *pRefYuv, int picWidth, int picHeight,
		 int stride, int interleave, int format, int endian,
		 int packed);
#if defined(__cplusplus)
}
#endif

#endif //#ifndef _JPU_HELPER_H_
