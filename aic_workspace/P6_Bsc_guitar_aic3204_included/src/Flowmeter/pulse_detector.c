/*
 * pulse_detector.c
 *
 *  Created on: 4. maj 2022
 *      Author: Mikkel S. Hansen
 */

#include <Flowmeter/pulse_detector.h>

int16_t stop_counter = -1;
void (*detection_stop_callb)(void) = NULL;

pd_status pulse_detector_init(void (*stop_callb)(void))
{
    set_pulse_edge_detection_stop_callb(stop_callb);

    CSL_I2S2_REGS->I2SINTMASK &= ~CSL_I2S_I2SINTMASK_RCVST_MASK; // Makes sure I2S2 stereo left/right receive data interrupt is disabled

    return SUCCESS;
}

pd_status pulse_edge_detection_start()
{
    CSL_I2S2_REGS->I2SINTMASK |= CSL_I2S_I2SINTMASK_RCVST_MASK; // Enable I2S2 stereo left/right receive data interrupt

    return SUCCESS;
}

pd_status pulse_edge_detection_stop_in_n(int16_t n) {
    stop_counter = n;
}

pd_status pulse_edge_detection_stop_counter() {
    // if stop_counter == -1 the sample counter has not been started and should not count down
    if (stop_counter == -1) return FAIL;
    stop_counter--;

    if (stop_counter == 0) {
        pulse_edge_detection_stop();
        stop_counter = -1;
    }

    return SUCCESS;
}

pd_status pulse_edge_detection_stop()
{
    //IRQ_disable(RCV2_EVENT); // Disable I2S2 receive interrupt
    CSL_I2S2_REGS->I2SINTMASK &= ~CSL_I2S_I2SINTMASK_RCVST_MASK; // Disable I2S2 stereo left/right receive data interrupt

    if (detection_stop_callb != NULL) detection_stop_callb();

    return SUCCESS;
}

pd_status pulse_cross_correlate(int32_t *signal, int32_t *reference)
{
    return SUCCESS;
}

pd_status set_pulse_edge_detection_stop_callb(void (*stop_callb)(void)) {
    detection_stop_callb = stop_callb;

    return SUCCESS;
}
