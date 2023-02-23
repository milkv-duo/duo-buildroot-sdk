/*
* Percepio Trace Recorder for Tracealyzer v4.6.4
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * @file 
 * 
 * @brief Public trace counter APIs.
 */

#ifndef TRC_COUNTER_H
#define TRC_COUNTER_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#define TRC_COUNTER_VALUE_INDEX 0
#define TRC_COUNTER_LOWER_LIMIT_INDEX 1
#define TRC_COUNTER_UPPER_LIMIT_INDEX 2

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup trace_counter_apis Trace Counter APIs
 * @ingroup trace_recorder_apis
 * @{
 */

/**
 * @brief Sets trace counter callback.
 * 
 * @param[in] xCallback Callback
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceCounterSetCallback(TraceCounterCallback_t xCallback);

/**
 * @brief Creates trace counter.
 * 
 * @param[in] szName Name.
 * @param[in] xInitialValue Initial value.
 * @param[in] xLowerLimit Lower limit.
 * @param[in] xUpperLimit Upper limit.
 * @param[out] pxCounterHandle Uninitialized trace counter handle.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceCounterCreate(const char* szName, TraceBaseType_t xInitialValue, TraceBaseType_t xLowerLimit, TraceBaseType_t xUpperLimit, TraceCounterHandle_t* pxCounterHandle);

/**
 * @brief Adds value to trace counter.
 * 
 * @param[in] xCounterHandle Initialized trace counter handle.
 * @param[in] xValue Value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceCounterAdd(xCounterHandle, xValue) xTraceCounterSet(xCounterHandle, (TraceBaseType_t)(xTraceEntryGetStateReturn((TraceEntryHandle_t)(xCounterHandle), TRC_COUNTER_VALUE_INDEX)) + (xValue))

/**
 * @brief Sets trace counter value.
 * 
 * @param[in] xCounterHandle Initialized trace counter handle. 
 * @param[in] xValue Value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceCounterSet(TraceCounterHandle_t xCounterHandle, TraceBaseType_t xValue);

/**
 * @brief Gets trace counter value.
 * 
 * @param[in] xCounterHandle Initialized trace counter handle.
 * @param[out] pxValue Returned value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceCounterGet(xCounterHandle, pxValue) xTraceEntryGetState((TraceEntryHandle_t)(xCounterHandle), TRC_COUNTER_VALUE_INDEX, (TraceUnsignedBaseType_t*)(pxValue))

/**
 * @brief Increases trace counter value.
 * 
 * @param[in] xCounterHandle Initialized trace counter handle
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceCounterIncrease(xCounterHandle) xTraceCounterAdd(xCounterHandle, 1)

/**
 * @brief Decreases trace counter value.
 * 
 * @param[in] xCounterHandle Initialized trace counter handle
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceCounterDecrease(xCounterHandle) xTraceCounterAdd(xCounterHandle, -1)

/**
 * @brief Gets trace counter upper limit.
 * 
 * @param[in] xCounterHandle Initialized trace counter handle
 * @param[out] pxValue Returned value
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceCounterGetUpperLimit(xCounterHandle, pxValue) xTraceEntryGetState((TraceEntryHandle_t)(xCounterHandle), TRC_COUNTER_UPPER_LIMIT_INDEX, (TraceUnsignedBaseType_t*)(pxValue))

/**
 * @brief Gets trace counter lower limit.
 * 
 * @param[in] xCounterHandle Initialized trace counter handle
 * @param[out] pxValue Returned value
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceCounterGetLowerLimit(xCounterHandle, pxValue) xTraceEntryGetState((TraceEntryHandle_t)(xCounterHandle), TRC_COUNTER_LOWER_LIMIT_INDEX, (TraceUnsignedBaseType_t*)(pxValue))

/**
 * @brief Gets trace counter name.
 * 
 * @param[in] xCounterHandle Initialized trace counter handle.
 * @param[out] pszName Returned name.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceCounterGetName(xCounterHandle, pszName) xTraceEntryGetSymbol((TraceEntryHandle_t)(xCounterHandle), pszName)

/** @} */

#ifdef __cplusplus
}
#endif

#else

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterSetCallback(__xCallback) ((void)(__xCallback), TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterCreate(__szName, __xInitialValue, __xLowerLimit, __xUpperLimit, __pxCounterHandle) ((void)(__szName), (void)(__xInitialValue), (void)(__xLowerLimit), (void)(__xUpperLimit), *(__pxCounterHandle) = 0, TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterAdd(__xCounterHandle, __xValue) ((void)(__xCounterHandle), (void)(__xValue), TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterSet(__xCounterHandle, __xValue) ((void)(__xCounterHandle), (void)(__xValue), TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterGet(__xCounterHandle, __pxValue) ((void)(__xCounterHandle), *(__pxValue) = 0, TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterIncrease(__xCounterHandle) ((void)(__xCounterHandle), TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterDecrease(__xCounterHandle) ((void)(__xCounterHandle), TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterGetUpperLimit(__xCounterHandle, __pxValue) ((void)(__xCounterHandle), *(__pxValue) = 0, TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterGetLowerLimit(__xCounterHandle, __pxValue) ((void)(__xCounterHandle), *(__pxValue) = 0, TRC_SUCCESS)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceCounterGetName(__xCounterHandle, __pszName) ((void)(__xCounterHandle), *(__pszName) = "N/A", TRC_SUCCESS)

#endif

#endif

#endif
