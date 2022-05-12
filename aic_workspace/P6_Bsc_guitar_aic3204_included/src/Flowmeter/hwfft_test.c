/*
 * hwfft_test.c
 *
 *  Created on: 11. maj 2022
 *      Author: tardi
 */
#include <Flowmeter/hwfft_test.h>


uint16_t fft_save_location;
//int32_t *fft_output_location;                         // Stores the memory location for ther output of the FFT


void run_fft(int32_t table[], int32_t data[], int32_t scratch[]){
    //int16_t fft_datapoints[1024*2];                       // Array containing imaginary and real parts of the time signal
    //int16_t real_freq[1024];                              // Array containing real parts of the FFT
    int16_t i = 0;
    for(i=0;i<8;i++){
        data[i] = table[i];
    }
    //data[0] = {0x00000000, 0x3FFF0000, 0x91270000, 0x7FFF0000, 0x91270000, 0x3FFF0000, 0x00000000, 0xC0010000};
//    int32_t *fft_output_location;                         // Stores the memory location for ther output of the FFT
    // const uint16_t fft_length = 16;                     // Lenght of the FFT
    //uint16_t fft_save_location;
    //int32_t data_br_buf[16];
    //int32_t scratch_buf[1024];                        //Array der indeholder bare reele og imaginære værdier, men indgangende har skiftet plads med bit reverse

    //                      ------------- CREATING INPUT ARRAY ------------
    //fft_create_datapoint_array(&signal_buffer[2048], fft_length, &fft_datapoints[0]); // Function for combining the real and negative parts of the samples into a single array

    //                      ------------- BIT REVERSING THE INPUTS -----------------------
    //hwafft_br(data, scratch, fft_length); // Function for bit reversing the samples to make them usable in a FFT
    int a;
    //                      ------------- PERFORMING FFT ---------------------------
//    fft_save_location = hwafft_8pts(data, scratch,0,1); // Function for calling the hardware FFT


}


