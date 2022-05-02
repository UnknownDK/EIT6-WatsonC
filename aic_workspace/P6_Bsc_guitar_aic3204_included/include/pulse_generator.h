/*
 * dma_init.h
 *
 *  Created on: 28. apr. 2022
 *      Author: tardi
 */

#ifndef INCLUDE_PULSE_GENERATOR_H_
#define INCLUDE_PULSE_GENERATOR_H_

#include "soc.h"
#include "error_handler.h"
#include "stdint.h"
#include "csl_dma.h"
#include "stdbool.h"

void pulse_generator_init(int32_t *src_addr, uint16_t src_len);

int16_t pulse_start();

int16_t pulse_start_periods(uint16_t periods);

int16_t pulse_stop();

void pulse_period_finished_callb();

bool pulse_check_interrupt_flag(uint16_t ifr_mask);

#endif /* INCLUDE_PULSE_GENERATOR_H_ */
