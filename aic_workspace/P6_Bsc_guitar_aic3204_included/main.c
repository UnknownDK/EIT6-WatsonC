#include <main.h>

#include "stdio.h"
#include "math.h"
#include "stdint.h"
//#include "string.h"
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
#include <Flowmeter/expansion_board.h>
#include <Dsplib.h>

#define FREQ 40000      // Pulse sine frequency
#define S_RATE 96000    // Sample rate
#define SEQ_LEN 128      // Gives 10 periods at 40 kHz
#define READ_BUFFER_LEN 1000
#define EDGE_THRESHOLD 12000    // Threshold that correspond to approx. positive 366 mVp voltage

// Get the index of the buffer_read array that is currently being (or hast last been) written to
#define BUFFER_READ_CURR_INDEX INT32_ARRAY_INDEX_FROM_ADDR(DMA1CH1_WORD_DEST_ADDR - 1, buffer_read) // "-1" is because the DMA (assumably) has already changed destination to the next element, whenever we try to read the address

// Declare functions prototypes
void flowmeter_init();  // init board and codec
void GPIO_test_init();
void do_sample_and_gain();

short max_finder(short array[], short length);
void crosscorr_test();
void fft_test();
void fdzp(short array[], short length);

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

#define FFTSIZE 16

long fftSignal[FFTSIZE*2] = { 0 };

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

//#pragma DATA_SECTION(data_br_buf, "data_br_buf");  //assign to certain memory regions
//#pragma DATA_SECTION(fft_scratch, "fft_scratch");  //this secures memory allignment
//int32_t fft_scratch[4096];
//int32_t data_br_buf[4096];




int main(void)
{

	//flowmeter_init();   // init board and codec

	//edge_detected = false;
	//pulse_edge_detection_start();
	//reader_start(&reader_handle);

	//pulse_start_periods(1);
	//pulse_start();
    crosscorr_test();
    //fft_test();


	volatile unsigned long tick = 0;

	while (1)
	{
		tick++;
	}
}

//int32_t fakeSignal[32] = { 0 };
//int32_t compareSignal[16] = { 0 };
// Cross correlation test ting
#define NX 16
void fft_test()
{
    //int i = 0;
    //generate_sine_table(sineTable, FREQ, S_RATE, SEQ_LEN);
    //for(i=0;i<FFTSIZE;i++){
    //    fftSignal[i] = (sineTable[i]);
    //}
    //long test[32] = { 0x599a, 0xd99a, 0x199a, 0xc000, 0x4ccd, 0xe666, 0x2666, 0xc000, 0x599a, 0xd99a, 0x199a, 0xc000, 0x4ccd, 0xe666, 0x2666, 0xc000 };

    //long test2[64];
    //for(i=0;i<FFTSIZE;i++){
    //    test[i] = (test[i]<<16);
    //}
//    for(i=0;i<64;i++){
//        test2[i] = 0;
//    }
    //rfft32(fftSignal,16,SCALE);

    long x[2*NX] ={
    -18187058, 46331372,
    54834337, 44286577,
    27689626, -30515381,
    7743059, -77363268,
    22536296, 60019827,
    -17048265, 13112998,
    28734391, 681949,
    9009481, -13622328,
    20970687, -51502515,
    -35142830, 7428049,
    51528547, -46119851,
    14576909, -28161749,
    -64328405, -7000883,
    15768554, 25364231,
    42760128, 12607556,
    21425929, -45144568,
    };



    cfft32_NOSCALE(x, NX);
    cbrev32(x, x, NX);
    cifft32_NOSCALE(x, NX);
    cbrev32(x, x, NX);

//    short Real_Part[FFTSIZE];
//    short Imaginary_Part[FFTSIZE];
//    for (i=0;i<FFTSIZE;i++){
//        Real_Part[i] = test2[i] >> 16;
//        Imaginary_Part[i] = test2[i] & 0x0000FFFF;
//    }
//    cifft32_SCALE(test, FFTSIZE);



}
#define COMPSIGLEN 21
#define FAKSIGLEN 128
#define RSLTCORRLEN (COMPSIGLEN+FAKSIGLEN-1)

