/**
 * @file
 * @ingroup RTEMSBSPsARMMK64F12
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_MK64F12_BSP_H
#define LIBBSP_ARM_MK64F12_BSP_H

/**
 * @defgroup RTEMSBSPsARMMK64F12 MK64F12
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief MK64F12 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION
#define BSP_ARMV7M_IRQ_PRIORITY_DEFAULT (13 << 4)
#define BSP_ARMV7M_SYSTICK_PRIORITY (14 << 4)
#define BSP_ARMV7M_SYSTICK_FREQUENCY 120000000

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */


#endif /* LIBBSP_ARM_MK64F12_BSP_H */
