/*
 * pulse_refine.c
 *
 *  Created on: 20. maj 2022
 *      Author: Mikkel S. Hansen
 */

#include "Flowmeter/pulse_refine.h"
#include "Flowmeter/sing_around.h"

short in_sig_array[OUTSIGLEN];

long fdzp_array[FDZPARRAYLEN];

short result_corr[RSLTCORRLEN];

int32_t * source_buffer = NULL;
uint16_t source_buffer_len = 0;

uint16_t get_start_index(uint16_t edge_index, uint16_t len);
uint16_t get_end_index(uint16_t end_index, uint16_t len);

void refine_init(int32_t * src_buffer, uint16_t src_buffer_len) {
	source_buffer = src_buffer;
	source_buffer_len = src_buffer_len;
}

PR_Status refine_pulse_time(uint16_t pulse_edge_index, uint16_t pulse_end_index, float edge_prop_time, float *refined_prop_time) {

	uint16_t start_index = get_start_index(pulse_edge_index, source_buffer_len);
	uint16_t end_index = get_end_index(pulse_end_index, source_buffer_len);

	memory_set((uint16_t *) in_sig_array, 0, sizeof(in_sig_array));

	cpy_source_buffer(start_index, end_index);

	// FDZP
	fdzp(in_sig_array, fdzp_array, INSIGLEN, OUTSIGLEN);

	// Pick out real (and not imaginary) values
	uint16_t i = 0;
	for (i = 0; i < OUTSIGLEN; i++)
	{
		// Reverting to only real
		in_sig_array[i] = (short) (fdzp_array[i * 2]);
	}

	// Cross correlation
	// Til Bjørn: (Antal nuller foran + signallængde)*Interp_f
	uint16_t sample_offset = crosscorr(in_sig_array, result_corr, INSIGLEN, OUTSIGLEN, COMPSIGLEN);

	// Calculate the more accurate propagation time
	*refined_prop_time = (float) (edge_prop_time - (float) PULSE_SAMPLES_START_MARGIN / S_RATE) + ((float) (sample_offset + 1) / (S_RATE * INTERP_F));

	return PR_SUCCES;
}

void cpy_source_buffer(uint16_t start_index, uint16_t end_index) {

	if (start_index < end_index) {
		// The pulse samples are contiguous
		cpy_int32_array_to_int16((int32_t * ) source_buffer + start_index, (int16_t *) in_sig_array, end_index - start_index);
	} else {
		// The pulse samples are NOT contiguous and the pulse has wrapped around the end of the array
		cpy_int32_array_to_int16((int32_t * ) source_buffer + start_index, (int16_t *) in_sig_array, source_buffer_len - start_index);
		cpy_int32_array_to_int16((int32_t * ) source_buffer, (int16_t *) in_sig_array + source_buffer_len - start_index, end_index);
	}
}

uint16_t get_start_index(uint16_t edge_index, uint16_t len) {
	int32_t index = edge_index - PULSE_SAMPLES_START_MARGIN;
	if (index < 0) index += len;
	return (uint16_t) index;
}

uint16_t get_end_index(uint16_t end_index, uint16_t len) {
	int32_t index = end_index + PULSE_SAMPLES_END_MARGIN;
	if (index > len) index -= len;
	return (uint16_t) index;
}
