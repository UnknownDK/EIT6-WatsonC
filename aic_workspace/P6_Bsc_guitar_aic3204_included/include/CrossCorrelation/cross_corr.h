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

float crosscorr(short inSignal[], short resultCorr[], short INSIGLEN, short OUTSIGLEN, short COMPSIGLEN);


#endif /* INCLUDE_CROSSCORRELATION_CROSS_CORR_H_ */
