/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#include "system.h"

rtems_task Priority_task(
  rtems_task_argument its_index_arg
)
{
  rtems_interval      timeout;
  rtems_task_priority its_priority;
  rtems_task_priority current_priority;
  rtems_status_code   status;
  int                 its_index;
  uint32_t            index;

  its_index = (int) its_index_arg;
  its_priority = Task_priority[ its_index ];

  if ( its_priority < 3 )
    timeout = 5 * rtems_clock_get_ticks_per_second();
  else
    timeout = RTEMS_NO_TIMEOUT;

  put_name( Priority_task_name[ its_index ], FALSE );
  puts( " - rtems_semaphore_obtain - wait forever on SM2" );

  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    timeout
  );
  directive_failed( status, "rtems_semaphore_obtain of SM2" );

  if ( its_priority < PRIORITY_INHERIT_BASE_PRIORITY ) {
    printf(
      "PRI%d - WHY AM I HERE? (pri=%" PRIdrtems_task_priority ")",
     its_index,
     its_priority
    );
    rtems_test_exit( 0 );
  }

  /* special case of setting priority while holding a resource */
  {
    rtems_task_priority priority;
    rtems_task_priority old_priority;

    puts( "Set priority of self while holding resource" );
    status =
      rtems_task_set_priority( RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority );
    directive_failed( status, "rtems_task_set_priority get current" );
    status = rtems_task_set_priority( RTEMS_SELF, priority, &old_priority );
    directive_failed( status, "rtems_task_set_priority with resource" );
    if ( priority != old_priority ) {
      printf(
        "priority != old_priority (%" PRIdrtems_task_priority
            " != %" PRIdrtems_task_priority ")\n",
         priority,
         old_priority
      );
      rtems_test_exit(0);
    }
  }

  if ( its_index == 5 )
    puts( "PRI5 - rtems_task_suspend - until all priority tasks blocked" );
  status = rtems_task_suspend( RTEMS_SELF );
  directive_failed( status, "rtems_task_suspend" );

  puts( "PRI5 - rtems_task_delete - all tasks waiting on SM2" );
  for ( index = 1 ; index < 5 ; index++ ) {
    status = rtems_task_delete( Priority_task_id[ index ] );
    directive_failed( status, "rtems_task_delete loop" );
  }

  puts( "PRI5 - rtems_semaphore_obtain - nested" );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 2 ],
    RTEMS_DEFAULT_OPTIONS,
    timeout
  );
  directive_failed( status, "rtems_semaphore_obtain nested" );

  puts( "PRI5 - rtems_semaphore_release - nested" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release nested " );

  puts( "PRI5 - rtems_semaphore_release - restore priority" );
  status = rtems_semaphore_release( Semaphore_id[ 2 ] );
  directive_failed( status, "rtems_semaphore_release" );

  status = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &current_priority
  );
  directive_failed( status, "PRI5 rtems_task_set_priority CURRENT" );
  printf(
    "PRI5 - priority of PRI5 is %" PRIdrtems_task_priority "\n",
     current_priority
  );

  (void) rtems_task_suspend( RTEMS_SELF );
}
