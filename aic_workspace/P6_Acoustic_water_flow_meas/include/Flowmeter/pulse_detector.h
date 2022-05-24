/*
 * pulse_detector.h
 *
 *  Created on: 4. maj 2022
 *      Author: Mikkel S. Hansen
 */

#ifndef INCLUDE_FLOWMETER_PULSE_DETECTOR_H_
#define INCLUDE_FLOWMETER_PULSE_DETECTOR_H_

#include "stdint.h"
#include "csl_intc.h"
#include <stddef.h>

//typedef struct {
//    //void * edge_detect_callb(void);
//} hej;

typedef enum {
    SUCCESS = 0,
    FAIL,
    NO_EDGE_DETECTED
} pd_status;

/**
 * Makes sure the I2S2 interrupts are initially disabled, and sets callback functions.
 * @param func_ptr Function pointer to callback function to be called when the pulse detection stops.
 * @return Either 0 indicating SUCCESS or an error code.
 */
pd_status pulse_detector_init(void (*stop_callb)(void));

/**
 * Enables the I2S2 receive interrupt that is used for evaluating whether signal magnitude is beyond edge threshold.
 * @return Either 0 indicating SUCCESS or an error code.
 */
pd_status pulse_edge_detection_start();

/**
 * Disables the I2S2 receive interrupt that is used for evaluating whether signal magnitude is beyond edge threshold.
 * @return Either 0 indicating SUCCESS or an error code.
 */
pd_status pulse_edge_detection_stop();

/**
 * Sets the current pulse detection process to stop after capturing a specified amount samples more.
 * @param n The amount of samples to capture before stopping pulse detection.
 * @return Either 0 indicating SUCCESS or an error code.
 */
pd_status pulse_edge_detection_stop_in_n(int16_t n);

/**
 * This function should be called inside the I2S2 receive interrupt
 * and counts down for each sample if pulse_edge_detection_stop_in_n() has been called previously.
 * When the counter runs out pulse_edge_detection_stop() is called.
 * @return Either 0 indicating SUCCESS or an error code.
 */
pd_status pulse_edge_detection_stop_counter();

pd_status pulse_cross_correlate(int32_t * signal, int32_t * reference);

/**
 * Sets the pulse detection stop callback function.
 * @param func_ptr Function pointer to callback function to be called when the pulse detection stops.
 * @return Either 0 indicating SUCCESS or an error code.
 */
pd_status set_pulse_edge_detection_stop_callb(void (*stop_callb)(void));


#endif /* INCLUDE_FLOWMETER_PULSE_DETECTOR_H_ */
