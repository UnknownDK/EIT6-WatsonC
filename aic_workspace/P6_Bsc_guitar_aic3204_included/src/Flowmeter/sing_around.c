/*
 *  ======= sing_around ========
 *  sing_around target-side implementation
 *
 *  Created on: 11. maj 2022
 *  Author:     nikol
 */
#include "Flowmeter/sing_around.h"

#define MAX_TOTAL_MEAS 32
#define SOUNDPATH_LENGTH    //Length between sensors
#define ANGLE_LOOKUP 1      //til cos vaerdi der passer til sensorer


#define MAGI NULL


int16_t syngRundt(uint16_t id1, uint16_t id2, uint16_t nrRounds,uint16_t antalMeas){
    if(antalMeas > MAX_TOTAL_MEAS){
        return 0;
    }

    //sluk alle enables


    int16_t *resultArray;
    int16_t resultHolder; //half scratch var
    uint32_t timerVar;

    //memory allocation
    resultArray = (int16_t*) malloc( * sizeof(int16_t));
    if (!resultArray){
        //fejlhaandtering
        return 0;
    }

    int16_t i = 0;                      //counts measuring number
    for(i = 0; i < antalMeas ; i++){    //loop for hver maaling

        int16_t j = 0;                  //counts sing arounds
        //start ur
        for (j = 0; j < nrRounds ; j++){
            //taend for de rette kanaler
            //gpioTingDAC(id1)
            //gpioTingADC(id2)
            pulse_start();
            pulse_edge_detection_start();
            while (MAGI){       //tjek for om DMA ting har givet en hoej vaerdi

            }
            pulse_stop();
            //gpioTingDAC(id2)
            //gpioTingADC(id1)
            pulse_start();
            pulse_edge_detection_start();
            while (MAGI){       //tjek for om DMA ting har givet en hoej vaerdi

            }
            pulse_stop();
        }

        timerVar = MAGI; //save time/stop ur
        resultHolder = calcSpeedSA(timerVar, nrRounds);
        //Error handling here - increase nrRounds hvis to er
        resultArray[i] = resultHolder;


    }
    resultHolder = averageSpeed(resultArray, antalMeas);
    free(resultArray);
    return resultHolder;

}

int16_t calcSpeedSA(uint32_t time, uint16_t loops){


}

int16_t averageSpeed(int16_t speedResults[], uint16_t nrOfMeasures){

}




