/*
 * hwfft_test.h
 *
 *  Created on: 12. maj 2022
 *      Author: tardi
 */

#ifndef INCLUDE_FLOWMETER_HWFFT_TEST_H_
#define INCLUDE_FLOWMETER_HWFFT_TEST_H_

#include "stdio.h"
#include "stdint.h"



void hwafft_br(
    int32_t *data,
    int32_t *data_br,
    uint16_t data_len
);

//uint16_t hwafft_8pts(
//    int32_t *data,
//    int32_t *scratch,
//    uint16_t fft_flag,
//    uint16_t scale_flag
//);

void run_fft(int32_t *table, int32_t data[], int32_t scratch[]);

#endif /* INCLUDE_FLOWMETER_HWFFT_TEST_H_ */
