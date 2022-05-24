/*
 *  ======= sing_around ========
 *  sing_around target-side implementation
 *
 *  Created on: 11. maj 2022
 *  Author:     nikol
 */
#include "Flowmeter/sing_around.h"

#define NR_ROUNDS 128; // Hardcoded in order to be able to make static arrays that are faster on the bus
#define SYSTEM_DELAY 357937.282 // ns

SA_round_result round_results[NR_ROUNDS];

const float velocity_factor = (SOUNDPATH_LENGTH/(2.0*ANGLE_LOOKUP));

int16_t singAround(SA_station_handle sa_station, uint16_t nrRounds,uint16_t nrMeas){

	// Disable ADC inputs and piezo power amplifiers
    exp_board_disable_adc(sa_station->expBoard);
    exp_board_disable_dac(sa_station->expBoard);

    // Rough memory check
    if((nrMeas+nrRounds) > MAX_MALLOC){
        return 0;
    }
  
    //memory allocation
    float *resultArray = (float*) malloc(nrMeas * sizeof(float));
    if (!resultArray){
        free(resultArray);
        // Error handling
        return 0;
    }

    float resultHolder = 0;

    // Iterate over a number of "Measurements", each consisting of a number of sing around rounds
    int32_t i = 0;                      //counts measuring number
    for(i = 0; i < nrMeas; i++){    //loop for hver maaling

    	// Perform a number of sing around rounds
        int16_t j = 0;
        for (j = 0; j < nrRounds ; j++){
        	SA_round_result res;
        	SA_status status = sing_one_round(sa_station, &res);
        	round_results[j] = res;
        	if (status != SA_SUCCES) j--; // Retry if the sing around was not successful
        }

        float round_avg_delta_freq = 0.0;

        // Calculate average delta frequencies of all rounds.
        for (j = 0; j < nrRounds ; j++){
        	round_avg_delta_freq += round_results[j].delta_freq; // find the average of delta freq
        }

        // Calculate average water velocity along the propagation path
        resultArray[i] = round_avg_delta_freq * velocity_factor / nrRounds;
    }
    resultHolder = averageSpeed(resultArray, nrMeas);
    free(resultArray);
    return resultHolder;
}


SA_status sing_one_way(SA_station_handle station, SA_pulse_result * result){

	// Choose which sensor is transmitter and receiver depending on whether we're sending up- or downstream.
	uint16_t sensor_transmitter = 0;
	uint16_t sensor_receiver = 0;

	if (result->direction == DOWNSTREAM) {
		sensor_transmitter = station->sensor_upstream;
		sensor_receiver = station->sensor_downstream;
	}
	if (result->direction == UPSTREAM) {
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

    // Disable receivers ADC and transmitters power amplifier
	exp_board_disable_dac(station->expBoard);
	exp_board_disable_adc(station->expBoard);

    if (*station->timeout_flag == true) {
    	*station->timeout_flag = false;
    	return SA_TIMEOUT;
    }

    // Read from the stopwatch the propagation time + system delay
    stopwatch_read_ns(station->watch, &result->edge_prop_time);    //saves time in timerVar

    result->edge_index = *station->edge_index;
    result->end_index = *station->end_index;



    return SA_SUCCES;
}

SA_status sing_one_round(SA_station_handle station, SA_round_result * result) {

	SA_status status = SA_SUCCES;

	/* Results are first written to local variables on the stack, and later moved to the dynamically allocated result array.
	 * It appears that this is more time consistent. A hypothesis is that this is caused by bus congestion as the DMA is reading and writing alot on
	 * DARAM and maybe there is delay as well on dynamically allocated memory like the result array.
	 */

	SA_pulse_result pulse_down = { DOWNSTREAM };
	SA_pulse_result pulse_up = { UPSTREAM };


	// Measure downstream transit time
	status = sing_one_way(station, &pulse_down);
	if (status != SA_SUCCES) return status;
	//ezdsp5535_waitusec(2000); // Wait to let WaveForms catch up

	// Refine downstream transit time using FDZP and cross correlation
	float refined_time_down = 0;
	refine_pulse_time(pulse_down.edge_index, pulse_down.end_index, pulse_down.edge_prop_time ,&refined_time_down);

	// Measure upstream transit time
	status = sing_one_way(station, &pulse_up);
	if (status != SA_SUCCES) return status;
	//ezdsp5535_waitusec(2000); // Wait to let WaveForms catch up

	// Refine upstream transit time using FDZP and cross correlation
	float refined_time_up = 0;
    refine_pulse_time(pulse_up.edge_index, pulse_up.end_index, pulse_up.edge_prop_time ,&refined_time_up);

    result->prop_time_downstream = refined_time_down;
    result->prop_time_upstream = refined_time_up;

    // Calculate delta frequency
	result->delta_freq = 1.0 / (result->prop_time_downstream - SYSTEM_DELAY) - 1.0 / (result->prop_time_upstream - SYSTEM_DELAY);

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
    float sum = 0;
    uint16_t i;
    for (i = 0 ; i < nrOfMeasures ; i++){
        sum += speedResults[i];
    }
    return (sum / nrOfMeasures);
}




