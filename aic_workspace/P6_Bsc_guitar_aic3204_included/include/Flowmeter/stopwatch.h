/*
 * stopwatch.h
 *
 *  Created on: 4. maj 2022
 *      Author: Mikkel S. Hansen
 */

#ifndef INCLUDE_FLOWMETER_STOPWATCH_H_
#define INCLUDE_FLOWMETER_STOPWATCH_H_

#include <csl_types.h>

typedef enum stopwatch_range {
    MILLIS_1,
    MILLIS_5,
    MILLIS_10,
    MILLIS_20
};

typedef struct {
    CSL_Instance instance;
    stopwatch_range range;
    CSL_GptObj gptObj;
    CSL_Handle hGpt;
} stopwatch_handle;

CSL_Status stopwatch_configure();

CSL_Status stopwatch_start();

CSL_Status stopwatch_pause();

CSL_Status stopwatch_stop();


#endif /* INCLUDE_FLOWMETER_STOPWATCH_H_ */
