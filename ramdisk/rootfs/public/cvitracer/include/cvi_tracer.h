#ifndef _CVI_TRACER_H
#define _CVI_TRACER_H

#ifdef __cplusplus
extern "C" {
#endif

void CVI_SYS_TraceBegin(const char *name);

void CVI_SYS_TraceCounter(const char *name, signed int value);

void CVI_SYS_TraceEnd();

#ifdef __cplusplus
}
#endif

#endif // End of _CVI_TRACER_H
