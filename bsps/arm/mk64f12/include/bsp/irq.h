/**
 * @file
 * @ingroup mk64fn_interrupt
 * @brief Interrupt definitions.
 */

/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_MK64FN_IRQ_H
#define LIBBSP_ARM_MK64FN_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/**
 * @defgroup mk64fn_interrupt Interrupt Support
 * @ingroup RTEMSBSPsARMMK64FN
 * @brief Interrupt Support
 * @{
 */

/* TODO: Add definitions 
 * See K64 Sub-Family Reference manual page 81. 
 */
#define MK64FN_IRQ_DMAXXX 0
#define MK64FN_IRQ_EMAC_ERR 85


/* Page 77 says "16 priorities"  */
#define MK64FN_IRQ_PRIORITY_VALUE_MIN 0
#define MK64FN_IRQ_PRIORITY_VALUE_MAX 15
#define MK64FN_IRQ_PRIORITY_COUNT (MK64FN_IRQ_PRIORITY_VALUE_MAX + 1)
#define MK64FN_IRQ_PRIORITY_HIGHEST MK64FN_IRQ_PRIORITY_VALUE_MIN
#define MK64FN_IRQ_PRIORITY_LOWEST MK64FN_IRQ_PRIORITY_VALUE_MAX

/*
 * Looking at the startup asm code this is the number of IRQs that
 * come after the SysTick handler
 *
 * See K64 Sub-Family Reference manual page 81. 
 */
#define BSP_INTERRUPT_VECTOR_COUNT 86

/** @} */

#endif /* LIBBSP_ARM_MK64FN_IRQ_H */
