/*
 * pulse_refine.c
 *
 *  Created on: 20. maj 2022
 *      Author: Mikkel S. Hansen
 */

#include "Flowmeter/pulse_refine.h"

int16_t in_sig_array[IN_SIG_LEN];

int32_t * source_buffer = NULL;
uint16_t source_buffer_len = 0;

uint16_t get_start_index(uint16_t edge_index, uint16_t len);
uint16_t get_end_index(uint16_t end_index, uint16_t len);

void refine_init(int32_t * src_buffer, uint16_t src_buffer_len) {
	source_buffer = src_buffer;
	source_buffer_len = src_buffer_len;
}

PR_Status refine_pulse_time(SA_pulse_result pulse, float *refined_prop_time) {

	uint16_t start_index = get_start_index(pulse.edge_index, source_buffer_len);
	uint16_t end_index = get_end_index(pulse.end_index, source_buffer_len);

	memory_set(in_sig_array, 0, sizeof(in_sig_array));

	cpy_source_buffer(start_index, end_index);

	// FDZP

	// Cross correlation

	*refined_prop_time = 0;
}

void cpy_source_buffer(uint16_t start_index, uint16_t end_index) {

	if (start_index < end_index) {
		// The pulse samples are contiguous
		cpy_int32_array_to_int16(source_buffer + start_index, in_sig_array, end_index - start_index);
	} else {
		// The pulse samples are NOT contiguous and the pulse has wrapped around the end of the array
		cpy_int32_array_to_int16(source_buffer + start_index, in_sig_array, source_buffer_len - start_index);
		cpy_int32_array_to_int16(source_buffer, in_sig_array + source_buffer_len - start_index, end_index);
	}
}

uint16_t get_start_index(uint16_t edge_index, uint16_t len) {
	int32_t index = edge_index - PULSE_SAMPLE_MARGIN;
	if (index < 0) index += len;
	return (uint16_t) index;
}

uint16_t get_end_index(uint16_t end_index, uint16_t len) {
	int32_t index = end_index + PULSE_SAMPLE_MARGIN;
	if (index > len) index -= len;
	return (uint16_t) index;
}
