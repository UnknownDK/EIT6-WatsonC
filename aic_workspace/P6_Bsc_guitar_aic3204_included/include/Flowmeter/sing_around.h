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
#include "ezdsp5535.h"
#include "circular_dma_reader.h"

#define MAX_MALLOC 4096
#define ANGLE_LOOKUP 1      //til cos vaerdi der passer til sensorer. 1 Svarer til at sensorer står på en linje midt i strømmen
#define SOUNDPATH_LENGTH  20    //Length between sensors
#define Q 15
#define FREQ_Q 13
#define Q_DIF Q-FREQ_Q
#define REPETITIONS 2
#define CALIB_FACTOR 383000   //kan evt traekkes fra
#define TIMEOUT // Number of samples to wait for the receival of a pulse before calling timeout

typedef enum {
	DOWNSTREAM,
	UPSTREAM
} SA_direction;

typedef struct {
	float prop_time_upstream;
	float prop_time_downstream;
	float delta_freq;
} SA_round_result;

typedef struct {
    stopwatch_handle *watch;
    exp_board_handle expBoard;
    uint16_t sensor_upstream;
    uint16_t sensor_downstream;
    bool *propagating;
    bool *prompt_gen_start;
    circular_dma_reader_handle reader_handle;
} SA_station_obj;


typedef SA_station_obj* SA_station_handle;


int16_t singAround(SA_station_handle sa_station, uint16_t nrRounds,uint16_t antalMeas);

void sing_one_way(SA_station_handle station, SA_direction dir, float *prop_time);

void sing_one_round(SA_station_handle station, SA_round_result * result);

uint16_t calcFreqQ(uint32_t time, uint16_t Q_outFormat);

float averageSpeed(float speedResults[], uint16_t nrOfMeasures);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_FLOWMETER_SING_AROUND_H_ */
