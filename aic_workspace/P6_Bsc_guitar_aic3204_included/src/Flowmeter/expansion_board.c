/*
 * expansion_board.c
 *
 *  Created on: 11. maj 2022
 *      Author: Mikkel S. Hansen
 */

#include <Flowmeter/expansion_board.h>

CSL_Status exp_board_init(exp_board_handle handle)
{
	uint32_t gpio_mask = (1 << handle->exp_config.gpio_adc_en_1)
			| (1 << handle->exp_config.gpio_adc_en_2) | (1 << handle->exp_config.gpio_adc_en_3)
			| (1 << handle->exp_config.gpio_adc_en_4) | (1 << handle->exp_config.gpio_dac_en_1)
			| (1 << handle->exp_config.gpio_dac_en_2) | (1 << handle->exp_config.gpio_dac_en_3)
			| (1 << handle->exp_config.gpio_dac_en_4);

	uint16_t gpio_mask_lower = (uint16_t) (gpio_mask & 0xFFFF);
	uint16_t gpio_mask_upper = (uint16_t) (gpio_mask >> 16);

	if (0xF81FF000 & gpio_mask)
		return CSL_ESYS_INVPARAMS; // The configuration is trying to access GPIO pins that are not enabled in the PPMODE field of the EBSR register yet.

	// Enable GPIO outputs instead of alternative pin functionality
	// Ensure that EBSR has set SPI1_MODE2 and SP0_MODE2 to enable necessary GPIO
	if ((CSL_SYSCTRL_REGS->EBSR & CSL_SYS_EBSR_SP0MODE_MASK) != (CSL_SYS_EBSR_SP0MODE_MODE2 << CSL_SYS_EBSR_SP0MODE_SHIFT)
			|| (CSL_SYSCTRL_REGS->EBSR & CSL_SYS_EBSR_SP1MODE_MASK) != (CSL_SYS_EBSR_SP1MODE_MODE2 << CSL_SYS_EBSR_SP1MODE_SHIFT)) {
		return CSL_ESYS_FAIL;
	}

	CSL_Status status = 0;
	handle->gpio_handle = GPIO_open(&handle->gpio_obj, &status);

	if (status != CSL_SOK)
		return status;

	CSL_GpioConfig gpio_config = { 0, 0, 0, 0, 0, 0 };

	gpio_config.GPIODIRL = gpio_mask_lower;
	gpio_config.GPIODIRH = gpio_mask_upper;

	status = GPIO_config(handle->gpio_handle, &gpio_config);

	if (status != CSL_SOK)
		return status;

	return exp_board_gpio_reset_val(handle);
}

CSL_Status exp_board_gpio_reset_val(exp_board_handle handle)
{
	CSL_Status status = 0;

	// ADC enable pins are active low
	STATUS_CHECK(exp_board_disable_adc(handle)); // All ADC enable pins high
	STATUS_CHECK(exp_board_disable_dac(handle)); // All DAC enable pins low

	return CSL_SOK;
}

CSL_Status exp_board_enable_adc(exp_board_handle handle, uint16_t adc_num)
{
	if (adc_num > 4)
		return CSL_ESYS_INVPARAMS;

	return exp_board_adc_set(handle, (~(1 << (adc_num - 1)) & 0xF));
}

CSL_Status exp_board_enable_dac(exp_board_handle handle, uint16_t dac_num)
{
	if (dac_num > 4)
		return CSL_ESYS_INVPARAMS;

	return exp_board_dac_set(handle, (1 << (dac_num - 1)));
}

CSL_Status exp_board_disable_adc(exp_board_handle handle)
{
	return exp_board_adc_set(handle, 0xF);
}

CSL_Status exp_board_disable_dac(exp_board_handle handle)
{
	return exp_board_dac_set(handle, 0);
}

CSL_Status exp_board_adc_set(exp_board_handle handle, uint16_t adc_mask)
{
	if (adc_mask > 0xF)
		return CSL_ESYS_INVPARAMS;

	CSL_Status status = 0;

	GPIO_write_secure(handle->gpio_handle, handle->exp_config.gpio_adc_en_1, ((adc_mask & (1 << 0)) != 0));
	GPIO_write_secure(handle->gpio_handle, handle->exp_config.gpio_adc_en_2, ((adc_mask & (1 << 1)) != 0));
	GPIO_write_secure(handle->gpio_handle, handle->exp_config.gpio_adc_en_3, ((adc_mask & (1 << 2)) != 0));
	GPIO_write_secure(handle->gpio_handle, handle->exp_config.gpio_adc_en_4, ((adc_mask & (1 << 3)) != 0));

	return status;
}

CSL_Status exp_board_dac_set(exp_board_handle handle, uint16_t dac_mask)
{
	if (dac_mask > 0xF)
		return CSL_ESYS_INVPARAMS;

	CSL_Status status = 0;

	GPIO_write_secure(handle->gpio_handle, handle->exp_config.gpio_dac_en_1, ((dac_mask & (1 << 0)) != 0));
	GPIO_write_secure(handle->gpio_handle, handle->exp_config.gpio_dac_en_2, ((dac_mask & (1 << 1)) != 0));
	GPIO_write_secure(handle->gpio_handle, handle->exp_config.gpio_dac_en_3, ((dac_mask & (1 << 2)) != 0));
	GPIO_write_secure(handle->gpio_handle, handle->exp_config.gpio_dac_en_4, ((dac_mask & (1 << 3)) != 0));

	return status;
}
