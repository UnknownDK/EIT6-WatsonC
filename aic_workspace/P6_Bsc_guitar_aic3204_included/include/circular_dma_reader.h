/*
 * circular_dma_reader.h
 *
 *  Created on: 3. maj 2022
 *      Author: Mikkel S. Hansen
 */

#ifndef INCLUDE_CIRCULAR_DMA_READER_H_
#define INCLUDE_CIRCULAR_DMA_READER_H_

#include "csl_error.h"
#include "stdint.h"
#include "stddef.h"
#include "csl_dma.h"
#include "csl_types.h"

// Appropriate reset value for when defining handles, before initialization
#define CIRCULAR_DMA_READER_HANDLER_RESET { CSL_DMA_CHAN_INV, NULL }

typedef struct {
    CSL_DMAChanNum  chan;   // DMA controller and channel
    CSL_DMAEvtType evtType;
    int32_t * src_addr;     // Pointer to source register
    int32_t * dest_addr;    // Pointer to destination buffer
    uint16_t buffer_len;    // Destination buffer length
} circular_dma_reader_config;

typedef struct {
    CSL_DMAChanNum dmaChNum;
    CSL_DMA_Handle dmaHandle;
    CSL_DMA_Config dmaConfig;
} circular_dma_reader_handle;

CSL_Status reader_init(circular_dma_reader_handle * handle, circular_dma_reader_config * config);

CSL_Status reader_start(circular_dma_reader_handle * handle);

CSL_Status reader_stop(circular_dma_reader_handle * handle);

#endif /* INCLUDE_CIRCULAR_DMA_READER_H_ */
