#include "stdio.h"
#include "ezdsp5535.h"  // board header
#include "csl_intc.h"
#include "ezdsp5535_gpio.h"
#include "register_system.h"
//#include "rtc_routines.h"
//#include "data_types.h"
//#include "register_cpu.h"
#include "pll.h"
#include "math.h"

// Audio codec setup
#include "aic3204.h"        // codec header
Uint16 sample_rate = 48000; // sample rate
Int16 gain_db = 0;         // gain on input (-1 er korrekt)
Int16 r, l;               // store read-values
#define PLL_12M     0
#define PLL_98M     0
#define PLL_100M    1


void InitSystem(void);
void ConfigPort(void);

void PLL_98MHz(void);
//

Uint16 fFilterOn = 0;
Uint16 fBypassOn = 1;
Uint16 clearOverlaps = 1;


// Declare functions
void pedal_init(Uint16 SAMPLE_RATE, Int16 GAIN_DB);    // init board and codec
void GPIO_test_init();
void overdrive(Int16 *left, Int16 *right);
    Int16 gain_overdrive, th_soft_overdrive, th_hard_overdrive;
void do_sample_and_gain();
//

#define seqLen 96

float magnitude = 3000;
unsigned short midpoint = 4000;
unsigned short sineTable[seqLen] = {0};
unsigned short tableIndex = 0;

////////////////// main.c ///////////////////////////////
int main(void)
 {
    int i = 0;
    for (; i < seqLen; i++) {
        sineTable[i] = midpoint + magnitude * sin(2 * 3.1415 * i / seqLen);
    }

    // setup
    pedal_init(sample_rate, gain_db);   // init board and codec
    // GPIO_test_init();

    // main loop
    while(1)
    {
        //ezdsp5535_GPIO_setOutput(GPIO13, 1); // test high pin 4

        aic3204_codec_read(&l, &r);
        //printf("Venstre: %u \n", l);
        //printf("H�jre: %u \n", r);
        aic3204_codec_write(l, r);
        //aic3204_codec_write(sineTable[tableIndex], sineTable[tableIndex]);
        if (++tableIndex == seqLen) tableIndex = 0;
        //if (sw == 0) sw = 0xFFFF;
        //else sw = 0;

        //ezdsp5535_GPIO_setOutput(GPIO13, 0); // test low pin 4
    }
}


///////////////////// Functions ///////////////////////////
void pedal_init(Uint16 SAMPLE_RATE, Int16 GAIN_DB)
{
    // eZdsp - dev board
    ezdsp5535_init();

    // AIC3204 - audio codec
    aic3204_hardware_init();
    //aic3204_init();
    //set_sampling_frequency_and_gain(SAMPLE_RATE, GAIN_DB);  // Sets samplerate and gain on input
    do_sample_and_gain();
}

void GPIO_test_init()
{
    ezdsp5535_GPIO_init();
    ezdsp5535_GPIO_setDirection(GPIO13, GPIO_OUT);
}

void overdrive(Int16 *left, Int16 *right)
{
    if( abs(*left) > th_soft_overdrive){ // soft-clip
        if(*left > 0){

        }
        if(*left <= 0){

        }
    }
    if( abs(*left) > th_hard_overdrive){ // hard-clip

    }
    else;
}

void PLL_98MHz(void)
{
    // PLL set up from RTC
    // bypass PLL
    CONFIG_MSW = 0x0;

#if (PLL_100M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0000;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82FA;

#elif (PLL_12M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0200;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82ED;
#elif (PLL_98M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0000;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82ED;

#endif
    while ( (PLL_CNTL3 & 0x0008) == 0);
    // Switch to PLL clk
    CONFIG_MSW = 0x1;
}

void InitSystem(void)
{
    Uint16 i;
    // PLL set up from RTC
    // bypass PLL
    CONFIG_MSW = 0x0;

#if (PLL_100M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0000;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82FA;

#elif (PLL_12M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0200;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82ED;
#elif (PLL_98M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0000;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82ED;

#elif (PLL_40M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0300;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x8262;
#endif

    while ((PLL_CNTL3 & 0x0008) == 0);
    // Switch to PLL clk
    CONFIG_MSW = 0x1;

    // clock gating
    // enable all clocks
    IDLE_PCGCR = 0x0;
    IDLE_PCGCR_MSW = 0xFF84;


    // reset peripherals
    PER_RSTCOUNT = 0x02;
    PER_RESET = 0x00fb;
    for (i=0; i< 0xFFF; i++);
}