/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>
#include <libchip/serial.h>

#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/usart.h>
#include <bsp/mk64f12.h>

console_tbl Console_Configuration_Ports [] = {
    {
      .sDeviceName = "/dev/ttyS0",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &mk64f12_usart_fns,
      .ulCtrlPort1 = (uint32_t)UART0, /* uart 0 */
      .ulCtrlPort2 = 0,
      .ulClock = 115200,
      .ulIntVector = 31
    },
};

#define PORT_COUNT \
  (sizeof(Console_Configuration_Ports) \
    / sizeof(Console_Configuration_Ports [0]))

unsigned long Console_Configuration_Count = PORT_COUNT;

static void output_char(char c)
{
  const console_fns *con =
    Console_Configuration_Ports [Console_Port_Minor].pDeviceFns;
  /*
   * note to self, this tries writing to the uart data register before
   * the uart has been configured. It causes a bus abort if the uart
   * clock isn't running
   */
  con->deviceWritePolled((int) Console_Port_Minor, c);
}

static int input_char(void)
{
  const console_fns *con =
    Console_Configuration_Ports [Console_Port_Minor].pDeviceFns;
  /*
   * note to self, this tries writing to the uart data register before
   * the uart has been configured. It causes a bus abort if the uart
   * clock isn't running
   */
  return con->deviceRead((int) Console_Port_Minor);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = input_char;
