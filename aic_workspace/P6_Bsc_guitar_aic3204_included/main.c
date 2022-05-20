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
#define EDGE_THRESHOLD ((int16_t) (46347 * 0.3))    // Threshold that correspond to approx. positive 0.3 Vp voltage

// Get the index of the buffer_read array that is currently being (or hast last been) written to
#define BUFFER_READ_CURR_INDEX INT32_ARRAY_INDEX_FROM_ADDR(DMA1CH1_WORD_DEST_ADDR - 1, buffer_read) // "-1" is because the DMA (assumably) has already changed destination to the next element, whenever we try to read the address

// Declare functions prototypes
void flowmeter_init();  // init board and codec
void GPIO_test_init();
void AIC3204_init();

interrupt void DMA_ISR(void);
void interrupt_init();
void generate_sine_table(int32_t *table, float scale, float freq, float s_rate,
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

bool propagating = false;
bool edge_detected = false;
bool prompt_gen_start = false;
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
                                 &propagating,
								 &prompt_gen_start
};
SA_station_handle singStationHandle;



void delay(){
	uint32_t hejsa = 0;

		while (hejsa++ < 1000000) {

		}
}

int main(void)
 {
	flowmeter_init();   // init board and codec
	uint32_t i = 1;
	uint32_t index = 0;
	for (; 1; i++)	 {
		reader_start(&reader_handle);

		delay();

		reader_stop(&reader_handle);

		index = (uint32_t) BUFFER_READ_CURR_INDEX;

		// Clear
		uint16_t j = 0;
		for (j = 0; j < READ_BUFFER_LEN; j++) {
			buffer_read[j] = 0;
		}

		//CSL_DMA1_REGS->DMACH1DSAL = 0x30D4 + 256 * i;
	}




	volatile unsigned long tick = 0;

	while (1)
	{
		tick++;
	}

	reader_start(&reader_handle);

	delay();

	reader_stop(&reader_handle);

	delay();

	//DMA_init();

	// Setup DMA configuration struct
//	reader_handle.dmaChNum = reader_config.chan;
//	reader_handle.dmaHandle = &reader_handle.dmaChObj;
//	reader_handle.dmaConfig.dmaEvt = reader_config.evtType;
//	reader_handle.dmaConfig.srcAddr = (uint32_t) reader_config.src_addr;
//	reader_handle.dmaConfig.destAddr = (uint32_t) reader_config.dest_addr;
//	reader_handle.dmaConfig.dataLen = 256;//config->buffer_len * 4; // DMA works in "byte-addressing space" and does not use CPU 16-bit addressing intervals. A 32 bit array element is therefore counted as 4 * 8 bits.
//
//	// Check for invalid parameters
//	if (reader_handle.dmaChNum == CSL_DMA_CHAN_INV
//			|| reader_handle.dmaConfig.dmaEvt == CSL_DMA_EVT_INVALID
//			|| reader_handle.dmaConfig.srcAddr == 0
//			|| reader_handle.dmaConfig.destAddr == 0
//			|| reader_handle.dmaConfig.dataLen == 0) return CSL_ESYS_INVPARAMS;
//
//	CSL_Status status = CSL_SOK;
//
//	reader_handle.dmaHandle = DMA_open(reader_handle.dmaChNum, reader_handle.dmaHandle, &status);
//
//	status = ;
	//DMA_config(reader_handle.dmaHandle, &reader_handle.dmaConfig);

	CSL_DMA1_REGS->DMACH1DSAU = 0x30D4;

	reader_start(&reader_handle);

	edge_detected = false;
	//reader_start(&reader_handle);
	//pulse_start();
	//singAround(singStationHandle,500,300);
//	ezdsp5535_waitusec(40);
//    singAround(singStationHandle,128,3);
//    ezdsp5535_waitusec(250);
//    singAround(singStationHandle,128,3);
	//pulse_start();


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

	generate_sine_table(sineTable, 0.5, FREQ, S_RATE, SEQ_LEN); // generate sine table for pulse generation. This is 10 periods of 40 kHz sine wave. 10 periods are necessary as one 40 kHz period at 96 ksps would only be 2.4 samples per period and the table can therefore not be repeated.

	DMA_init();

	// Clear input buffer
	uint16_t i = 0;
	//for(;i<30000;i++){}
	for (i = 0; i < READ_BUFFER_LEN; i++) {
		buffer_read[i] = 0;
	}

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

    //CSL_I2S2_REGS->I2SINTMASK |= CSL_I2S_I2SINTMASK_XMITST_MASK; // Enable I2S2 stereo left/right transmit data interrupt

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
 * @param scale A scale between 0 and 1 (max amplitude).
 * @param freq Sinuoidal wave frequency.
 * @param s_rate Sample rate.
 * @param samples The number of samples to generate.
 */
void generate_sine_table(int32_t *table, float scale, float freq, float s_rate,
							uint16_t samples)
{
	uint16_t i = 0;
	float inc = 2 * M_PI * (freq / s_rate);
	for (; i < samples; i++)
	{
		table[i] = (((int32_t) (0x7fff * scale * sin(i * inc))) & 0xFFFF) << 16;
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

uint32_t dma_stop_address = 0;

// Callback function for when edge detection is stopping
void edge_detection_stop_callb(void)
{
    propagating = false;
	edge_detected = false; // Reset edge detection
	buffer_index_stop = (uint16_t) BUFFER_READ_CURR_INDEX; // Capture current buffer array index before DMA registers change
	reader_stop(&reader_handle);
	if (buffer_index_stop > 1000) {
		dma_stop_address = (CSL_DMA1_REGS->DMACH1DSAL | ((uint32_t) CSL_DMA1_REGS->DMACH1DSAU << 16));
		volatile hej = 0;
	}
	//cpy_int32_array_to_int16(buffer_read, buffer_read_int16, READ_BUFFER_LEN); // Copy upper 16 bits from the 32 bit dma buffer array into a 16 bit array that can be read easier
}

uint16_t full_cnt = 0;

uint16_t transmit_cnt = 0;

interrupt void DMA_ISR(void)
{
	// Disable interrupt flag so the interrupt can be called again later
	uint16_t ifr = CSL_SYSCTRL_REGS->DMAIFR;
	CSL_SYSCTRL_REGS->DMAIFR = ifr;

	// Check for particular interrupt flag bit and respond to it
	if (pulse_check_interrupt_flag(ifr)) { // DMA (I2S2 transmit) transfer complete interrupt
		pulse_repetition_ended_callb();
		if (!(CSL_DMA1_REGS->DMACH1TCR2 & CSL_DMA_DMACH1TCR2_EN_MASK)) {
			full_cnt = transmit_cnt;
		}
	}
}

bool prompt_stopwatch_start = false;

interrupt void I2S2_transmit_ISR(void) {
	if (prompt_stopwatch_start) {
		stopwatch_start(&tim_handle);
		transmit_cnt = 0;
		prompt_stopwatch_start = false;
	}
	if (prompt_gen_start) {
		pulse_start_periods(REPETITIONS);
		reader_start(&reader_handle);
		pulse_edge_detection_start();
		prompt_gen_start = false;
		prompt_stopwatch_start = true;
	}
	transmit_cnt++;
}

int16_t lastEdgeVal = 0;
uint32_t edgePtr = 0;

uint32_t highestPtr = 0;

interrupt void I2S2_receive_ISR(void)
{
	if (!propagating) return;

	// Copy I2S value and DMA destination addresses before they change
	short reg = (short) CSL_I2S2_REGS->I2SRXRT1;

	uint16_t curr_index = (uint16_t) BUFFER_READ_CURR_INDEX; // current array index being written to

	// if an edge has been detected, count down until the end of the recording
	pulse_edge_detection_stop_counter();

//	edgePtr = (uint32_t) CSL_DMA1_REGS->DMACH1DSAL | ((uint32_t) CSL_DMA1_REGS->DMACH1DSAU << 16);
//	if (edgePtr > highestPtr) highestPtr = edgePtr;
//	if (highestPtr > 0x15000) {
//		//reader_stop(&reader_handle);
//		volatile int hej = 0;
//		hej++;
//	}

	// Check if the current sample is an edge (if the absolute magnitude is above the threshold)
	if (!edge_detected  && (reg > EDGE_THRESHOLD || reg < -EDGE_THRESHOLD))
	{
		buffer_index_edge = curr_index; // Remember this index as the first edge detected
	    stopwatch_stop(singStationHandle->watch);


	    lastEdgeVal = reg;

		edge_detected = true;

		pulse_edge_detection_stop_in_n(REPETITIONS * SEQ_LEN + 10); // Capture another SEQ_LEN + 10 samples before stopping capturing
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

	IRQ_plug(XMT2_EVENT, &I2S2_transmit_ISR); // I2S2 receive interrupt
	IRQ_enable(XMT2_EVENT); // Enable I2S2 receive interrupt

	IRQ_globalEnable();
}
