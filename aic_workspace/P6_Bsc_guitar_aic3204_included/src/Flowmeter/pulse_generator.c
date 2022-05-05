/*
 * dma_init.c
 *
 *  Created on: 28. apr. 2022
 *      Author: tardi
 */

#include <Flowmeter/pulse_generator.h>
#include "stdint.h"

bool counting_periods = false;
uint32_t periods_left = 0;

CSL_DMA_Handle dmaHandle;

const CSL_DMAChanNum dmaChNum = CSL_DMA_CHAN4;
CSL_DMA_ChannelObj dmaChanObj;

CSL_DMA_Config dmaConfig = {
                                  CSL_DMA_PING_PONG_DISABLE,
                                  CSL_DMA_AUTORELOAD_ENABLE,
                                  CSL_DMA_TXBURST_1WORD,
                                  CSL_DMA_EVENT_TRIGGER,
                                  CSL_DMA_EVT_I2S2_TX,
                                  CSL_DMA_INTERRUPT_ENABLE,
                                  CSL_DMA_WRITE,
                                  CSL_DMA_TRANSFER_IO_MEMORY,
                                  0,
                                  0,
                                  0x2A0C
};

void set_i2s_output_value(uint32_t val);

CSL_Status pulse_generator_init(int32_t *src_addr, uint16_t src_len) {
    DMA_init();

    CSL_Status status = 0;
    dmaHandle = DMA_open(dmaChNum, &dmaChanObj, &status);

    if (status != CSL_SOK) return status;

    dmaConfig.dataLen = (uint16_t) src_len * 4; // DMA works in "byte-addressing space" and does not use CPU 16-bit addressing intervals. A 32 bit array element is therefore counted as 4 * 8 bits.
    dmaConfig.srcAddr = (uint32_t) src_addr;

    status = DMA_config(dmaHandle, &dmaConfig);

    return status;
}

CSL_Status pulse_start() {

    counting_periods = false;

    return DMA_start(dmaHandle);
}

CSL_Status pulse_start_periods(uint16_t periods) {

    counting_periods = true;

    periods_left = periods;

    return DMA_start(dmaHandle);
}

CSL_Status pulse_stop() {

    CSL_Status status = DMA_stop(dmaHandle);

    set_i2s_output_value(0);

    return status;
}

void pulse_period_finished_callb() {
    if (counting_periods) {
        // Decrement pulse period counter, and if 0, stop pulsing
        if (--periods_left == 0) pulse_stop();
    }
}

void set_i2s_output_value(uint32_t val) {
    CSL_I2S2_REGS->I2STXRT0 = val & 0xFFFF;
    CSL_I2S2_REGS->I2STXRT1 = val >> 16;
}

bool pulse_check_interrupt_flag(uint16_t ifr_mask) {
    return ifr_mask & CSL_SYS_DMAIFR_DMA1CH0IF_MASK;
}
