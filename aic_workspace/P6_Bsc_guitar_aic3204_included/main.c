#include "stdio.h"
#include "math.h"
#include "stdint.h"
#include "string.h"
#include <stdbool.h>
#include <stdlib.h>

#include "csl_intc.h"
#include <csl_gpt.h>
#include <csl_dma.h>
#include "ezdsp5535.h"  // board header
#include "ezdsp5535_gpio.h"
#include "register_system.h"
#include "pll.h"
#include "aic3204.h"        // codec header
#include <Flowmeter/circular_dma_reader.h>
#include <Flowmeter/pulse_detector.h>
#include <Flowmeter/pulse_generator.h>
#include <Flowmeter/stopwatch.h>
#include <Flowmeter/sing_around.h>

#define M_PI 3.14159265358979323846

#define FREQ 40000      // Pulse sine frequency
#define S_RATE 96000    // Sample rate
#define SEQ_LEN 24      // Gives 10 periods at 40 kHz
#define READ_BUFFER_LEN 1000
#define EDGE_THRESHOLD 12000    // Threshold that correspond to approx. positive 366 mVp voltage

// The following macros are used for extracting the 16 bit data stored in int32_t type arrays.
// The arrays must be 32 bit in order to use DMA.
#define INT32ARRAY_ADDRESS_FROM_INDEX(arr, index) (((int16_t *) arr) + index*2)
#define INT32ARRAY_VAL_FROM_INDEX(arr, index) (*INT32ARRAY_ADDRESS_FROM_INDEX(arr, index))

// Macros for converting from byte address space (which the DMA works in) to word address space (which the CPU works in)
#define WORD_TO_BYTE_DARAM_ADDR_SPACE(addr) ((((uint32_t) addr) << CSL_DMA_ADDR_SHIFT) + CSL_DMA_DARAM_ADDR_OFFSET)
#define WORD_TO_BYTE_SARAM_ADDR_SPACE(addr) ((((uint32_t) addr) << CSL_DMA_ADDR_SHIFT) + CSL_DMA_SARAM_ADDR_OFFSET)
#define BYTE_TO_WORD_DARAM_ADDR_SPACE(addr) ((((uint32_t) addr) - CSL_DMA_DARAM_ADDR_OFFSET) >> CSL_DMA_ADDR_SHIFT)
#define BYTE_TO_WORD_SARAM_ADDR_SPACE(addr) ((((uint32_t) addr) - CSL_DMA_SARAM_ADDR_OFFSET) >> CSL_DMA_ADDR_SHIFT)
#define CSL_DARAM_START_BYTE_ADDR           WORD_TO_BYTE_DARAM_ADDR_SPACE((uint32_t) CSL_DMA_DARAM_START_ADDR) // Byte: 0x100C0
#define CSL_DARAM_END_BYTE_ADDR             WORD_TO_BYTE_DARAM_ADDR_SPACE((uint32_t) CSL_DMA_DARAM_END_ADDR) // Word: 0xFFFF
#define CSL_SARAM_START_BYTE_ADDR           WORD_TO_BYTE_SARAM_ADDR_SPACE((uint32_t) CSL_DMA_SARAM_START_ADDR) // Byte: 0x90000
#define CSL_SARAM_END_BYTE_ADDR             WORD_TO_BYTE_SARAM_ADDR_SPACE((uint32_t) CSL_DMA_SARAM_END_ADDR) // Word: 0x3FFFF
#define BYTE_TO_WORD_ADDR_SPACE(addr)       ((((uint32_t) addr) >= CSL_DARAM_START_BYTE_ADDR && ((uint32_t) addr) <= CSL_DARAM_END_BYTE_ADDR) ? (BYTE_TO_WORD_DARAM_ADDR_SPACE(addr)) : ((((uint32_t) addr) >= CSL_SARAM_START_BYTE_ADDR && ((uint32_t) addr) <= CSL_SARAM_END_BYTE_ADDR) ? (BYTE_TO_WORD_SARAM_ADDR_SPACE(addr)) : ((uint32_t) addr)))

// DMA1 Channel 1 Current Destination Address (word addressing space)
#define DMA1CH1_WORD_DEST_ADDR BYTE_TO_WORD_ADDR_SPACE((CSL_DMA1_REGS->DMACH1DSAL | ((uint32_t) CSL_DMA1_REGS->DMACH1DSAU << 16)))

// Get element index in a 32 bit array using the address of that element
#define INT32_ARRAY_INDEX_FROM_ADDR(addr, arr) ((((uint32_t) addr) - ((uint32_t) arr)) >> 1) // Subtract address by array address, bit shift to divide by two, as each element is 2 (16 bit) words

// Get the index of the buffer_read array that is currently being (or hast last been) written to
#define BUFFER_READ_CURR_INDEX INT32_ARRAY_INDEX_FROM_ADDR(DMA1CH1_WORD_DEST_ADDR - 1, buffer_read) // "-1" is because the DMA (assumably) has already changed destination to the next element, whenever we try to read the address

// Declare functions prototypes
void flowmeter_init();  // init board and codec
void GPIO_test_init();
void do_sample_and_gain();

interrupt void DMA_ISR(void);
void interrupt_init();
void generate_sine_table(int32_t *table, float freq, float s_rate, uint16_t samples);
void edge_detection_stop_callb(void);

// Interrupt vector table - This is required for correct interrupt setup
extern void VECSTART(void);

