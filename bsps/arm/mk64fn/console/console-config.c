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
#include <bsp/mk64fn.h>

console_tbl Console_Configuration_Ports [] = {
    {
      .sDeviceName = "/dev/ttyS0",
      .deviceType = SERIAL_CUSTOM,
      .pDeviceFns = &mk64fn_usart_fns,
      .ulCtrlPort1 = (uint32_t) 0x4006A000u, /* uart 0 */
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

  con->deviceWritePolled((int) Console_Port_Minor, c);
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
