/**************************************************************************//**
 * @file     arm-helpers.h
 * @brief    CMSIS Cortex-M4 Core Peripheral Access Layer Header File
 * @version  V4.30
 * @date     20. October 2015
 ******************************************************************************/
/* Copyright (c) 2009 - 2015 ARM LIMITED
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
#include "stm32f446x.h"

// /**
//   \brief   Set Priority Grouping
//   \details Sets the priority grouping field using the required unlock sequence.
//            The parameter PriorityGroup is assigned to the field SCB->AIRCR [10:8] PRIGROUP field.
//            Only values from 0..7 are used.
//            In case of a conflict between priority grouping and available
//            priority bits (__NVIC_PRIO_BITS), the smallest possible priority group is set.
//   \param [in]      PriorityGroup  Priority grouping field.
//  */
// static inline void NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
// {
//   uint32_t reg_value;
//   uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);             /* only values 0..7 are used          */

//   reg_value  =  SCB->AIRCR;                                                   /* read old register configuration    */
//   reg_value &= ~((uint32_t)(SCB_AIRCR_VECTKEY_Msk | SCB_AIRCR_PRIGROUP_Msk)); /* clear bits to change               */
//   reg_value  =  (reg_value                                   |
//                 ((uint32_t)0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
//                 (PriorityGroupTmp << 8U)                      );              /* Insert write key and priorty group */
//   SCB->AIRCR =  reg_value;
// }

// /**
//   \brief   Enable External Interrupt
//   \details Enables a device-specific interrupt in the NVIC interrupt controller.
//   \param [in]      IRQn  External interrupt number. Value cannot be negative.
//  */
// static inline void NVIC_EnableIRQ(IRQn_Type IRQn)
// {
//   NVIC->ISER[(((uint32_t)(int32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
// }

// /**
//   \brief   No Operation
//   \details No Operation does nothing. This instruction can be used for code alignment purposes.
//  */
// __attribute__((always_inline)) static inline void __NOP(void)
// {
//   __asm volatile ("nop");
// }

// /**
//   \brief   Data Synchronization Barrier
//   \details Acts as a special kind of Data Memory Barrier.
//            It completes when all explicit memory accesses before this instruction complete.
//  */
// __attribute__((always_inline)) static inline void __DSB(void)
// {
//   __asm volatile ("dsb 0xF":::"memory");
// }

// /**
//   \brief   System Reset
//   \details Initiates a system reset request to reset the MCU.
//  */
// static inline void NVIC_SystemReset(void)
// {
//   __DSB();                                                          /* Ensure all outstanding memory accesses included
//                                                                        buffered write are completed before reset */
//   SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
//                            (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
//                             SCB_AIRCR_SYSRESETREQ_Msk    );         /* Keep priority group unchanged */
//   __DSB();                                                          /* Ensure completion of memory access */

//   for(;;)                                                           /* wait until reset */
//   {
//     __NOP();
//   }
// }