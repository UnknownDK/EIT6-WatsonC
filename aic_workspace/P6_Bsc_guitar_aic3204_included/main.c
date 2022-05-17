#include <main.h>

#include "stdio.h"
#include "math.h"
#include "stdint.h"
//#include "string.h" //smadrer support for huge memory model - memset virker ikke
#include <stdbool.h>
#include <stdlib.h>

#include "csl_intc.h"
#include <csl_gpt.h>
#include <csl_dma.h>
#include <csl_gpio.h>
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
#include <Flowmeter/expansion_board.h>

#define FREQ 40000      // Pulse sine frequency
#define S_RATE 96000    // Sample rate
#define SEQ_LEN 24      // Gives 10 periods at 40 kHz
#define READ_BUFFER_LEN 1000
#define EDGE_THRESHOLD 12000    // Threshold that correspond to approx. positive 366 mVp voltage

// Get the index of the buffer_read array that is currently being (or hast last been) written to
#define BUFFER_READ_CURR_INDEX INT32_ARRAY_INDEX_FROM_ADDR(DMA1CH1_WORD_DEST_ADDR - 1, buffer_read) // "-1" is because the DMA (assumably) has already changed destination to the next element, whenever we try to read the address

// Declare functions prototypes
void flowmeter_init();  // init board and codec
void GPIO_test_init();
void AIC3204_init();

interrupt void DMA_ISR(void);
void interrupt_init();
void generate_sine_table(int32_t *table, float freq, float s_rate,
							uint16_t samples);
void edge_detection_stop_callb(void);

// Interrupt vector table - This is required for correct interrupt setup
extern void VECSTART(void);

// Variables that are used externally
Uint16 fFilterOn = 0;
Uint16 fBypassOn = 1;
Uint16 clearOverlaps = 1;

stopwatch_handle tim_handle = { GPT_0 };

int32_t buffer_read[READ_BUFFER_LEN] = { 0 };
int16_t buffer_read_int16[READ_BUFFER_LEN] = { 0 };
int32_t sineTable[SEQ_LEN] = { 0 };

bool edgeBegun = false;
bool edge_detected = false;
uint16_t buffer_index_stop = 0; // Buffer array index at which capturing stopped
uint16_t buffer_index_edge = 0; // Buffer array index at which an edge was first detected

circular_dma_reader_config reader_config = { CSL_DMA_CHAN5, CSL_DMA_EVT_I2S2_RX,
												(int32_t*) 0x2A2C, buffer_read,
												READ_BUFFER_LEN };

circular_dma_reader_handle reader_handle = CIRCULAR_DMA_READER_HANDLER_RESET;


exp_board_obj exp_obj = { { CSL_GPIO_PIN8, CSL_GPIO_PIN3, CSL_GPIO_PIN0,
									CSL_GPIO_PIN7, CSL_GPIO_PIN9, CSL_GPIO_PIN2,
									CSL_GPIO_PIN1, CSL_GPIO_PIN6 } };

exp_board_handle exp_handle;

SA_station_obj singStationObj = {&tim_handle,
                                 &exp_obj, //hate this
                                 1,
                                 2,
                                 &edgeBegun};
SA_station_handle singStationHandle;





int main(void)
{
//    int test;
//    int32_t time = 357142;      //0.5 meter
//    int32_t time2 = 10714285*2; //30 meter
//
//
//    test = calcFreqQ(time,12);
//    test = calcFreqQ(time,15);
//    test = calcFreqQ(time,18);
//    test = calcFreqQ(time,21);
//    test = calcFreqQ(time,22);
//    test = calcFreqQ(time,26); //omkring sweetspot
//    test = calcFreqQ(time,31);
//
//    test = calcFreqQ(time2,22);
//    test = calcFreqQ(time2,26);
//    test = calcFreqQ(time2,31);
//    test = calcFreqQ(time2,33); // omkring sweetspot
//    test = calcFreqQ(time2,35);
//    test = calcFreqQ(time2,38);



	flowmeter_init();   // init board and codec

	edge_detected = false;
	reader_start(&reader_handle);

	//singAround(singStationHandle,2,1);
	pulse_start();

	volatile unsigned long tick = 0;

	while (1)
	{
		tick++;
	}
}

void flowmeter_init()
{

	/* The EBSR register must be set in order to enable I2S2 and desired GPIO pins.
	 * Before modifying the values in the EBSR register, you must clock gate all affected peripherals through the PCGCR register.
	 * The external bus selection register must be modified only once, during device configuration.
	 * Continuously changing the EBSR configuration is not supported.
	 * Setting PPMODE_MODE1 is required in order to enable I2S2 on the pins, and SP1MODE_MODE2 and SP0MODE_MODE2 is set in order to enable necessary GPIO.
	 */
	CSL_SYSCTRL_REGS->EBSR =
			(CSL_SYS_EBSR_PPMODE_MODE1 << CSL_SYS_EBSR_PPMODE_SHIFT)
			| (CSL_SYS_EBSR_SP1MODE_MODE2 << CSL_SYS_EBSR_SP1MODE_SHIFT)
			| (CSL_SYS_EBSR_SP0MODE_MODE2 << CSL_SYS_EBSR_SP0MODE_SHIFT);

	generate_sine_table(sineTable, FREQ, S_RATE, SEQ_LEN); // generate sine table for pulse generation. This is 10 periods of 40 kHz sine wave. 10 periods are necessary as one 40 kHz period at 96 ksps would only be 2.4 samples per period and the table can therefore not be repeated.
	//memset(buffer_read, 0, sizeof(buffer_read)); // clear read buffer

	// eZdsp - dev board
	ezdsp5535_init();
	exp_handle = &exp_obj;
	exp_board_init(exp_handle);
	singStationHandle = &singStationObj;


	pulse_generator_init(sineTable, SEQ_LEN);
	pulse_detector_init(&edge_detection_stop_callb);
	reader_init(&reader_handle, &reader_config);

	// Reset I2S receive register values
	CSL_I2S2_REGS->I2SRXRT0 = 0;
	CSL_I2S2_REGS->I2SRXRT1 = 0;

	stopwatch_configure(&tim_handle);





	// AIC3204 - audio codec
	aic3204_hardware_init();

	AIC3204_init();

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
void generate_sine_table(int32_t *table, float freq, float s_rate,
							uint16_t samples)
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
void cpy_int32_array_to_int16(int32_t *src, int16_t *dest, uint16_t len)
{
	uint16_t i;
	for (i = 0; i < len; i++)
	{
		dest[i] = INT32ARRAY_VAL_FROM_INDEX(src, i); // Macro for getting the 16 bit value from the upper 16 bits of the 32 bit element at index i in the src array.
	}
}

// Callback function for when edge detection is stopping
void edge_detection_stop_callb(void)
{
    edgeBegun = true;
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

interrupt void I2S2_receive_ISR(void)
{

	// Copy I2S value and DMA destination addresses before they change
	short reg = (short) CSL_I2S2_REGS->I2SRXRT1;
	uint16_t curr_index = (uint16_t) BUFFER_READ_CURR_INDEX; // current array index being written to

	// if an edge has been detected, count down until the end of the recording
	pulse_edge_detection_stop_counter();

	// Check if the current sample is an edge (if the absolute magnitude is above the threshold)
	if (!edge_detected && (reg > EDGE_THRESHOLD || reg < -EDGE_THRESHOLD))
	{
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
