/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT

#include "system.h"
#include "tmacros.h"
#include <stdio.h>
#include <stdlib.h>

const char rtems_test_name[] = "UNLIMITED SHELL";

#include <rtems/shell.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_task_priority old_priority;
  rtems_mode          old_mode;

  /* lower the task priority to allow created tasks to execute */
  rtems_task_set_priority(
    RTEMS_SELF, RTEMS_MAXIMUM_PRIORITY - 1, &old_priority);
  rtems_task_mode(RTEMS_PREEMPT,  RTEMS_PREEMPT_MASK, &old_mode);

  rtems_shell_init(
      "SHLL",                       /* task name */
      RTEMS_MINIMUM_STACK_SIZE * 4, /* task stack size */
      100,                          /* task priority */
      "/dev/console",               /* device name */
      false,                        /* run forever */
      true,                         /* wait for shell to terminate */
      NULL       /* login check function,
      use NULL to disable a login check */
  );

  exit( 0 );
}

