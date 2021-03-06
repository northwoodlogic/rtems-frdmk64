/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTaskMP
 *
 * @brief This source file contains the implementation to support the Task
 *   Manager in multiprocessing (MP) configurations.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/sysinit.h>

/**
 *  The following data structure defines the packet used to perform
 *  remote task operations.
 */
typedef struct {
  rtems_packet_prefix               Prefix;
  RTEMS_tasks_MP_Remote_operations  operation;
  rtems_name                        name;
  rtems_task_priority               the_priority;
}   RTEMS_tasks_MP_Packet;

RTEMS_STATIC_ASSERT(
  sizeof(RTEMS_tasks_MP_Packet) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  RTEMS_tasks_MP_Packet
);

static RTEMS_tasks_MP_Packet *_RTEMS_tasks_MP_Get_packet( void )
{
  return (RTEMS_tasks_MP_Packet *) _MPCI_Get_packet();
}

static RTEMS_tasks_MP_Packet *_RTEMS_tasks_MP_Get_request_packet(
  Objects_Id id
)
{
  if ( !_Thread_MP_Is_remote( id ) ) {
    return NULL;
  }

  return _RTEMS_tasks_MP_Get_packet();
}

/*
 *  _RTEMS_tasks_MP_Send_process_packet
 *
 */

