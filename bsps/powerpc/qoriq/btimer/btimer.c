/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief QorIQ benchmark timer.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/btimer.h>

#include <libcpu/powerpc-utility.h>

static benchmark_timer_t benchmark_timer_base;

void benchmark_timer_initialize(void)
{
  benchmark_timer_base = ppc_alternate_time_base();
}

benchmark_timer_t benchmark_timer_read(void)
{
  return ppc_alternate_time_base() - benchmark_timer_base;
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_average_overhead)
{
  /* VOID */
}
