-- SPDX-License-Identifier: BSD-2-Clause

--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 4 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions
--  are met:
--  1. Redistributions of source code must retain the above copyright
--     notice, this list of conditions and the following disclaimer.
--  2. Redistributions in binary form must reproduce the above copyright
--     notice, this list of conditions and the following disclaimer in the
--     documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
--

with INTERFACES; use INTERFACES;
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIMER_DRIVER;
with RTEMS.SEMAPHORE;

package body TMTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;

      TMTEST.TEST_INIT;

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TEST_INIT
--

   procedure TEST_INIT
   is
      STATUS   : RTEMS.STATUS_CODES;
   begin

      TMTEST.TASK_COUNT := TIME_TEST_SUPPORT.OPERATION_COUNT;

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASKS.CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            10, 
            1024, 
            RTEMS.NO_PREEMPT,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         RTEMS.TASKS.START( 
            TMTEST.TASK_ID( INDEX ),
            TMTEST.LOW_TASKS'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

      RTEMS.SEMAPHORE.CREATE(
         RTEMS.BUILD_NAME( 'S', 'M', '1', ' ' ),
         0,
         RTEMS.DEFAULT_ATTRIBUTES,
         RTEMS.TASKS.NO_PRIORITY,
         TMTEST.SEMAPHORE_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_CREATE OF SM1" );

   end TEST_INIT;

-- 
--  HIGHEST_TASK
--

   procedure HIGHEST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      OLD_PRIORITY : RTEMS.TASKS.PRIORITY;
      STATUS       : RTEMS.STATUS_CODES;
   begin

      if ARGUMENT = 1 then

         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

         TIME_TEST_SUPPORT.PUT_TIME( 
            "TASK_RESTART (blocked, preempt)",
            TMTEST.END_TIME, 
            1, 
            0,
            RTEMS_CALLING_OVERHEAD.TASK_RESTART
         );
 
         RTEMS.TASKS.SET_PRIORITY( RTEMS.SELF, 254, OLD_PRIORITY, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY" );

      elsif ARGUMENT = 2 then

         TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

         TIME_TEST_SUPPORT.PUT_TIME( 
            "TASK_RESTART (ready, preempt)",
            TMTEST.END_TIME, 
            1, 
            0,
            RTEMS_CALLING_OVERHEAD.TASK_RESTART
         );

         RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE" );
 
      else

         RTEMS.SEMAPHORE.OBTAIN( 
            TMTEST.SEMAPHORE_ID, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            STATUS
         );

      end if;

   end HIGHEST_TASK;

-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      OLD_PRIORITY : RTEMS.TASKS.PRIORITY;
      OVERHEAD     : RTEMS.UNSIGNED32;
      NAME         : RTEMS.NAME;
      STATUS       : RTEMS.STATUS_CODES;
   begin
 
      TIMER_DRIVER.INITIALIZE;
         RTEMS.TASKS.RESTART( TMTEST.HIGHEST_ID, 1, STATUS );
      -- preempted by Higher_task
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESTART" );

      TIMER_DRIVER.INITIALIZE;
         RTEMS.TASKS.RESTART( TMTEST.HIGHEST_ID, 2, STATUS );
      -- preempted by Higher_task
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESTART" );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            TIMER_DRIVER.EMPTY_FUNCTION;
         end loop;
      OVERHEAD := TIMER_DRIVER.READ_TIMER;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.SEMAPHORE.RELEASE( TMTEST.SEMAPHORE_ID, STATUS );
         end loop;
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_RELEASE" );

      TIME_TEST_SUPPORT.PUT_TIME( 
         "SEMAPHORE_RELEASE (readying)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.SEMAPHORE_RELEASE
      );

      -- All low priority (non-preemptible) tasks are ready now.  We must
      -- prevent them from running (this would result in an invalid task exit),
      -- since the rtems_task_delete() performs an implicit join.
      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASKS.SUSPEND( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );
      end loop;

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASKS.DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE" );
      end loop;
     
      NAME := RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASKS.CREATE( 
               NAME,
               10, 
               1024, 
               RTEMS.NO_PREEMPT,
               RTEMS.DEFAULT_ATTRIBUTES,
               TMTEST.TASK_ID( INDEX ),
               STATUS
            );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_CREATE",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_CREATE
      );

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASKS.START( 
               TMTEST.TASK_ID( INDEX ),
               TMTEST.LOW_TASKS'ACCESS, 
               0, 
               STATUS
            );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_START",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_START
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASKS.DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE LOOP" );
      end loop;
     
      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASKS.CREATE( 
            NAME,
            250, 
            1024, 
            RTEMS.NO_PREEMPT,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );
         RTEMS.TASKS.START( 
            TMTEST.TASK_ID( INDEX ),
            TMTEST.RESTART_TASK'ACCESS, 
            0, 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );
         RTEMS.TASKS.SUSPEND( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND LOOP" );
      end loop;
     
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASKS.RESTART( TMTEST.TASK_ID( INDEX ), 0, STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESTART (suspended)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_RESTART
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASKS.SUSPEND( TMTEST.TASK_ID( INDEX ), STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND LOOP" );
      end loop;

      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASKS.DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_DELETE (suspended)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_DELETE
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASKS.CREATE( 
            NAME,
            250, 
            1024, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );
         RTEMS.TASKS.START( 
            TMTEST.TASK_ID( INDEX ),
            TMTEST.RESTART_TASK'ACCESS, 
            0, 
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );
      end loop;
     
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASKS.RESTART( TMTEST.TASK_ID( INDEX ), 1, STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESTART (ready)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_RESTART
      );

      for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop
         RTEMS.TASKS.SET_PRIORITY( 
            TMTEST.TASK_ID( INDEX ), 
            5, 
            OLD_PRIORITY, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SET_PRIORITY LOOP" );
      end loop; 

      -- yield processor -- tasks block
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
    
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASKS.RESTART( TMTEST.TASK_ID( INDEX ), 1, STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESTART (blocked, no preempt)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_RESTART
      );

      -- yield processor -- tasks block
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS ); 
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
    
      TIMER_DRIVER.INITIALIZE;
         for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
         loop
            RTEMS.TASKS.DELETE( TMTEST.TASK_ID( INDEX ), STATUS );
         end loop; 
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_DELETE (blocked)",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         OVERHEAD,
         RTEMS_CALLING_OVERHEAD.TASK_DELETE
      );

      TEST_SUPPORT.ADA_TEST_END;
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

-- 
--  LOW_TASKS
--

   procedure LOW_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      ID         : RTEMS.ID;
      STATUS     : RTEMS.STATUS_CODES;
   begin
   
      TMTEST.TASK_COUNT := TMTEST.TASK_COUNT - 1;

      if TMTEST.TASK_COUNT = 0 then

         RTEMS.TASKS.CREATE( 
            RTEMS.BUILD_NAME( 'H', 'I', ' ', ' ' ),
            5, 
            2048, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE HI" );

         RTEMS.TASKS.START( 
            ID, 
            TMTEST.HIGH_TASK'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START HI" );


         RTEMS.TASKS.CREATE( 
            RTEMS.BUILD_NAME( 'H', 'I', 'G', 'H' ),
            3, 
            2048, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.HIGHEST_ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE HIGH" );

         RTEMS.TASKS.START( 
            TMTEST.HIGHEST_ID, 
            TMTEST.HIGHEST_TASK'ACCESS, 
            0, 
            STATUS 
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START HIGH" );


      end if;

      RTEMS.SEMAPHORE.OBTAIN( 
         TMTEST.SEMAPHORE_ID, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.NO_TIMEOUT,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "SEMAPHORE_OBTAIN" );

   end LOW_TASKS;

-- 
--  RESTART_TASK
--

   procedure RESTART_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      STATUS   : RTEMS.STATUS_CODES;
   begin
   
      if ARGUMENT = 1 then
         RTEMS.SEMAPHORE.OBTAIN( 
            TMTEST.SEMAPHORE_ID, 
            RTEMS.DEFAULT_OPTIONS,
            RTEMS.NO_TIMEOUT,
            STATUS
         );
      end if;

   end RESTART_TASK;

end TMTEST;
