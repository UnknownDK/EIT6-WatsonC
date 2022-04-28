/*
 * dma_init.c
 *
 *  Created on: 28. apr. 2022
 *      Author: tardi
 */

#include "dma_init.h"
#include "stdint.h"

void dma_init(int32_t *dest_addr) {

    // Deenable I2S before configuring DMA
    CSL_I2S2_REGS->I2SSCTRL &= ~((uint16_t)1<<15);


    // Take the DMA controller out of reset mode
//    1. Ensure the DMA controller is out of reset by setting the DMA_RST bit to 0 in the peripheral reset
//    control register (PRCR). PRCR is a chip configuration register, it is not part of the DMA controller, see
//    the device data manual for more details.
    if(CSL_SYSCTRL_REGS->PRCR & 1<<4) {
        error_handler();
    }



    // Enable DMA controller input clock
//    2. Enable the DMA controller input clock by setting the corresponding DMAnCG bit to 0 in the peripheral
//    clock gating configuration registers (PCGCR1 and PCGCR2). PCGCR1 and PCGCR2 are chip
//    configuration registers, they are not part of the DMA controller, see the device data manual for more
//    details.
    CSL_SYSCTRL_REGS->PCGCR2 &= ~(1<<3);


    // Clear all DMA channel interrupt flags
//    3. Ensure that all DMA channel interrupt flags are cleared by writing a 1 to the bits of the DMA interrupt
//    flag register (DMAIFR). Also, ensure all DMA interrupt flags in the CPU interrupt flag registers (IFR0
//    and IFR1) are cleared.
    CSL_SYSCTRL_REGS->DMAIFR |= (15<<4);

    // Enable channel interrupts
//    4. If using interrupts, enable the desired channel interrupt by setting the DMAnCHmIE bits of the interrupt
//    enable register (DMAIER). The CPU interrupt enable bit (INTEN) in the transfer control register 2
//    (DMACHmTCR2) must also be set.
    CSL_SYSCTRL_REGS->DMAIER |= (15<<4);


    // Select the desired event
//    5. If using synchronization events, select the event to be used through the CHmEVT bits of the channel
//    event source registers (DMAnCESR1 and DMAnCESR2). The synchronization mode bit (SYNCMODE)
//    of DMACHmTCR2 must also be set, although this should be done only when the channel is ready to
//    be enabled.
    CSL_SYSCTRL_REGS->DMA1CESR1 = (CSL_SYSCTRL_REGS->DMA1CESR1 & 0xF0) | 1;

    // Load source address
//    6. Load the source address to the source start address registers (DMACHmSSAL and DMACHmSSAU).
//    See Section 3.2.4, Start Address in a Channel, for more information on calculating the correct source
//    start address.
    CSL_DMA1_REGS->DMACH0SSAL = 0x2A0C; // Hard code for the address of (CSL_I2S2_REGS->I2STXRT0)


    // Load destination address
//    7. Load the destination address to the destination start address registers (DMACHmDSAL and
//    DMACHmDSAU). See Section 3.2.4, Start Address in a Channel, for more information on calculating
//    the correct destination start address.
    CSL_DMA1_REGS->DMACH0DSAL = (uint16_t)(((uint32_t) dest_addr) & 0xFF);
    CSL_DMA1_REGS->DMACH0DSAU = (uint16_t)(((uint32_t) dest_addr) >> 16);

    // Load DMA transfer control register 1
    //CSL_DMA1_REGS->DMACH0TCR1 =

    // Load DMA transfer control register 2
//    9. Configure DMACHmTCR2 accordingly. Through this register you can specify the source and
//    destination addressing modes and burst mode. You can also enable automatic reload, event
//    synchronization, CPU interrupts and Ping-Pong mode. Note that you must keep EN = 0 and
//    SYNCMODE = 0 during this step.
    CSL_DMA1_REGS->DMACH0TCR2 = (1<<13) | (1<<12) | (2<<8) | (1<<2);

    // Ensure corresponding peripheral is not active
//    10. If the DMA channel is servicing a peripheral, ensure that the corresponding peripheral is not active and
//    hence not generating synchronization events.
    if(CSL_I2S2_REGS->I2SSCTRL & ((uint16_t)1<<15)){
        error_handler();
    }

    // Enable DMA channel
//    11. Enable the DMA channel by setting EN = 1 (and SYNCMODE = 1 if using synchronization events).
    CSL_DMA1_REGS->DMACH0TCR2 |= (uint16_t)(1<<15);
    // Enable peripheral
//    12. If necessary, enable peripheral being serviced the DMA channel.

}


