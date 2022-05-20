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
    SA_round_result *round_results; // Array holding the "delta frequency" of each sing around round.
    float *resultArray;
    round_results = (SA_round_result*) malloc(nrRounds * sizeof(SA_round_result));
    if (!round_results){
        //fejlhaandtering
        return 0;
    }
    resultArray = (float*) malloc(antalMeas * sizeof(float));
    if (!resultArray){
        free(resultArray);
        //fejlhaandtering
        return 0;
    }



    const float factor = (SOUNDPATH_LENGTH/(2.0*ANGLE_LOOKUP));
    float round_avg_velocity = 0; //half scratch var
    float resultHolder = 0;

    int32_t i = 0;                      //counts measuring number
    for(i = 0; i < antalMeas ; i++){    //loop for hver maaling


        int16_t j = 0; //counts sing around rounds
        for (j = 0; j < nrRounds ; j++){
        	sing_one_round(sa_station, &round_results[j]);
        }


        float round_avg_delta_freq = 0.0;

        // Calculate average delta frequencies of all rounds.
        for (j = 0; j < nrRounds ; j++){
        	round_avg_delta_freq += round_results[j].delta_freq; // find the average of delta freq
        }

        // Calculate average water velocity along the propagation path
        round_avg_velocity = round_avg_delta_freq * factor / nrRounds;


        //Error handling here - increase nrRounds hvis to maalinger er meget forskellige
        //resultArray[i] = (int16_t) (singResultHolder*(qOmregner>>Q_DIF));


    }
    resultHolder = averageSpeed(resultArray, antalMeas);
    free(round_results);
    free(resultArray);
    return resultHolder;
}


void sing_one_way(SA_station_handle station, SA_direction dir, float *prop_time){

	// Choose which sensor is transmitter and receiver depending on whether we're sending up- or downstream.
    uint16_t sensor_transmitter = (dir == DOWNSTREAM) ? station->sensor_upstream : station->sensor_downstream;
    uint16_t sensor_receiver = (dir == DOWNSTREAM) ? station->sensor_downstream : station->sensor_upstream;

    // Enable receiver ADC and transmitter power amplifier
    exp_board_enable_adc(station->expBoard, sensor_receiver);
    exp_board_enable_dac(station->expBoard, sensor_transmitter);

    // Prompt the pulse generator to start sending a pulse
    *station->prompt_gen_start = true;
    *station->propagating = true;

    // Wait until a pulse edge has been detected on the receiver end
    while (*station->propagating == true){}

    reader_stop(&station->reader_handle);

    while ((CSL_DMA1_REGS->DMACH1TCR2 & CSL_DMA_DMACH1TCR2_EN_MASK)) {}

    // Read from the stopwatch the propagation time + system delay
    stopwatch_read_ns(station->watch, prop_time);    //saves time in timerVar

    // Disable receivers ADC and transmitters power amplifier
    exp_board_disable_dac(station->expBoard);
    exp_board_disable_adc(station->expBoard);
}

void sing_one_round(SA_station_handle station, SA_round_result * result) {
	sing_one_way(station, DOWNSTREAM, &result->prop_time_downstream);
	ezdsp5535_waitusec(1000); // Wait to let WaveForms catch up

	sing_one_way(station, UPSTREAM, &result->prop_time_upstream);
	ezdsp5535_waitusec(1000); // Wait to let WaveForms catch up


	result->delta_freq = 1.0 / (result->prop_time_downstream) - 1.0 / (result->prop_time_upstream);
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

float averageSpeed(float speedResults[], uint16_t nrOfMeasures){
    float sum = 0; //evt make it 32 bit
    uint16_t i;
    for (i = 0 ; i < nrOfMeasures ; i++){
        sum += speedResults[i];
    }
    return (int16_t) (sum / nrOfMeasures);
}




