/*
 * dma_init.c
 *
 *  Created on: 28. apr. 2022
 *      Author: tardi
 */

#include <pulse_generator.h>
#include "stdint.h"

bool init_complete = false;
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

void pulse_generator_init(int32_t *src_addr, uint16_t src_len) {
    DMA_init();

    CSL_Status status = 0;
    dmaHandle = DMA_open(dmaChNum, &dmaChanObj, &status);

    dmaConfig.dataLen = (uint16_t) src_len * 4;
    dmaConfig.srcAddr = (uint32_t) src_addr;


    status = DMA_config(dmaHandle, &dmaConfig);

    init_complete = true;
}

int16_t pulse_start() {

    if (!init_complete) return 0;

    counting_periods = false;

    CSL_Status status = DMA_start(dmaHandle);

    return 1;
}

int16_t pulse_start_periods(uint16_t periods) {
    if (!init_complete) return 0;

    counting_periods = true;

    periods_left = periods;

    CSL_Status status = DMA_start(dmaHandle);

    return 1;
}

int16_t pulse_stop() {
    if (!init_complete) return 0;

    CSL_Status status = DMA_stop(dmaHandle);

    set_i2s_output_value(0);

    return 1;
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
