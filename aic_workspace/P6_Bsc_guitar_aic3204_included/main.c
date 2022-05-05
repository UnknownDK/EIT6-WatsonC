#include "stdio.h"
#include "math.h"
#include "stdint.h"
#include "string.h"

#include "csl_intc.h"
#include <csl_gpt.h>
#include "ezdsp5535.h"  // board header
#include "ezdsp5535_gpio.h"
#include "register_system.h"
#include "pll.h"
#include "aic3204.h"        // codec header
#include <Flowmeter/circular_dma_reader.h>
#include <Flowmeter/pulse_detector.h>
#include <Flowmeter/pulse_generator.h>
#include <Flowmeter/stopwatch.h>

#define M_PI 3.14159265358979323846

#define FREQ 40000
#define S_RATE 96000
#define SEQ_LEN 24 // Gives 10 periods at 40 kHz
#define READ_BUFFER_LEN 1000

// Declare functions
void flowmeter_init();  // init board and codec
void GPIO_test_init();
void do_sample_and_gain();

interrupt void DMA_ISR(void);
void interrupt_init();
void generate_sine_table(int32_t *table, float freq, float s_rate, uint16_t samples);

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

stopwatch_handle tim_handle = { GPT_0 };


int main(void)
{
    generate_sine_table(sineTable, FREQ, S_RATE, SEQ_LEN); // generate sine table for pulse generation
    memset(buffer_read, 0, sizeof(buffer_read)); // clear read buffer

    flowmeter_init();   // init board and codec

    //pulse_start_periods(5);
    pulse_start();
    reader_start(&reader_handle);

    volatile unsigned long tick = 0;

    // main loop

    stopwatch_start(&tim_handle);

    while (tick < 3000000)
    {
        tick++;
    }

    stopwatch_stop(&tim_handle);
    uint32_t ns = 0;
    stopwatch_read_ns(&tim_handle, &ns);

    pulse_edge_detection_start();

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

    stopwatch_configure(&tim_handle);

    // AIC3204 - audio codec
    aic3204_hardware_init();

    do_sample_and_gain();
}

void generate_sine_table(int32_t *table, float freq, float s_rate, uint16_t samples)
{
    uint16_t i = 0;
    float inc = 2 * M_PI * (freq / s_rate);
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
    if (pulse_check_interrupt_flag(ifr)) // DMA (I2S2 transmit) transfer complete interrupt
        pulse_period_finished_callb();
}

interrupt void I2S2_receive_ISR(void) {
    if (abs(CSL_I2S2_REGS->I2SRXRT1) > 4500) // above approx. 100 mVp amplitude
        pulse_edge_detection_stop();
}

void interrupt_init()
{
    IRQ_globalDisable();
    //IRQ_disableAll();
    IRQ_clearAll();
    IRQ_setVecs((uint32_t) (&VECSTART));

    IRQ_plug(DMA_EVENT, &DMA_ISR);
    IRQ_enable(DMA_EVENT);

    IRQ_plug(RCV2_EVENT, &I2S2_receive_ISR); // I2S2 receive interrupt
    IRQ_enable(RCV2_EVENT); // Enable I2S2 receive interrupt

    IRQ_globalEnable();
}
