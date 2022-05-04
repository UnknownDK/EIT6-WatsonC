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

//typedef struct {
//    //void * edge_detect_callb(void);
//} hej;

typedef enum {
    SUCCES = 0,
    FAIL,
    NO_EDGE_DETECTED
} pd_status;

pd_status pulse_detector_init();

pd_status pulse_edge_detection_start();

pd_status pulse_edge_detection_stop();

pd_status pulse_cross_correlate(int32_t * signal, int32_t * reference);


#endif /* INCLUDE_FLOWMETER_PULSE_DETECTOR_H_ */
