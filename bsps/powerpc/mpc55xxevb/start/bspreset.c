/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief BSP reset.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdbool.h>

#include <bsp/bootcard.h>

#include <mpc55xx/regs.h>

void bsp_reset(void)
{
  while (true) {
    #if MPC55XX_CHIP_FAMILY == 564
      /* TODO */
    #else
      SIU.SRCR.R = 1U << (31 - 0);
    #endif
  }
}
