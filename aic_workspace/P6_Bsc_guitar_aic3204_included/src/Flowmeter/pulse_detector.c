/*
 * pulse_detector.c
 *
 *  Created on: 4. maj 2022
 *      Author: Mikkel S. Hansen
 */

#include <Flowmeter/pulse_detector.h>

pd_status pulse_detector_init()
{
    // Do we need any initialization???
}

pd_status pulse_edge_detection_start()
{
    CSL_I2S2_REGS->I2SINTMASK |= CSL_I2S_I2SINTMASK_RCVST_MASK; // Enable I2S2 stereo left/right receive data interrupt
}

pd_status pulse_edge_detection_stop()
{
    //IRQ_disable(RCV2_EVENT); // Disable I2S2 receive interrupt
    CSL_I2S2_REGS->I2SINTMASK &= ~CSL_I2S_I2SINTMASK_RCVST_MASK; // Disable I2S2 stereo left/right receive data interrupt
}

pd_status pulse_cross_correlate(int32_t *signal, int32_t *reference)
{

}
