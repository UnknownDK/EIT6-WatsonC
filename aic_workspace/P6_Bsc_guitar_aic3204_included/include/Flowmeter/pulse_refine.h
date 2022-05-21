/*
 * pulse_refine.h
 *
 *  Created on: 21. maj 2022
 *      Author: Mikkel S. Hansen
 */

#ifndef INCLUDE_FLOWMETER_PULSE_REFINE_H_
#define INCLUDE_FLOWMETER_PULSE_REFINE_H_

#include "sing_around.h"
#include "stdint.h"
#include "stddef.h"
#include "main.h"
#include "CrossCorrelation/cross_corr.h"

#define PR_SUCCES	(int16_t) (0)
#define PR_FAIL		(int16_t) (-1)

typedef int16_t PR_Status;

void refine_init(int32_t * src_buffer, uint16_t src_buffer_len);

PR_Status refine_pulse_time(uint16_t pulse_edge_index, uint16_t pulse_end_index, float edge_prop_time, float *refined_prop_time);

void cpy_source_buffer(uint16_t start_index, uint16_t end_index);

#endif /* INCLUDE_FLOWMETER_PULSE_REFINE_H_ */
