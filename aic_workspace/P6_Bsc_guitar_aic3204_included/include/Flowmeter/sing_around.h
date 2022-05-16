/*
 * sing_around.h
 *
 *  Created on: 11. maj 2022
 *      Author: nikol
 */

#ifndef INCLUDE_FLOWMETER_SING_AROUND_H_
#define INCLUDE_FLOWMETER_SING_AROUND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "pulse_generator.h"
#include "pulse_detector.h"
#include "expansion_board.h"
#include "stopwatch.h"

#define MAX_MALLOC 4096
#define ANGLE_LOOKUP 0.707      //til cos vaerdi der passer til sensorer
#define SOUNDPATH_LENGTH  15    //Length between sensors
#define Q 15
#define FREQ_Q 13
#define Q_DIF Q-FREQ_Q
#define REPETITIONS 1
#define CALIB_FACTOR 5479   //kan evt traekkes fra


typedef struct {
    stopwatch_handle *watch;
    exp_board_handle expBoard;
    uint16_t sensorID1;
    uint16_t sensorID2;
    bool *edgeDetected;
} SA_station_obj;

typedef SA_station_obj* SA_station_handle;


int16_t singAround(SA_station_handle sa_station, uint16_t nrRounds,uint16_t antalMeas);

void measureOneWay(SA_station_handle station, bool direction, uint32_t *timerVar);

uint16_t calcFreqQ(uint32_t time, uint16_t Q_outFormat);

int16_t averageSpeed(int16_t speedResults[], uint16_t nrOfMeasures);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_FLOWMETER_SING_AROUND_H_ */
