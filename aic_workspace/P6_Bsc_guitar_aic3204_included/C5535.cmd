/****************************************************************************/
/*  C5535.cmd                                                               */
/*  Copyright (c) 2012  Texas Instruments Incorporated                      */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on a C5535.              */
/*                 Use it as a guideline.  You will want to                 */
/*                 change the memory layout to match your specific          */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/

MEMORY
{
    MMR:     o = 0x000000  l = 0x0000c0  /* 192B Memory Mapped Registers */
    DARAM0:  o = 0x0000C0  l = 0x001F40  /* 8kB Dual Access RAM 0 */
    DARAM1_2:  o = 0x002000  l = 0x004000  /* 8kB Dual Access RAM 1 - stjaeler lige fra DARAM 2 */
    //DARAM2:  o = 0x004000  l = 0x002000  /* 8kB Dual Access RAM 2 */
    DARAM3_4:  o = 0x006000  l = 0x004000  /* 8kB Dual Access RAM 3 */
    //DARAM4:  o = 0x008000  l = 0x002000  /* 8kB Dual Access RAM 4 */
    DARAM_BIG: o = 0x00A000 l = 0x006000
    //DARAM5:  o = 0x00A000  l = 0x002000  /* 8kB Dual Access RAM 5 */
    //DARAM6:  o = 0x00C000  l = 0x002000  /* 8kB Dual Access RAM 6 */
    //DARAM7:  o = 0x00E000  l = 0x002000  /* 8kB Dual Access RAM 7 */
  
    SARAM0:   o = 0x010000  l = 0x002000  /* 8kB Single Access RAM 0 */
    SARAM1:   o = 0x012000  l = 0x002000  /* 8kB Single Access RAM 1 */
    SARAM2:   o = 0x014000  l = 0x002000  /* 8kB Single Access RAM 2 + det samme fra ram3 */
    SARAM3:   o = 0x016000  l = 0x002000  /* 8kB Single Access RAM 3 */
    SARAM4:   o = 0x018000  l = 0x002000  /* 8kB Single Access RAM 4 */
    SARAM5:   o = 0x01A000  l = 0x002000  /* 8kB Single Access RAM 5 */
    SARAM6:   o = 0x01C000  l = 0x002000  /* 8kB Single Access RAM 6 */
    SARAM7:   o = 0x01E000  l = 0x002000  /* 8kB Single Access RAM 7 */
    SARAM8:   o = 0x020000  l = 0x002000  /* 8kB Single Access RAM 8 */
    SARAM9:   o = 0x022000  l = 0x002000  /* 8kB Single Access RAM 9 */
    SARAM_40: o = 0x024000  l = 0x00A000
    //SARAM10:  o = 0x024000  l = 0x002000  /* 8kB Single Access RAM 10 */
    //SARAM11:  o = 0x026000  l = 0x002000  /* 8kB Single Access RAM 11 */
    //SARAM12:  o = 0x028000  l = 0x002000  /* 8kB Single Access RAM 12 */
    //SARAM13:  o = 0x02A000  l = 0x002000  /* 8kB Single Access RAM 13 */
    //SARAM14:  o = 0x02C000  l = 0x002000  /* 8kB Single Access RAM 14 */
    SARAMREST:o = 0x02E000  l = 0x020000  /* 16 * 8kB Single Access RAM 15-30 */
    SARAM31:  o = 0x04E000  l = 0x001e00  /* 8kB Single Access RAM 31 - minus lidt til VECS */
                    
    ROM(RIX):     o = 0xFE0000  l = 0x01FF00  /* 128kB ROM (MPNMC=0) or CS5 (MPNMC=1) */
    VECS: 	 o = 0x04FE00  l = 0x000200  /*  512B */

}

                    
SECTIONS            
{
	.text 			  >> SARAMREST|SARAM9|SARAM8
    vectors 	  	  >  VECS  /* If MPNMC = 1, remove the NOLOAD directive */
    .cinit            >  DARAM0
   	.pinit    		  >  DARAM0
	/* Arbitrary assignment of memory segments to .text section.   */
	/* Can be expanded or reduced observing limitations of SPRAA46  - not our comment*/
    .stack            >  DARAM_BIG
    .sysstack         >  DARAM_BIG
    .sysmem           >  DARAM3_4
    .data             >  DARAM3_4
    .cio              >  SARAM9
    .bss              >> DARAM1_2|SARAM6|SARAM7
    .const            >> SARAM_40|SARAM5
    .switch   		  >  SARAM8                /* Switch statement tables     */
    .args     		  >  SARAM1                /* Arguments to main()         */
    //data_br_buf  	  >  DARAM1_2 /* ADDR = 0x002000, Aligned to addr with 14 least-sig zeros */
    //fft_scratch 	  >  DARAM3_4



}
