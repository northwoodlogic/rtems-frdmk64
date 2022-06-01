/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libchip/sersupp.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/usart.h>
#include <bsp/mk64fn.h>
#include <fsl_uart.h>
#include <fsl_clock.h>

static uint8_t uart_initialized = 0;

#if 0
static uint32_t usart_get_baud(const console_tbl *ct)
{
  return ct->ulClock;
}
#endif

static void usart_initialize(int minor)
{
  // const console_tbl *ct = Console_Port_Tbl [minor];

  uart_config_t cfg;
  UART_GetDefaultConfig(&cfg);
  cfg.enableTx = true;
  cfg.enableRx = true;
  UART_Init(UART0, &cfg, CLOCK_GetCoreSysClkFreq());
}

static int usart_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  const console_tbl *ct = Console_Port_Tbl [minor];
  console_data *cd = &Console_Port_Data [minor];

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, ct->ulClock);

  return 0;
}

static int usart_last_close(int major, int minor, void *arg)
{
  return 0;
}

static int usart_read_polled(int minor)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  if (!uart_initialized) {
    usart_initialize(minor);
    uart_initialized = 1;
  }

  if (UART_GetRxFifoCount(UART0) > 0)
    return UART_ReadByte(UART0) & 0xFF;

  
  return -1;
}

char console_membuf[8192] = { 0 };
int console_membuf_ptr = 0;

static void usart_write_polled(int minor, char c)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  if (!uart_initialized) {
    usart_initialize(minor);
    uart_initialized = 1;
  }
  console_membuf[console_membuf_ptr++ % sizeof(console_membuf)] = c;
  UART_WriteBlocking(UART0, (const uint8_t *)&c, 1);
}

static ssize_t usart_write_support_polled(
  int minor,
  const char *s,
  size_t n
)
{
  ssize_t i = 0;

  for (i = 0; i < n; ++i) {
    usart_write_polled(minor, s [i]);
  }

  return n;
}

static int usart_set_attributes(int minor, const struct termios *term)
{
  return -1;
}

const console_fns mk64fn_usart_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = usart_first_open,
  .deviceLastClose = usart_last_close,
  .deviceRead = usart_read_polled,
  .deviceWrite = usart_write_support_polled,
  .deviceInitialize = usart_initialize,
  .deviceWritePolled = usart_write_polled,
  .deviceSetAttributes = usart_set_attributes,
  .deviceOutputUsesInterrupts = false
};
