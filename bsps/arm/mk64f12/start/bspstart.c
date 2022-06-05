/*
 * Copyright (c) 2022 Dave Rush.  All rights reserved.
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
#include <bsp/mk64f12.h>
#include <bsp/flashconfig.h>
__attribute__((used)) static const void *mkflash_cfg = &FlashConfig;

void BOARD_InitBootPins(void);
void BOARD_InitBootClocks(void);
static void bsp_earlycon(void);

uint32_t SystemCoreClock = 120000000;

/*
 * Using FSL, setup console for early use on UART0.
 * 115200 8N1
 */
static void
bsp_earlycon(void)
{
  uart_config_t cfg;
  UART_GetDefaultConfig(&cfg);
  cfg.enableTx = true;
  cfg.enableRx = true;
  UART_Init(UART0, &cfg, CLOCK_GetCoreSysClkFreq());
}

void bsp_start(void)
{
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  bsp_earlycon();
  bsp_interrupt_initialize();
}
