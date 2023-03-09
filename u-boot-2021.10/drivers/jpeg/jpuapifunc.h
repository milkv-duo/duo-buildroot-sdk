#ifndef _JPUAPI_UTIL_H_
#define _JPUAPI_UTIL_H_

#include "jpuapi.h"
#include "regdefine.h"

#define DC_TABLE_INDEX0				0
#define AC_TABLE_INDEX0				1
#define DC_TABLE_INDEX1				2
#define AC_TABLE_INDEX1				3

#define Q_COMPONENT0				0
#define Q_COMPONENT1				0x40
#define Q_COMPONENT2				0x80

typedef enum {
	JPG_START_PIC = 0,
	JPG_START_INIT,
	JPG_START_STOP,
	JPG_START_PARTIAL
} JpgStartCmd;

typedef struct{
	UINT	tag;
	UINT	type;
	int		count;
	int		offset;
} TAG;

enum {
	JFIF		= 0,
	JFXX_JPG	= 1,
	JFXX_PAL	= 2,
	JFXX_RAW	= 3,
	EXIF_JPG	= 4
};

typedef struct {
	int			PicX;
	int			PicY;
	int			BitPerSample[3];
	int			Compression; // 1 for uncompressed / 6 for compressed(jpeg)
	int			PixelComposition; // 2 for RGB / 6 for YCbCr
	int			SamplePerPixel;
	int		PlanrConfig; // 1 for chunky / 2 for planar
	int			YCbCrSubSample; // 00020002 for YCbCr 4:2:0 / 00020001 for YCbCr 4:2:2
	UINT		JpegOffset;
	UINT		JpegThumbSize;
} EXIF_INFO;

typedef struct {
	BYTE *buffer;
	int index;
	int size;
} vpu_getbit_context_t;

#define init_get_bits(CTX, BUFFER, SIZE) JpuGbuInit(CTX, BUFFER, SIZE)
#define show_bits(CTX, NUM) JpuGguShowBit(CTX, NUM)
#define get_bits(CTX, NUM) JpuGbuGetBit(CTX, NUM)
#define get_bits_left(CTX) JpuGbuGetLeftBitCount(CTX)
#define get_bits_count(CTX) JpuGbuGetUsedBitCount(CTX)

typedef struct {
	PhysicalAddress streamWrPtr;
	PhysicalAddress streamRdPtr;
	int	streamEndflag;
	PhysicalAddress streamBufStartAddr;
	PhysicalAddress streamBufEndAddr;
	int streamBufSize;
	BYTE *pBitStream;

	int frameOffset;
	int consumeByte;
	int nextOffset;
	int currOffset;

	FrameBuffer *frameBufPool;
	int numFrameBuffers;
	int stride;
	int strideY;
	int strideC;
	int rotationEnable;
	int mirrorEnable;
	int mirrorDirection;
	int rotationAngle;
	FrameBuffer rotatorOutput;
	int rotatorStride;
	int rotatorOutputValid;
	int initialInfoObtained;
	int minFrameBufferNum;
	int streamEndian;
	int frameEndian;
	int chroma_interleave;

	int picWidth;
	int picHeight;
	int alignedWidth;
	int alignedHeight;
	int headerSize;
	int ecsPtr;
	int pagePtr;
	int wordPtr;
	int bitPtr;
	int format;
	int rstIntval;

	int userHuffTab;

	int huffDcIdx;
	int huffAcIdx;
	int Qidx;

	BYTE huffVal[4][162];
	BYTE huffBits[4][256];
	BYTE cInfoTab[4][6];
	BYTE qMatTab[4][64];

	Uint32 huffMin[4][16];
	Uint32 huffMax[4][16];
	BYTE huffPtr[4][16];

	// partial
	int usePartial;
	int lineNum;
	int bufNum;

	int busReqNum;
	int compNum;
	int mcuBlockNum;
	int compInfo[3];
	int frameIdx;
	int bitEmpty;
	int iHorScaleMode;
	int iVerScaleMode;
	int mcuWidth;
	int mcuHeight;
	vpu_getbit_context_t gbc;

#ifdef MJPEG_ERROR_CONCEAL
	//error conceal
	struct{
		int bError;
		int rstMarker;
		int errPosX;
		int errPosY;
	} errInfo;

	int curRstIdx;
	int nextRstIdx;
	int setPosX;
	int setPosY;
	int gbuStartPtr;         // entry point in stream buffer before pic_run

	int numRstMakerRounding;
#endif

	//ROI
	int roiEnable;
	int roiOffsetX;
	int roiOffsetY;
	int roiWidth;
	int roiHeight;
	int roiMcuOffsetX;
	int roiMcuOffsetY;
	int roiMcuWidth;
	int roiMcuHeight;
	int packedFormat;
} JpgDecInfo;

