/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief Benchmark timer support.
 */

/*
 * Copyright (c) 2008, 2009 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/btimer.h>

#include <bsp.h>

static uint32_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = lpc32xx_timer();
}

benchmark_timer_t benchmark_timer_read(void)
{
  return lpc32xx_timer() - benchmark_timer_base;
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_average_overhead)
{
  /* VOID */
}
