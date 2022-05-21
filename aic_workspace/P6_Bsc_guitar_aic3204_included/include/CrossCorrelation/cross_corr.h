/*
 * cross_corr.h
 *
 *  Created on: 18. maj 2022
 *      Author: tardi
 */

#ifndef INCLUDE_CROSSCORRELATION_CROSS_CORR_H_
#define INCLUDE_CROSSCORRELATION_CROSS_CORR_H_

#include <Dsplib.h>
#include <math.h>
//#include <stdio.h>
#include <stdlib.h>

short crosscorr(short inSignal[], short resultCorr[], short INSIGLEN, short OUTSIGLEN, short COMPSIGLEN);

void fdzp(short shortArray[],long fdzpArray[], short length, short outLen);

#endif /* INCLUDE_CROSSCORRELATION_CROSS_CORR_H_ */
