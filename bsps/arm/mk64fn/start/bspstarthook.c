/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/start.h>

/*
 * There is a very short window after power on to reconfigure or disable the
 * watchdog. This code is how the kinetis sdk does it.
 */
void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
    /* Write 0xC520 to watchdog unlock register */
    *((volatile unsigned short *)0x4005200E) = 0xC520;
    /*  Followed by 0xD928 to complete the unlock */
    *((volatile unsigned short *)0x4005200E) = 0xD928;
    /* Now disable watchdog via STCTRLH register */
    *((volatile unsigned short *)0x40052000) = 0x01D2u;
}

void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  bsp_start_clear_bss();

  /* At this point we can use objects outside the .start section */
}
