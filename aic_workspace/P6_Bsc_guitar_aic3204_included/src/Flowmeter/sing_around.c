/*
 *  ======= sing_around ========
 *  sing_around target-side implementation
 *
 *  Created on: 11. maj 2022
 *  Author:     nikol
 */
#include "Flowmeter/sing_around.h"



int16_t singAround(SA_station_handle sa_station, uint16_t nrRounds,uint16_t antalMeas){
    exp_board_disable_adc(sa_station->expBoard); //disable
    exp_board_disable_dac(sa_station->expBoard);

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
    uint16_t qOmregner = (uint16_t)1 << Q;
    //(int16_t)(1 << Q))
    const float factor = (SOUNDPATH_LENGTH/(2*ANGLE_LOOKUP)) / qOmregner;
    float singResultHolder; //half scratch var
    int16_t resultHolder;
    uint32_t watchVar;


    int32_t i = 0;                      //counts measuring number
    for (;i<3000000;i++){}
    for(i = 0; i < antalMeas ; i++){    //loop for hver maaling
        //uint32_t test[256];

        int16_t j = 0;                  //counts sing arounds
        for (j = 0; j < nrRounds ; j++){
//            int k = 0;
//            for(;k<3;k++){}
            measureOneWay(sa_station, false, &watchVar);//, test,j);
            singArray[j] = calcFreqQ(watchVar, Q);  //upstream freq

            //sender lyd tilbage
            measureOneWay(sa_station, true, &watchVar);//,test,(j+nrRounds));
            singArray[j] -= calcFreqQ(watchVar, Q); //subtracts downstream freq
        }

        for (j = 0; j < nrRounds ; j++){
            singResultHolder += (factor * (float) singArray[j]);
        }
        singResultHolder /= nrRounds;
        //Error handling here - increase nrRounds hvis to maalinger er meget forskellige
        resultArray[i] = (int16_t) (singResultHolder*(qOmregner>>Q_DIF));


    }
    resultHolder = averageSpeed(resultArray, antalMeas);
    free(singArray);
    free(resultArray);
    return resultHolder;
}


void measureOneWay(SA_station_handle station, bool direction, uint32_t *timerVar){//,uint32_t in[],uint16_t count){ //til test
    uint16_t id1 = station->sensorID1;;
    uint16_t id2 = station->sensorID2;
    if (direction == true){//change dir
        id1 = station->sensorID2;
        id2 = station->sensorID1;
    }
    exp_board_enable_adc(station->expBoard, id1);
    exp_board_enable_dac(station->expBoard, id2);
    stopwatch_start(station->watch);
    pulse_start_periods(REPETITIONS);
    pulse_edge_detection_start();
    while (*station->edgeDetected == false){}       //tjek for om DMA ting har givet en hoej vaerdi
    stopwatch_read_ns(station->watch, timerVar);    //saves time in timerVar
    exp_board_disable_adc(station->expBoard);
    exp_board_disable_dac(station->expBoard);
    //in[count] = *timerVar; //til test
    *station->edgeDetected = false;
}

uint16_t calcFreqQ(uint32_t time, uint16_t Q_outFormat){
    uint16_t shift;
    uint16_t timeshift = 0;
    if (Q_outFormat > 15){
        timeshift = Q_outFormat - 15;
        shift = 31;
    }
    else{
        shift = Q_outFormat + 16;
    }
    int32_t output;
    time -= CALIB_FACTOR;
    time >>= timeshift;
    if(time != 0){
        output = ((uint32_t)1 << shift);
        output /= time;
        return ((uint16_t) (output >> 16));
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
    return (int16_t) (sum / nrOfMeasures);
}




