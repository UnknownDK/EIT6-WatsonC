/*
 * stopwatch.c
 *
 *  Created on: 4. maj 2022
 *      Author: Mikkel S. Hansen
 */

#include <Flowmeter/stopwatch.h>

CSL_Config tim_config = {
                         GPT_AUTO_DISABLE,
                         GPT_PRE_SC_DIV_0,
                         GPT_TIMER_ENABLE,
                         0,
                         0
};


CSL_Status stopwatch_configure(stopwatch_handle * handle)
{
    CSL_Status status = CSL_SOK;
    handle->hGpt = GPT_open(handle->instance, &handle->gptObj, &status);

    if (status != CSL_SOK) return status;

    tim_config.prdLow = 0xFFFF;
    tim_config.prdHigh = 0xFFFF;

    status = GPT_config(handle->hGpt, &tim_config);

    stopwatch_reset(handle);

    return status;
}

CSL_Status stopwatch_start(stopwatch_handle * handle)
{
    return GPT_start(handle->hGpt);
}

CSL_Status stopwatch_stop(stopwatch_handle * handle)
{
    return GPT_stop(handle->hGpt);
}

CSL_Status stopwatch_reset(stopwatch_handle * handle) {
    if (handle == NULL) return CSL_ESYS_BADHANDLE;

    /* Set GPT counter register to default value as 0                           */
    handle->hGpt->regs->TIMCNT1 = CSL_TIM_TIMCNT1_RESETVAL;
    handle->hGpt->regs->TIMCNT2 = CSL_TIM_TIMCNT2_RESETVAL;

    return CSL_SOK;
}

CSL_Status stopwatch_read_ns(stopwatch_handle * handle, uint32_t * ns) {
    // By default and using internal 32 kHz clock the SYS_CLK is 36.846 MHz
    uint32_t cnt = 0;
    CSL_Status status = GPT_getCnt(handle->hGpt, &cnt);

    if (status != CSL_SOK) return status;

    *ns = (uint32_t) (cnt * 27.1267361111111111111111);

    return CSL_SOK;
}
