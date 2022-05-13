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
#include "pulse_generator.h"
#include "pulse_detector.h"
#include "expansion_board.h"


int16_t syngRundt(uint16_t id1, uint16_t id2, uint16_t nrRounds,uint16_t antalMeas);

int16_t calcSpeedSA(uint32_t time, uint16_t loops);

int16_t averageSpeed(int16_t speedResults[], uint16_t nrOfMeasures);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_FLOWMETER_SING_AROUND_H_ */
