/*
 *  ======= sing_around ========
 *  sing_around target-side implementation
 *
 *  Created on: 11. maj 2022
 *  Author:     nikol
 */
#include "Flowmeter/sing_around.h"

#define NR_ROUNDS 128; // Hardcoded in order to be able to make static arrays that are faster on the bus

//SA_round_result round_results[128];

int16_t singAround(SA_station_handle sa_station, uint16_t nrRounds,uint16_t antalMeas){

	//nrRounds = NR_ROUNDS; // Hardcode number of rounds

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
        	SA_round_result res;
        	SA_status status = sing_one_round(sa_station, &res);
        	round_results[j] = res;
        	if (status != SA_SUCCES) j--; // Retry if the sing around was not succesful
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


SA_status sing_one_way(SA_station_handle station, SA_direction dir, float *prop_time){

	// Choose which sensor is transmitter and receiver depending on whether we're sending up- or downstream.
    //uint16_t sensor_transmitter = 1; //(dir == DOWNSTREAM) ? station->sensor_upstream : station->sensor_downstream;
    //uint16_t sensor_receiver = 2; //(dir == DOWNSTREAM) ? station->sensor_downstream : station->sensor_upstream;
	uint16_t sensor_transmitter = 0;
	uint16_t sensor_receiver = 0;

	if (dir == DOWNSTREAM) {
		sensor_transmitter = station->sensor_upstream;
		sensor_receiver = station->sensor_downstream;
	}
	if (dir == UPSTREAM) {
		sensor_transmitter = station->sensor_downstream;
		sensor_receiver = station->sensor_upstream;
	}

    // Enable receiver ADC and transmitter power amplifier
    exp_board_enable_adc(station->expBoard, sensor_receiver);
    exp_board_enable_dac(station->expBoard, sensor_transmitter);

    // Prompt the pulse generator to start sending a pulse
    *station->propagating = true;
    *station->prompt_gen_start = true;

    // Wait until a pulse edge has been detected on the receiver end
    while (*station->propagating == true){}

    if (*station->timeout_flag == true) {
    	*station->timeout_flag = false;
    	return SA_TIMEOUT;
    }

    // Read from the stopwatch the propagation time + system delay
    stopwatch_read_ns(station->watch, prop_time);    //saves time in timerVar

    // Disable receivers ADC and transmitters power amplifier
    exp_board_disable_dac(station->expBoard);
    exp_board_disable_adc(station->expBoard);

    return SA_SUCCES;
}

SA_status sing_one_round(SA_station_handle station, SA_round_result * result) {
	SA_status status = SA_SUCCES;

	/* Results are first written to local variables on the stack, and later moved to the dynamically allocated result array.
	 * It appears that this is more time consistent. A hypothesis is that this is caused by bus congestion as the DMA is reading and writing alot on
	 * DARAM and maybe there is delay as well on dynamically allocated memory like the result array.
	 */
//	float a = 0;
//	float b = 0;

	status = sing_one_way(station, DOWNSTREAM, &result->prop_time_downstream);
	if (status != SA_SUCCES) return status;
	ezdsp5535_waitusec(1000); // Wait to let WaveForms catch up

	status = sing_one_way(station, UPSTREAM, &result->prop_time_upstream);
	if (status != SA_SUCCES) return status;
	ezdsp5535_waitusec(1000); // Wait to let WaveForms catch up

	//result->delta_freq = 1.0 / (result->prop_time_downstream) - 1.0 / (result->prop_time_upstream);

//	result->prop_time_downstream = a;
//	result->prop_time_upstream = b;

	return status;
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




