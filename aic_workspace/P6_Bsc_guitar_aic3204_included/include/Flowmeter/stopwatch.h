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


/**
 * Timer initialization and configuration.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status stopwatch_configure(stopwatch_handle * handle);

/**
 * Resets the stopwatch timer counter and starts it.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status stopwatch_start(stopwatch_handle * handle);

/**
 * Stops the stopwatch timer counter.
 * @param handle
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status stopwatch_stop(stopwatch_handle * handle);

/**
 * Gives the amount of time that the stopwatch has counted in nanoseconds. There has not been taken account for code headroom.
 * @param handle
 * @param ns The amount of nanoseconds that the stopwatch has counted. It can count to a maximum 2^32 nanoseconds, meaning approximately 4.29 seconds.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status stopwatch_read_ns(stopwatch_handle * handle, uint32_t * ns);

#endif /* INCLUDE_FLOWMETER_STOPWATCH_H_ */
