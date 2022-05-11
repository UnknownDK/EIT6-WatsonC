/*
 * dma_init.h
 *
 *  Created on: 28. apr. 2022
 *      Author: tardi
 */

#ifndef INCLUDE_FLOWMETER_PULSE_GENERATOR_H_
#define INCLUDE_FLOWMETER_PULSE_GENERATOR_H_

#include "soc.h"
#include "error_handler.h"
#include "stdint.h"
#include "csl_dma.h"
#include "stdbool.h"
#include "csl_types.h"

/**
 * Configures and initializes DMA channel and internal members.
 * @param src_addr Start address of the source buffer.
 * @param src_len Amount of 32 bit elements in the source buffer.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status pulse_generator_init(int32_t *src_addr, uint16_t src_len);

/**
 * Starts a pulse sampled from the source buffer that continues until stopped.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status pulse_start();

/**
 * Starts a pulse sampled from the source buffer and repeats in a specified amount of times before stopping the pulse.
 * @param repetitions The amount of source buffer repetitions.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status pulse_start_periods(uint16_t repetitions);

/**
 * Stops the currently running pulse.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status pulse_stop();

/**
 * This function should be called in the DMA transfer complete interrupt every time a pulse repetition is completed.
 */
void pulse_repetition_ended_callb();

/**
 * This function checks if the DMA complete interrupt flag for the pulse channel is set.
 * @param ifr_mask The contents of the DMA interrupt flag register.
 * @return Returns true if the interrupt flag is set, otherwise false is returned.
 */
bool pulse_check_interrupt_flag(uint16_t ifr_mask);

#endif /* INCLUDE_FLOWMETER_PULSE_GENERATOR_H_ */
