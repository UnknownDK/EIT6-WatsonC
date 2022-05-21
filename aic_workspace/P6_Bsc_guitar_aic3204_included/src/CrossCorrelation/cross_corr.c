/*
 * cross_corr.c
 *
 *  Created on: 18. maj 2022
 *      Author: tardi
 */



#define FREQ 40000      // Pulse sine frequency
#define S_RATE 96000    // Sample rate

#include <CrossCorrelation/cross_corr.h>

/*  Cross Correlation  */




#define M_PI 3.14159265358979323846


//void crosscorr_test();
short max_finder(short array[], short length);
void generate_compare_signal(short *table, float freq, float s_rate,
                            unsigned short samples);



short crosscorr(short inSignal[], short resultCorr[], short inSigLen, short outSigLen, short compSigLen)
{
    unsigned short offlag;
    //short resultCorr[RSLTCORRLEN]; // Result of crosscorrelation
    //short inSignal[OUTSIGLEN]; // Incoming signal of size OUTSIGLEN so the real parts of the output will fit back in inSignal

    int i = 0; // For loops
    short RSLTCORRLEN = (compSigLen+outSigLen-1);

    /* Clear resultCorr */
    for(i=0;i<RSLTCORRLEN;i++){
        resultCorr[i] = 0;
    }


    offlag = 0; // Mostly for testing, so it turns yellow if it is *STILL* overflowing after editing something.


    /* CROSSCORRELATION */
    /* Compare Signal */
    short* compareSignal; // Known signal
    compareSignal = (short*)malloc(compSigLen * sizeof(short));
    if (!compareSignal){
        return -999;
    }

    generate_compare_signal(compareSignal, FREQ, S_RATE*(outSigLen/inSigLen), compSigLen); // Generate sinetable for compareSignal


    //corr_bias  (DATA *x, DATA *y, DATA *r, ushort nx, ushort ny);
    offlag = corr_unbias(compareSignal, inSignal, resultCorr, compSigLen, outSigLen);
    short corrIndex = max_finder(resultCorr, RSLTCORRLEN);
    //float timeLag = ((float)(corrIndex +1)-compSigLen)/(S_RATE*(outSigLen/inSigLen)); // Magi

    return corrIndex;
}
//766

void fdzp(short shortArray[],long fdzpArray[], short length, short outLen){
    //long  fdzpArray[INSIGLEN*4*INTERF]; // *4 is enough, but makin space for fdzp
    //short size =
    if(outLen % length){
        return;
    }
    short interF = outLen/length;
    int i = 0;
    for(i=0;i<length*2;i++){ // Adding complex 0 values
        //fdzpArray[i] = i % 2 ? 0x00 : shortArray[(int)(i/2)];
        fdzpArray[i] = i % 2 ? 0x00000000 : shortArray[i/2];
    }
    for(i=length*2;i<outLen*4;i++){ // MEMSET 0
        //fdzpArray[i] = i % 2 ? 0x00 : shortArray[(int)(i/2)];
        fdzpArray[i] = 0x00000000;
    }
    //0xFE79 == 0xFFFFFE79
    cfft32_NOSCALE(fdzpArray, length);
    cbrev32(fdzpArray, fdzpArray, length);
    int iStart = (outLen*2)-1;
    int iSmallerThan = (outLen*2)-(length)-1;
    for(i=iStart;i>iSmallerThan;i--){
        int swap = i-((2*interF-2)*length);
        fdzpArray[i] = fdzpArray[swap];
        fdzpArray[swap] = 0x00000000;
    }

    cifft32_SCALE(fdzpArray, outLen);
    cbrev32(fdzpArray, fdzpArray, outLen);
}

short max_finder(short array[], short length){
    short i = 0;
    short largestYet = abs(array[0]);
    short index = 0;
    for(i=0;i<length;i++){
        if(largestYet < abs(array[i])){
            largestYet = abs(array[i]);
            index = i;
        }
    }
    return index;
}

void generate_compare_signal(short *table, float freq, float s_rate,
                            unsigned short samples)
{
    unsigned short i = 0;
    float inc = 2 * M_PI * (freq / s_rate);
    for (; i < samples; i++)
    {
        //table[i] = (((int32_t) (0x30 * sin(i * inc))) & 0xFFFF) << 16;
        table[i] = (((short) (0x330F * sin(i * inc))));
    }
}
