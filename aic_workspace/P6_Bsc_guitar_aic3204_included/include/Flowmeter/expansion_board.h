/*
 * expansion_board.h
 *
 *  Created on: 11. maj 2022
 *      Author: Mikkel S. Hansen
 */

#ifndef INCLUDE_FLOWMETER_EXPANSION_BOARD_H_
#define INCLUDE_FLOWMETER_EXPANSION_BOARD_H_

#include <csl_gpio.h>
#include <stdint.h>
#include <main.h>

#define GPIO_write_secure(a, b, c) STATUS_CHECK(GPIO_write(a, b, c))

typedef struct
{
	CSL_GpioPinNum gpio_adc_en_1;
	CSL_GpioPinNum gpio_adc_en_2;
	CSL_GpioPinNum gpio_adc_en_3;
	CSL_GpioPinNum gpio_adc_en_4;
	CSL_GpioPinNum gpio_dac_en_1;
	CSL_GpioPinNum gpio_dac_en_2;
	CSL_GpioPinNum gpio_dac_en_3;
	CSL_GpioPinNum gpio_dac_en_4;
} exp_board_config;

typedef struct {
	exp_board_config exp_config;
	CSL_GpioObj gpio_obj;
	GPIO_Handle gpio_handle;
} exp_board_obj;

typedef exp_board_obj* exp_board_handle;

/**
 * Enable gpio and set pin directions.
 * @param handle
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status exp_board_init(exp_board_handle handle);

/**
 * Reset ADC and DAC enable pins to their initial disabled states.
 * @param handle
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status exp_board_gpio_reset_val(exp_board_handle handle);

/**
 * Enable the ADC signal to the specified transducer board.
 * @param handle
 * @param adc_num Transducerboard number (1-4).
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status exp_board_enable_adc(exp_board_handle handle, uint16_t adc_num);

/**
 * Enable the DAC signal to the specified transducer board.
 * @param handle
 * @param dac_num Transducer board number (1-4).
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status exp_board_enable_dac(exp_board_handle handle, uint16_t dac_num);

/**
 * Disable all ADC signals.
 * @param handle
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status exp_board_disable_adc(exp_board_handle handle);

/**
 * Disable all DAC signals.
 * @param handle
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status exp_board_disable_dac(exp_board_handle handle);

/**
 * Sets all ADC enable pin states using a bit mask.
 * @param handle
 * @param adc_mask Bit mask (bit 0, 1, 2 and 3) indicating the HIGH/LOW output state of the 4 enable pins.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status exp_board_adc_set(exp_board_handle handle, uint16_t adc_mask);

/**
 * Sets all DAC enable pin states using a bit mask.
 * @param handle
 * @param dac_mask Bit mask (bit 0, 1, 2 and 3) indicating the HIGH/LOW output state of the 4 enable pins.
 * @return CSL success or error codes found in csl_error.h
 */
CSL_Status exp_board_dac_set(exp_board_handle handle, uint16_t dac_mask);

#endif /* INCLUDE_FLOWMETER_EXPANSION_BOARD_H_ */
