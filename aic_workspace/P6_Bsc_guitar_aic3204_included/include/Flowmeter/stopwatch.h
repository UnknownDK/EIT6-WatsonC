/*
 * stopwatch.h
 *
 *  Created on: 4. maj 2022
 *      Author: Mikkel S. Hansen
 */

#ifndef INCLUDE_FLOWMETER_STOPWATCH_H_
#define INCLUDE_FLOWMETER_STOPWATCH_H_

#include <csl_types.h>
#include <csl_error.h>
#include <csl_gpt.h>
#include <stdint.h>

typedef struct {
    CSL_Instance instance;
    CSL_GptObj gptObj;
    CSL_Handle hGpt;
} stopwatch_handle;

CSL_Status stopwatch_configure(stopwatch_handle * handle);

CSL_Status stopwatch_start(stopwatch_handle * handle);

CSL_Status stopwatch_stop(stopwatch_handle * handle);

CSL_Status stopwatch_reset(stopwatch_handle * handle);

CSL_Status stopwatch_read_ns(stopwatch_handle * handle, uint32_t * ns);

#endif /* INCLUDE_FLOWMETER_STOPWATCH_H_ */