// Variables that are used externally
Uint16 fFilterOn = 0;
Uint16 fBypassOn = 1;
Uint16 clearOverlaps = 1;

stopwatch_handle tim_handle = { GPT_0 };

int32_t buffer_read[READ_BUFFER_LEN] = { 0 };
int16_t buffer_read_int16[READ_BUFFER_LEN] = {0};
int32_t sineTable[SEQ_LEN] = { 0 };

bool edge_detected = false;
uint16_t buffer_index_stop = 0; // Buffer array index at which capturing stopped
uint16_t buffer_index_edge = 0; // Buffer array index at which an edge was first detected

circular_dma_reader_config reader_config = {
                                            CSL_DMA_CHAN5,
                                            CSL_DMA_EVT_I2S2_RX,
                                            (int32_t *) 0x2A2C,
                                            buffer_read,
                                            READ_BUFFER_LEN
};

circular_dma_reader_handle reader_handle = CIRCULAR_DMA_READER_HANDLER_RESET;

int main(void)
{

    flowmeter_init();   // init board and codec

    edge_detected = false;
    pulse_edge_detection_start();
    reader_start(&reader_handle);

    //pulse_start_periods(1);
    pulse_start();

    volatile unsigned long tick = 0;

    // main loop

//    stopwatch_start(&tim_handle);
//
//    while (tick < 100000000)
//    {
//        tick++;
//    }
//
//    stopwatch_stop(&tim_handle);
//    uint32_t ns = 0;
//    stopwatch_read_ns(&tim_handle, &ns);


    while (1) {
        tick++;
    }
}

void flowmeter_init()
{
    generate_sine_table(sineTable, FREQ, S_RATE, SEQ_LEN); // generate sine table for pulse generation
    memset(buffer_read, 0, sizeof(buffer_read)); // clear read buffer

    // eZdsp - dev board
    ezdsp5535_init();

    pulse_generator_init(sineTable, SEQ_LEN);
    pulse_detector_init(&edge_detection_stop_callb);
    reader_init(&reader_handle, &reader_config);

    // Reset I2S receive register values
    CSL_I2S2_REGS->I2SRXRT0 = 0;
    CSL_I2S2_REGS->I2SRXRT1 = 0;

    stopwatch_configure(&tim_handle);

    // AIC3204 - audio codec
    aic3204_hardware_init();

    do_sample_and_gain();

    interrupt_init();
}

/**
 * Generates samples for a sinusoidal wave at a specified frequency and sample rate.
 * It might be necessary to generate samples for more than one sinusoidal period,
 * in order to get a clean continuous wave when repeating the sequence.
 * @param table Array for resulting samples.
 * @param freq Sinuoidal wave frequency.
 * @param s_rate Sample rate.
 * @param samples The number of samples to generate.
 */
void generate_sine_table(int32_t *table, float freq, float s_rate, uint16_t samples)
{
    uint16_t i = 0;
    float inc = 2 * M_PI * (freq / s_rate);
    for (; i < samples; i++)
    {
        table[i] = (((int32_t) (0x7fff * sin(i * inc))) & 0xFFFF) << 16;
    }
}

// This function copies the effective data (upper 16 bits) from a 32 bit array into a 16 bit array.
// The array is 32 bit as this is required by DMA.
void cpy_int32_array_to_int16(int32_t * src, int16_t * dest, uint16_t len) {
    uint16_t i;
    for (i = 0; i < len; i++) {
        dest[i] = INT32ARRAY_VAL_FROM_INDEX(src, i); // Macro for getting the 16 bit value from the upper 16 bits of the 32 bit element at index i in the src array.
    }
}

// Callback function for when edge detection is stopping
void edge_detection_stop_callb(void) {
    buffer_index_stop = (uint16_t) BUFFER_READ_CURR_INDEX; // Capture current buffer array index before DMA registers change
    reader_stop(&reader_handle); // Stop capturing
    cpy_int32_array_to_int16(buffer_read, buffer_read_int16, READ_BUFFER_LEN); // Copy upper 16 bits from the 32 bit dma buffer array into a 16 bit array that can be read easier
    edge_detected = false; // Reset edge detection
}

interrupt void DMA_ISR(void)
{
    // Disable interrupt flag so the interrupt can be called again later
    uint16_t ifr = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR = ifr;

    // Check for particular interrupt flag bit and respond to it
    if (pulse_check_interrupt_flag(ifr)) // DMA (I2S2 transmit) transfer complete interrupt
        pulse_repetition_ended_callb();
}

interrupt void I2S2_receive_ISR(void) {

    // Copy I2S value and DMA destination addresses before they change
    short reg = (short) CSL_I2S2_REGS->I2SRXRT1;
    uint16_t curr_index = (uint16_t) BUFFER_READ_CURR_INDEX; // current array index being written to

    // if an edge has been detected, count down until the end of the recording
    pulse_edge_detection_stop_counter();

    // Check if the current sample is an edge (if the absolute magnitude is above the threshold)
    if (!edge_detected && (reg > EDGE_THRESHOLD || reg < -EDGE_THRESHOLD)) {
        buffer_index_edge = curr_index; // Remember this index as the first edge detected

        edge_detected = true;

        pulse_edge_detection_stop_in_n(SEQ_LEN + 10); // Capture another SEQ_LEN + 10 samples before stopping capturing
    }
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
