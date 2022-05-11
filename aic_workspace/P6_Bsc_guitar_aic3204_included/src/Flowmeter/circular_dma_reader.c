#include <Flowmeter/circular_dma_reader.h>



CSL_Status reader_init(circular_dma_reader_handle * handle, circular_dma_reader_config * config) {

    // Setup DMA configuration struct
    handle->dmaChNum = config->chan;
    handle->dmaHandle = &handle->dmaChObj;
    handle->dmaConfig.dmaEvt = config->evtType;
    handle->dmaConfig.srcAddr = (uint32_t) config->src_addr;
    handle->dmaConfig.destAddr = (uint32_t) config->dest_addr;
    handle->dmaConfig.dataLen = config->buffer_len * 4; // DMA works in "byte-addressing space" and does not use CPU 16-bit addressing intervals. A 32 bit array element is therefore counted as 4 * 8 bits.

    // Check for invalid parameters
    if (handle->dmaChNum == CSL_DMA_CHAN_INV
            || handle->dmaConfig.dmaEvt == CSL_DMA_EVT_INVALID
            || handle->dmaConfig.srcAddr == 0
            || handle->dmaConfig.destAddr == 0
            || handle->dmaConfig.dataLen == 0) return CSL_ESYS_INVPARAMS;

    CSL_Status status = CSL_SOK;

    handle->dmaHandle = DMA_open(handle->dmaChNum, handle->dmaHandle, &status);

    if (status != CSL_SOK) return status;

    status = DMA_config(handle->dmaHandle, &handle->dmaConfig);

    if (status != CSL_SOK) return status;

    return status;
}

CSL_Status reader_start(circular_dma_reader_handle * handle) {
    return DMA_start(handle->dmaHandle);
}

CSL_Status reader_stop(circular_dma_reader_handle * handle) {
    return DMA_stop(handle->dmaHandle);
}

uint16_t reader_get_index() {
    // Get array index where next read value is written to
    return 0;
}
