#include "ezdsp5535.h"
#include "aic3204.h" 
#include "stdio.h"        // For printf();
#include "csl_error.h"
#include "csl_types.h"


/**
 * Initialize AIC3204 with a sampling frequency of 96 ksps on both ADC and DAC.
 * @return
 */
CSL_Status AIC3204_init()
{
    /* Configure Parallel Port */
	// Ensure that EBSR have set MODE 1 to enable I2S2
	if ((CSL_SYSCTRL_REGS->EBSR & CSL_SYS_EBSR_PPMODE_MASK) != (CSL_SYS_EBSR_PPMODE_MODE1 << CSL_SYS_EBSR_PPMODE_SHIFT)) {
		return CSL_ESYS_FAIL;
	}


    /* Configure AIC3204 registers */

    // Define starting point
    AIC3204_rset( 0, 0 );      // Select page 0
    AIC3204_rset( 1, 1 );      // Reset codec

    // Program Clock Settings PLL_CLKIN = MCLK = 12 MHz
    // J= 7 ,D = 1680 (0x0690) ,R = 1 (if PLL is necessary) PLL clock will be 86,016 MHz
    AIC3204_rset( 27, 0x1d );  // BCLK and WCLK is set as o/p to AIC3204(Master)
    AIC3204_rset( 28, 0x00 );  // Data ofset = 0
    AIC3204_rset( 4, 3 ); // Set PLL_CLK = CODEC_CLKIN
    AIC3204_rset( 6, 7 ); // J = 7
    AIC3204_rset( 7, 0x06 ); // D_HighByte, D = 1680
    AIC3204_rset( 8, 0x90 ); // D_LowByte, D = 1680
    AIC3204_rset( 30, 0x84 );  // For 32 bit clocks per frame in Master mode ONLY
    AIC3204_rset( 5, 1<<7 | 1<<4 | 1 ); // PLL power up P = 1, R = 1

    // Program and power up NDAC & MDAC
        AIC3204_rset( 11, 1<<7 | 4 ); // NDAC power up
        AIC3204_rset( 12, 1<<7 | 8 ); // MDAC power up (MACD is the same, if not powered up) ADC_MOD_CLK = DAC_MOD_CLK -
        AIC3204_rset( 18, 1<<7 | 7 );  // Power up NADC and set NADC value to 7
        AIC3204_rset( 19, 1<<7 | 2 );  // Power up MADC and set MADC value to 2


    // Program OSR value
    AIC3204_rset( 13, 0 ); // DOSR MSB
    AIC3204_rset( 14, 28 ); // DOSR LSB
    AIC3204_rset( 20, 64 ); // AOSR = DOSR = 64

    //Program the processing block to be used
    AIC3204_rset( 0x3d, 10 ); // ADC Signal Processing Block PRB_R10 is chosen
    //AIC3204_rset( 0x3d, 4);
    AIC3204_rset( 60, 7 );

    /* EXPOSE CLOCK_OUT ON GPIO */
    AIC3204_rset( 52, 4 << 2); // Expose CLKOUT on GPIO pin
    AIC3204_rset( 25, 0x05 ); // CDIV_CLKIN = ADC_MOD_CLK
    AIC3204_rset( 26, 1<<7 | 1 ); // Power up CLKOUT divider and set to 1

    // PROGRAM analog blocks
    AIC3204_rset( 0, 1 );      // Select page 1
    AIC3204_rset( 1, 8 );      // Disable crude AVDD generation from DVDD
    AIC3204_rset( 2, 1 );      // Enable Analog Blocks, use LDO power


    /* DAC ROUTING and Power Up */
    AIC3204_rset( 0, 1 );      // Select page 1
    AIC3204_rset( 0x0c, 8 );   // LDAC AFIR routed to HPL
    AIC3204_rset( 0x0d, 8 );   // RDAC AFIR routed to HPR
    AIC3204_rset( 0, 0 );      // Select page 0
    AIC3204_rset( 64, 2 );     // Left vol=right vol
    AIC3204_rset( 65, 0 );     // Left DAC gain to 0dB VOL; Right tracks Left
    AIC3204_rset( 63, 0xd4 );  // Power up left,right data paths and set channel
    AIC3204_rset( 0, 1 );      // Select page 1
    AIC3204_rset( 16, 0 );  // Unmute HPL , 0dB gain
    AIC3204_rset( 17, 0 );  // Unmute HPR , 0dB gain
    AIC3204_rset( 9, 0x30 );   // Power up HPL,HPR
    AIC3204_rset( 0, 0 );      // Select page 0
        ezdsp5535_wait( 100 );    // wait
    /* ADC ROUTING and Power Up */
    AIC3204_rset( 0, 1 );      // Select page 1
    AIC3204_rset( 51, 0x48);  // power up MICBIAS with AVDD (0x40)or LDOIN (0x48)   //MM - added micbias
    AIC3204_rset( 0x34, 0x10 );// STEREO 1 Jack
                               // IN2_L to LADC_P through 0 kohm
    AIC3204_rset( 0x37, 0x10 );// IN2_R to RADC_P through 0 kohmm
    AIC3204_rset( 0x36, 1 );   // CM_1 (common mode) to LADC_M through 0 kohm
    AIC3204_rset( 0x39, 0x40 );// CM_1 (common mode) to RADC_M through 0 kohm
    AIC3204_rset( 0x3b, 1<<7 );   // MIC_PGA_L unmute gain set to 1
    AIC3204_rset( 0x3c, 1<<7 );   // MIC_PGA_R unmute gain set to 1
    AIC3204_rset( 0, 0 );      // Select page 0
    AIC3204_rset( 0x51, 0xc0 );// Powerup Left and Right ADC
    AIC3204_rset( 0x52, 0 );   // Unmute Left and Right ADC








    AIC3204_rset( 0, 0 );
    ezdsp5535_wait( 100 );  // Wait for 100 cycles


    /* Configure I2S */
    I2S2_SRGR = 0x0;     // Set sample rate generator register to 0
    I2S2_CR = 0x8010;    // Set serializer control regsiter for 16-bit word, slave, enable I2C

    return CSL_SOK;
}