ushort offlag = 0;
short resultCorr[RSLTCORRLEN];
short fakeSignal[FAKSIGLEN] = { 0 };
short compareSignal[COMPSIGLEN] = { 0 };
long  fdzpArray[FAKSIGLEN*4];

void crosscorr_test()
{
    int i = 0;
    generate_sine_table(sineTable, FREQ, S_RATE, SEQ_LEN);
    //memset(fakeSignal,0,sizeof(fakeSignal));
    //memset(compareSignal,0,sizeof(compareSignal));
    for(i=0;i<RSLTCORRLEN;i++){
        resultCorr[i] = 0;
    }
    for(i=0;i<FAKSIGLEN;i++){
        fakeSignal[i] = 0;
    }
    for(i=0;i<COMPSIGLEN;i++){
        compareSignal[i] = 0;
    }
    i = 0;
    for(i=30;i<51;i++){
        fakeSignal[i] = (short)(sineTable[i-30]>>16);
    }
    i = 0;
    for(i=0;i<21;i++){
        compareSignal[i] = (short)(sineTable[i]>>16);
    }
    offlag = 0;
    //fdzp();
    fdzp(fakeSignal,FAKSIGLEN);
    //corr_bias  (DATA *x, DATA *y, DATA *r, ushort nx, ushort ny);
    offlag = corr_raw(compareSignal, fakeSignal, resultCorr, COMPSIGLEN, FAKSIGLEN);
    short corrIndex = max_finder(resultCorr, RSLTCORRLEN);
    float timeLag = ((float)(corrIndex +1)-COMPSIGLEN)/S_RATE; // Magi
}
//766

void fdzp(short shortArray[], short length){
    int i = 0;
    for(i=0;i<length*2;i++){
        //fdzpArray[i] = i % 2 ? 0x00 : shortArray[(int)(i/2)];
        fdzpArray[i] = i % 2 ? 0x00000000 : shortArray[i/2];
    }
    for(i=length*2;i<length*4;i++){
        //fdzpArray[i] = i % 2 ? 0x00 : shortArray[(int)(i/2)];
        fdzpArray[i] = 0x00000000;
    }
    //0xFE79 == 0xFFFFFE79
    cfft32_SCALE(fdzpArray, length);
    cbrev32(fdzpArray, fdzpArray, length);
    cifft32_SCALE(fdzpArray, length);
    cbrev32(fdzpArray, fdzpArray, length);
}

short max_finder(short array[], short length){
    short i = 0;
    short largestYet = array[0];
    short index = 0;
    for(i=0;i<length;i++){
        if(largestYet < array[i]){
            largestYet = array[i];
            index = i;
        }
    }
    return index;
}

void flowmeter_init()
{
	generate_sine_table(sineTable, FREQ, S_RATE, SEQ_LEN); // generate sine table for pulse generation. This is 10 periods of 40 kHz sine wave. 10 periods are necessary as one 40 kHz period at 96 ksps would only be 2.4 samples per period and the table can therefore not be repeated.
	//memset(buffer_read, 0, sizeof(buffer_read)); // clear read buffer

	// eZdsp - dev board
	ezdsp5535_init();
	exp_handle = &exp_obj;
	exp_board_init(exp_handle);


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
void generate_sine_table(int32_t *table, float freq, float s_rate,
							uint16_t samples)
{
	uint16_t i = 0;
	float inc = 2 * M_PI * (freq / s_rate);
	for (; i < samples; i++)
	{
		//table[i] = (((int32_t) (0x30 * sin(i * inc))) & 0xFFFF) << 16;
	    table[i] = (((int32_t) (0x30F * sin(i * inc))) & 0xFFFF) << 16;
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
