/*
 * RTEMS generic mcf548x BSP
 *
 * This file declares glue functions to the Freescale Mcdma API.
 */

/*
 * Copyright (c) 2004, 2009 embedded brains GmbH. All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _MCDMA_GLUE_H
#define _MCDMA_GLUE_H

#include <rtems.h>
#include <rtems/irq.h>
#include <mcf548x/mcf548x.h>

#define MCDMA_CLR_PENDING(chan) (MCF548X_DMA_DIPR = (1 << (chan)))
#define MCDMA_GET_PENDING(chan) (MCF548X_DMA_DIPR & (1 << (chan)))

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_enable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   enable interrupt for given task number                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_taskno                           /* task number to enable    */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_disable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   disable interrupt for given task number                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_taskno                           /* task number to disable   */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_install
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   install given function as mcdma interrupt handler                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_taskno,                          /* task number for handler  */
 void (*the_handler)(rtems_irq_hdl_param),     /* function to call         */
 void *the_param
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the mcdma module (if not yet done):                          |
|   - load code                                                             |
|   - initialize registers                                                  |
|   - initialize bus arbiter                                                |
|   - initialize interrupt control                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void *sram_base         /* base address for SRAM, to be used for DMA task */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

#endif /* _MCDMA_GLUE_H */
