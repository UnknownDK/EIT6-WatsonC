/*
 * main.h
 *
 *  Created on: 11. maj 2022
 *      Author: Mikkel S. Hansen
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

#define M_PI 3.14159265358979323846

#define STATUS_CHECK(TARGET) status = TARGET; if (status != CSL_SOK) return status; // The target function call is checked for CSL_Status and returns if it is not CSL_SOK

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

#define FREQ 40000      // Pulse sine frequency
#define S_RATE 96000    // Sample rate
#define SEQ_LEN 24      // Gives 10 periods at 40 kHz Change to 21 to match matlab

// The number of margin samples desired before and after the pulse sequence
#define PULSE_SAMPLES_END_MARGIN 0
#define PULSE_SAMPLES_START_MARGIN 16

#define PULSE_SAMPLE_LENGTH (REPETITIONS * SEQ_LEN) // = (2 * 24) + (16) + 0 = 64

/* Crosscorr, FDZP */
#define INTERP_F 8
#define INSIGLEN (PULSE_SAMPLE_LENGTH + PULSE_SAMPLES_START_MARGIN + PULSE_SAMPLES_END_MARGIN) // Incoming signal; We want to know how delayed this is.
#define OUTSIGLEN  (INSIGLEN*INTERP_F) // Outgoing signal; Signal after fdzp
#define COMPSIGLEN (PULSE_SAMPLE_LENGTH*INTERP_F) // Compare signal; We are looking for this
#define FDZPARRAYLEN (OUTSIGLEN*4) // Length of FDZP array due to function requirements
#define RSLTCORRLEN (COMPSIGLEN+OUTSIGLEN-1) // Length of output array for corr_raw

void cpy_int32_array_to_int16(int32_t *src, int16_t *dest, uint16_t len);

void memory_set(uint16_t * start, uint16_t val, uint16_t size);

#endif /* MAIN_H_ */
