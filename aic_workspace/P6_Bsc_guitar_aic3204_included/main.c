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
#include "stdint.h"
#include "aic3204.h"        // codec header
#include <pulse_generator.h>
#include "circular_dma_reader.h"
#include "string.h"

#define M_PI 3.14159265358979323846

#define FREQ 4000
#define SEQ_LEN (96000 / FREQ)
#define READ_BUFFER_LEN 1000

// Declare functions
void flowmeter_init();  // init board and codec
void GPIO_test_init();
void do_sample_and_gain();

interrupt void DMA_ISR(void);
void interrupt_init();
void generate_sine_table(int32_t *table, uint16_t samples);

// Interrupt vector table
extern void VECSTART(void);

// Variables that are used externally
Uint16 fFilterOn = 0;
Uint16 fBypassOn = 1;
Uint16 clearOverlaps = 1;
int32_t sineTable[SEQ_LEN] = { 0 };
uint16_t tableIndex = 0;


int32_t buffer_read[READ_BUFFER_LEN] = {0};

circular_dma_reader_config reader_config = {
                                            CSL_DMA_CHAN5,
                                            CSL_DMA_EVT_I2S2_RX,
                                            (int32_t *) 0x2A2C,
                                            buffer_read,
                                            READ_BUFFER_LEN
};

circular_dma_reader_handle reader_handle = CIRCULAR_DMA_READER_HANDLER_RESET;


#include <stdlib.h>
#include <math.h>
#include <tms320.h>
#include <dsplib.h>
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    generate_sine_table(sineTable, SEQ_LEN); // generate sine table for pulse generation
    memset(buffer_read, 0, sizeof(buffer_read)); // clear read buffer

    flowmeter_init();   // init board and codec

    //pulse_start_periods(5);
    pulse_start();

    reader_start(&reader_handle);

    volatile unsigned long tick = 0;

    // main loop
//    while (tick < 10000000)
//    {
//        tick++;
//    }
//
//    reader_stop(&reader_handle);

    while (1) {
        tick++;
    }
}

void flowmeter_init()
{
    interrupt_init();

    // eZdsp - dev board
    ezdsp5535_init();

    pulse_generator_init((int32_t*) sineTable, SEQ_LEN);
    reader_init(&reader_handle, &reader_config);

    // AIC3204 - audio codec
    aic3204_hardware_init();

    do_sample_and_gain();
}

void generate_sine_table(int32_t *table, uint16_t samples)
{
    uint16_t i = 0;
    float inc = 2 * M_PI / samples;
    for (; i < samples; i++)
    {
        table[i] = (((int32_t) (0x7fff * sin(i * inc))) & 0xFFFF) << 16;
    }
}

interrupt void DMA_ISR(void)
{
    // Disable interrupt flag so the interrupt can be called again later
    uint16_t ifr = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR = ifr;

    // Check for particular interrupt flag bit and respond to it
    if (pulse_check_interrupt_flag(ifr))
        pulse_period_finished_callb();
}

void interrupt_init()
{
    IRQ_globalDisable();
    IRQ_clearAll();
    IRQ_setVecs((uint32_t) (&VECSTART));

    IRQ_plug(DMA_EVENT, &DMA_ISR);
    IRQ_enable(DMA_EVENT);

    IRQ_globalEnable();
}
