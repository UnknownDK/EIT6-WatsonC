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

    uint32_t period = 0;
    status = get_timer_period(handle->range, &period);

    if (status != CSL_SOK) return status;

    tim_config.prdLow = (uint16_t) (period & 0xFFFF);
    tim_config.prdHigh = (uint16_t) (period >> 16);

    status = GPT_config(handle->hGpt, tim_config);

    stopwatch_reset();

    return status;
}

CSL_Status get_timer_period(stopwatch_range range, uint32_t * period) {
    switch (range) {
    case MILLIS_1:
        break;
    case MILLIS_5:
        break;
    case MILLIS_10:
        break;
    case MIllis_20:
        break;
    default:
        return CSL_SOK;
    }
    return CSL_ESYS_INVPARAMS;
}

CSL_Status stopwatch_start()
{

}

CSL_Status stopwatch_pause()
{

}

CSL_Status stopwatch_stop()
{

}

CSL_Status stopwatch_reset() {

}