typedef struct {
	JpgEncOpenParam openParam;
	JpgEncInitialInfo initialInfo;
	PhysicalAddress streamRdPtr;
	PhysicalAddress streamWrPtr;
	PhysicalAddress streamBufStartAddr;
	PhysicalAddress streamBufEndAddr;
	int streamBufSize;

	FrameBuffer *frameBufPool;
	int numFrameBuffers;
	int stride;
	int rotationEnable;
	int mirrorEnable;
	int mirrorDirection;
	int rotationAngle;
	int initialInfoObtained;

	int picWidth;
	int picHeight;
	int alignedWidth;
	int alignedHeight;
	int seqInited;
	int frameIdx;
	int format;

	int streamEndian;
	int frameEndian;
	int chroma_interleave;

	int rstIntval;
	int busReqNum;
	int mcuBlockNum;
	int compNum;
	int compInfo[3];

	// give command
	int disableAPPMarker;
	int quantMode;
	int stuffByteEnable;

	Uint32 huffCode[4][256];
	Uint32 huffSize[4][256];
	BYTE *pHuffVal[4];
	BYTE *pHuffBits[4];
	BYTE *pCInfoTab[4];
	BYTE *pQMatTab[4];
	// partial
	int usePartial;
	int partiallineNum;
	int partialBufNum;
	int packedFormat;

	JpgEncParamSet *paraSet;

} JpgEncInfo;

typedef struct JpgInst {
	int inUse;
	int instIndex;
	int loggingEnable;
	union {
		JpgEncInfo encInfo;
		JpgDecInfo decInfo;
	} JpgInfo;
} JpgInst;

#ifdef __cplusplus
extern "C" {
#endif

	JpgRet InitJpgInstancePool(void);
	JpgRet GetJpgInstance(JpgInst **ppInst);
	void	FreeJpgInstance(JpgInst *pJpgInst);
	JpgRet CheckJpgInstValidity(JpgInst *pci);
	JpgRet CheckJpgDecOpenParam(JpgDecOpenParam *pop);

	int JpuGbuInit(vpu_getbit_context_t *ctx, BYTE *buffer, int size);
	int JpuGbuGetUsedBitCount(vpu_getbit_context_t *ctx);
	int JpuGbuGetLeftBitCount(vpu_getbit_context_t *ctx);
	unsigned int JpuGbuGetBit(vpu_getbit_context_t *ctx, int bit_num);
	unsigned int JpuGguShowBit(vpu_getbit_context_t *ctx, int bit_num);

	int JpegDecodeHeader(JpgDecInfo *jpg);
	int JpgDecQMatTabSetUp(JpgDecInfo *jpg);
	int JpgDecHuffTabSetUp(JpgDecInfo *jpg);
	void JpgDecGramSetup(JpgDecInfo *jpg);

	JpgRet CheckJpgEncOpenParam(JpgEncOpenParam *pop);
	JpgRet CheckJpgEncParam(JpgEncHandle handle, JpgEncParam *param);
	int JpgEncLoadHuffTab(JpgEncInfo *pJpgEncInfo);
	int JpgEncLoadQMatTab(JpgEncInfo *pJpgEncInfo);
	int JpgEncEncodeHeader(JpgEncHandle handle, JpgEncParamSet *para);

	JpgRet JpgEnterLock(void);
	JpgRet JpgLeaveLock(void);
	JpgRet JpgSetClockGate(Uint32 on);

	void SetJpgPendingInst(JpgInst *inst);
	void ClearJpgPendingInst(void);
	JpgInst *GetJpgPendingInst(void);

#ifdef MJPEG_ERROR_CONCEAL
	int JpegDecodeConcealError(JpgDecInfo *jpg);
#endif

#ifdef __cplusplus
}
#endif

#endif //_JPUAPI_UTIL_H_
