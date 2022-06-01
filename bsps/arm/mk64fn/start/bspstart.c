/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <assert.h>
#include <MK64F12.h>
#include <bsp/mk64fn.h>
#include <bsp/flashconfig.h>
__attribute__((used)) static const void *mkflash_cfg = &FlashConfig;

void BOARD_InitBootPins(void);
void BOARD_InitBootClocks(void);

uint32_t SystemCoreClock = 120000000;

void bsp_start( void )
{
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  bsp_interrupt_initialize();
}
