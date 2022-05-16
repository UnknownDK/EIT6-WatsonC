

/**
 * main.c
 */
#include "stdint.h"
#include <math.h>
#include <hwafft.h>

#define FREQ 25213      // Pulse sine frequency
#define S_RATE 96000    // Sample rate
#define SEQ_LEN 1024      // Gives 10 periods at 40 kHz
#define M_PI 3.14159265358979323846

int32_t sineTable[SEQ_LEN] = { 0 };

void generate_sine_table(int32_t *table, float freq, float s_rate,
                            uint16_t samples)
{
    uint16_t i = 0;
    float inc = 2 * M_PI * (freq / s_rate);
    for (; i < samples; i++)
    {
        table[i] = (((int32_t) (0x3f * sin(i * inc))) & 0xFFFF) << 16;
    }
}

#pragma DATA_SECTION(data_br_buf,"data_br_buf");    //Kommando der placere arrayet med bit reversed pladser et bestemt sted
#pragma DATA_ALIGN(data_br_buf,1024);           //-||-
int32_t data_br_buf[512];                        //Array der har sine indgange bit reversed

#pragma DATA_SECTION(scratch_buf,"scratch_buf");    //Kommando der placere arrayet med bit reversed pladser et bestemt sted
#pragma DATA_ALIGN(scratch_buf,1024);           //-||-
int32_t scratch_buf[512];                        //Array der indeholder bare reele og imaginære værdier, men indgangende har skiftet plads med bit reverse

#define fft_length 256

int main(void)
{
//    const uint16_t fft_length = 256;                     // Lenght of the FFT
    uint16_t fft_save_location;

    generate_sine_table(sineTable, FREQ, S_RATE, SEQ_LEN);

//    int i = 0;
//    for (i=0;i<fft_length;i++){
//        data_br_buf[i] = sineTable[i];
//    }
    hwafft_br(sineTable, data_br_buf,fft_length);

    fft_save_location = hwafft_256pts(data_br_buf, scratch_buf,0,0); // Function for calling the hardware FFT
    int i = 0;

    int y = 0;

    for (i=0;i<1000;i++){
        y++;
    }
    unsigned short Real_Part[fft_length];
    unsigned short Imaginary_Part[fft_length];
    for (i=0;i<fft_length;i++){
        Real_Part[i] = data_br_buf[i] >> 16;
        Imaginary_Part[i] = data_br_buf[i] & 0x0000FFFF;
    }




    i = 0;

}