void _RTEMS_tasks_MP_Send_process_packet (
  RTEMS_tasks_MP_Remote_operations operation,
  Objects_Id                       task_id,
  rtems_name                       name
)
{
  RTEMS_tasks_MP_Packet *the_packet;

  switch ( operation ) {

    case RTEMS_TASKS_MP_ANNOUNCE_CREATE:
    case RTEMS_TASKS_MP_ANNOUNCE_DELETE:

      the_packet                    = _RTEMS_tasks_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_TASKS;
      the_packet->Prefix.length     = sizeof ( RTEMS_tasks_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( RTEMS_tasks_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = task_id;
      the_packet->name              = name;

      _MPCI_Send_process_packet( MPCI_ALL_NODES, &the_packet->Prefix );
      break;

    case RTEMS_TASKS_MP_SUSPEND_REQUEST:
    case RTEMS_TASKS_MP_SUSPEND_RESPONSE:
    case RTEMS_TASKS_MP_RESUME_REQUEST:
    case RTEMS_TASKS_MP_RESUME_RESPONSE:
    case RTEMS_TASKS_MP_SET_PRIORITY_REQUEST:
    case RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE:
      break;
  }
}

static rtems_status_code _RTEMS_tasks_MP_Send_request_packet(
  RTEMS_tasks_MP_Packet            *the_packet,
  Objects_Id                        id,
  RTEMS_tasks_MP_Remote_operations  operation
)
{
  Status_Control status;

  the_packet->Prefix.the_class  = MP_PACKET_TASKS;
  the_packet->Prefix.length     = sizeof( *the_packet );
  the_packet->Prefix.to_convert = sizeof( *the_packet );
  the_packet->Prefix.id         = id;
  the_packet->operation         = operation;

  status = _MPCI_Send_request_packet(
    _Objects_Get_node( id ),
    &the_packet->Prefix,
    STATES_READY /* Not used */
  );
  return _Status_Get( status );
}

rtems_status_code _RTEMS_tasks_MP_Set_priority(
  rtems_id             id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
)
{
  RTEMS_tasks_MP_Packet *the_packet;

  the_packet = _RTEMS_tasks_MP_Get_request_packet( id );
  if ( the_packet == NULL ) {
    return RTEMS_INVALID_ID;
  }

  the_packet->the_priority = new_priority;
  _Thread_Executing->Wait.return_argument = old_priority;
  return _RTEMS_tasks_MP_Send_request_packet(
    the_packet,
    id,
    RTEMS_TASKS_MP_SET_PRIORITY_REQUEST
  );
}

rtems_status_code _RTEMS_tasks_MP_Suspend( rtems_id id )
{
  RTEMS_tasks_MP_Packet *the_packet;

  the_packet = _RTEMS_tasks_MP_Get_request_packet( id );
  if ( the_packet == NULL ) {
    return RTEMS_INVALID_ID;
  }

  return _RTEMS_tasks_MP_Send_request_packet(
    the_packet,
    id,
    RTEMS_TASKS_MP_SUSPEND_REQUEST
  );
}

rtems_status_code _RTEMS_tasks_MP_Resume( rtems_id id )
{
  RTEMS_tasks_MP_Packet *the_packet;

  the_packet = _RTEMS_tasks_MP_Get_request_packet( id );
  if ( the_packet == NULL ) {
    return RTEMS_INVALID_ID;
  }

  return _RTEMS_tasks_MP_Send_request_packet(
    the_packet,
    id,
    RTEMS_TASKS_MP_RESUME_REQUEST
  );
}

/*
 *  _RTEMS_tasks_MP_Send_response_packet
 *
 */

static void _RTEMS_tasks_MP_Send_response_packet (
  RTEMS_tasks_MP_Remote_operations  operation,
  Thread_Control                   *the_thread
)
{
  RTEMS_tasks_MP_Packet *the_packet;

  switch ( operation ) {

    case RTEMS_TASKS_MP_SUSPEND_RESPONSE:
    case RTEMS_TASKS_MP_RESUME_RESPONSE:
    case RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE:

      the_packet = (RTEMS_tasks_MP_Packet *) the_thread->receive_packet;

/*
 *  The packet being returned already contains the class, length, and
 *  to_convert fields, therefore they are not set in this routine.
 */
      the_packet->operation    = operation;
      the_packet->Prefix.id    = the_packet->Prefix.source_tid;

      _MPCI_Send_response_packet(
        _Objects_Get_node( the_packet->Prefix.source_tid ),
        &the_packet->Prefix
      );
      break;

    case RTEMS_TASKS_MP_ANNOUNCE_CREATE:
    case RTEMS_TASKS_MP_ANNOUNCE_DELETE:
    case RTEMS_TASKS_MP_SUSPEND_REQUEST:
    case RTEMS_TASKS_MP_RESUME_REQUEST:
    case RTEMS_TASKS_MP_SET_PRIORITY_REQUEST:
      break;

  }
}

/*
 *
 *  _RTEMS_tasks_MP_Process_packet
 *
 */

static void _RTEMS_tasks_MP_Process_packet (
  rtems_packet_prefix  *the_packet_prefix
)
{
  RTEMS_tasks_MP_Packet *the_packet;
  Thread_Control   *the_thread;

  the_packet = (RTEMS_tasks_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case RTEMS_TASKS_MP_ANNOUNCE_CREATE:

      _Objects_MP_Allocate_and_open(
        &_RTEMS_tasks_Information.Objects,
        the_packet->name,
        the_packet->Prefix.id,
        true
      );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case RTEMS_TASKS_MP_ANNOUNCE_DELETE:

      _Objects_MP_Close(
        &_RTEMS_tasks_Information.Objects,
        the_packet->Prefix.id
      );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case RTEMS_TASKS_MP_SUSPEND_REQUEST:

      the_packet->Prefix.return_code = rtems_task_suspend(
        the_packet->Prefix.id
      );

      _RTEMS_tasks_MP_Send_response_packet(
        RTEMS_TASKS_MP_SUSPEND_RESPONSE,
        _Thread_Executing
      );
      break;

    case RTEMS_TASKS_MP_SUSPEND_RESPONSE:
    case RTEMS_TASKS_MP_RESUME_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case RTEMS_TASKS_MP_RESUME_REQUEST:

      the_packet->Prefix.return_code = rtems_task_resume(
        the_packet->Prefix.id
      );

      _RTEMS_tasks_MP_Send_response_packet(
        RTEMS_TASKS_MP_RESUME_RESPONSE,
        _Thread_Executing
      );
      break;

    case RTEMS_TASKS_MP_SET_PRIORITY_REQUEST:

      the_packet->Prefix.return_code = rtems_task_set_priority(
        the_packet->Prefix.id,
        the_packet->the_priority,
        &the_packet->the_priority
      );

      _RTEMS_tasks_MP_Send_response_packet(
        RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE,
        _Thread_Executing
      );
      break;

    case RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      *(rtems_task_priority *)the_thread->Wait.return_argument =
                                               the_packet->the_priority;

      _MPCI_Return_packet( the_packet_prefix );
      break;
  }
}

static void _RTEMS_tasks_MP_Initialize( void )
{
  _MPCI_Register_packet_processor(
    MP_PACKET_TASKS,
    _RTEMS_tasks_MP_Process_packet
  );
}

RTEMS_SYSINIT_ITEM(
  _RTEMS_tasks_MP_Initialize,
  RTEMS_SYSINIT_CLASSIC_TASKS_MP,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
