/*
 *  ======= sing_around ========
 *  sing_around target-side implementation
 *
 *  Created on: 11. maj 2022
 *  Author:     nikol
 */
#include "Flowmeter/sing_around.h"

#define MAX_MALLOC 4096
#define ANGLE_LOOKUP 0.707      //til cos vaerdi der passer til sensorer
#define SOUNDPATH_LENGTH  15    //Length between sensors
#define Q 15



#define MAGI NULL


int16_t syngRundt(exp_board_handle expboard, uint16_t id1, uint16_t id2, uint16_t nrRounds,uint16_t antalMeas ){
    exp_board_disable_adc(expboard); //disable
    exp_board_disable_dac(expboard);


    if((antalMeas+nrRounds) > MAX_MALLOC){
        return 0;
    }
  


    //memory allocation
    int16_t *singArray;
    int16_t *resultArray;
    singArray = (int16_t*) malloc(nrRounds * sizeof(int16_t));
    if (!singArray){
        //fejlhaandtering
        return 0;
    }
    resultArray = (int16_t*) malloc(antalMeas * sizeof(int16_t));
    if (!resultArray){
        free(singArray);
        //fejlhaandtering
        return 0;
    }

    const float factor = (SOUNDPATH_LENGTH/(2*ANGLE_LOOKUP)) / ((int16_t)(1 << Q));
    float singResultHolder; //half scratch var
    int16_t resultHolder;
    uint32_t timerVar;


    int16_t i = 0;                      //counts measuring number
    for(i = 0; i < antalMeas ; i++){    //loop for hver maaling

        int16_t j = 0;                  //counts sing arounds
        for (j = 0; j < nrRounds ; j++){
            
            exp_board_enable_adc(expboard, id1);
            exp_board_enable_dac(expboard, id2);
            //start ur
            pulse_start();
            pulse_edge_detection_start();
            while (MAGI){       //tjek for om DMA ting har givet en hoej vaerdi

            }
            timerVar = MAGI; //save time/stop ur
            exp_board_disable_adc(expboard);
            exp_board_disable_dac(expboard);
            pulse_stop();
            singArray[j] = calcFreqQ(timerVar, Q);  //upstream freq

            //sender lyd tilbage
            exp_board_enable_adc(expboard, id2);
            exp_board_enable_dac(expboard, id1);
            //start ur
            pulse_start();
            pulse_edge_detection_start();
            while (MAGI){       //tjek for om DMA ting har givet en hoej vaerdi

            }
            timerVar = MAGI; //save time/stop ur
           

            exp_board_disable_adc(expboard);
            exp_board_disable_dac(expboard);
            pulse_stop();
            singArray[j] -= calcFreqQ(timerVar, Q); //subtracts downstream freq
        }

        for (j = 0; j < nrRounds ; j++){
            singResultHolder += (factor * (float) singArray[j]);
        }
        singResultHolder /= nrRounds;
        //Error handling here - increase nrRounds hvis to er
        resultArray[i] = (int16_t) singResultHolder;


    }
    resultHolder = averageSpeed(resultArray, antalMeas);
    free(singArray);
    free(resultArray);
    return resultHolder;
}

int16_t calcFreqQ(uint32_t time, uint16_t Q_outFormat){
    uint16_t shift = Q_outFormat + 16;
    int16_t output;
    if(time != 0){
        output = ((int32_t)1 << shift) / time;
        return ((int16_t) output >> 16);
    }
    else{
        //BAD
        return 0;
    }
}

int16_t averageSpeed(int16_t speedResults[], uint16_t nrOfMeasures){
    int32_t sum = 0; //evt make it 32 bit
    uint16_t i;
    for (i = 0 ; i < nrOfMeasures ; i++){
        sum += speedResults[i];
    }
    return (int16_t) (sum /= nrOfMeasures);
}




