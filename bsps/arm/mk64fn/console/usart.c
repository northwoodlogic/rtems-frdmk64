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

#if 0
static volatile stm32f4_usart *usart_get_regs(const console_tbl *ct)
{
  return (stm32f4_usart *) ct->ulCtrlPort1;
}
#endif

#if 0
static rtems_vector_number usart_get_irq_number(const console_tbl *ct)
{
  return ct->ulIntVector;
}
#endif

#if 0
static const stm32f4_rcc_index usart_rcc_index [] = {
  STM32F4_RCC_USART1,
  STM32F4_RCC_USART2,
  STM32F4_RCC_USART3,
  STM32F4_RCC_UART4,
  STM32F4_RCC_UART5,
#ifdef STM32F4_FAMILY_F4XXXX
  STM32F4_RCC_USART6
#endif /* STM32F4_FAMILY_F4XXXX */
};

static stm32f4_rcc_index usart_get_rcc_index(const console_tbl *ct)
{
  return usart_rcc_index [ct->ulCtrlPort2];
}
#endif


static uint32_t usart_get_baud(const console_tbl *ct)
{
  return ct->ulClock;
}

static void usart_initialize(int minor)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
#if 0
  volatile stm32f4_usart *usart = usart_get_regs(ct);
  uint32_t pclk = usart_get_pclk(ct);
  uint32_t baud = usart_get_baud(ct);
  stm32f4_rcc_index rcc_index = usart_get_rcc_index(ct);

  stm32f4_rcc_set_clock(rcc_index, true);

  usart->cr1 = 0;
  usart->cr2 = 0;
  usart->cr3 = 0;
  usart->bbr = usart_get_bbr(usart, pclk, baud);
  usart->cr1 = STM32F4_USART_CR1_UE
    | STM32F4_USART_CR1_TE
    | STM32F4_USART_CR1_RE;
#endif
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
  return -1;
#if 0
  volatile stm32f4_usart *usart = usart_get_regs(ct);

  if ((usart->sr & STM32F4_USART_SR_RXNE) != 0) {
    return STM32F4_USART_DR_GET(usart->dr);
  } else {
    return -1;
  }
#endif
}

char console_membuf[8192] = { 0 };
int console_membuf_ptr = 0;

static void usart_write_polled(int minor, char c)
{
  const console_tbl *ct = Console_Port_Tbl [minor];
  console_membuf[console_membuf_ptr++ % sizeof(console_membuf)] = c;
#if 0
  volatile stm32f4_usart *usart = usart_get_regs(ct);

  while ((usart->sr & STM32F4_USART_SR_TXE) == 0) {
    /* Wait */
  }

  usart->dr = STM32F4_USART_DR(c);
#endif
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
