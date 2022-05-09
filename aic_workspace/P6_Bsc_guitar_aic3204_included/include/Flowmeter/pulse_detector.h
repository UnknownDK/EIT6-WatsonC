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
    SUCCES = 0,
    FAIL,
    NO_EDGE_DETECTED
} pd_status;

pd_status pulse_detector_init(void (*func_ptr)(void));

pd_status pulse_edge_detection_start();

pd_status pulse_edge_detection_stop();

pd_status pulse_edge_detection_stop_in_n(int16_t n);

pd_status pulse_edge_detection_stop_counter();

pd_status pulse_cross_correlate(int32_t * signal, int32_t * reference);

pd_status set_pulse_edge_detection_stop_callb(void (*func_ptr)(void));


#endif /* INCLUDE_FLOWMETER_PULSE_DETECTOR_H_ */
