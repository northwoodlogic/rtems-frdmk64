/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function Creates New POSIX semaphore or Opens an existing Semaphore
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/posix/semaphoreimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/sysinit.h>

#include <stdarg.h>
#include <fcntl.h>
#include <limits.h>

static sem_t *_POSIX_Semaphore_Create_support(
  const char   *name_arg,
  size_t        name_len,
  unsigned int  value
)
{
  POSIX_Semaphore_Control *the_semaphore;
  char                    *name;

  if ( value > SEM_VALUE_MAX ) {
    rtems_set_errno_and_return_value( EINVAL, SEM_FAILED );
  }

  /*
   * Make a copy of the user's string for name just in case it was
   * dynamically constructed.
   */
  name = _Workspace_String_duplicate( name_arg, name_len );
  if ( name == NULL ) {
    rtems_set_errno_and_return_value( ENOMEM, SEM_FAILED );
  }

  the_semaphore = _POSIX_Semaphore_Allocate_unprotected();
  if ( the_semaphore == NULL ) {
    _Workspace_Free( name );
    rtems_set_errno_and_return_value( ENOSPC, SEM_FAILED );
  }

  the_semaphore->open_count = 1;
  the_semaphore->linked = true;

  /*
   *  POSIX does not appear to specify what the discipline for
   *  blocking tasks on this semaphore should be.  It could somehow
   *  be derived from the current scheduling policy.  One
   *  thing is certain, no matter what we decide, it won't be
   *  the same as  all other POSIX implementations. :)
   */
  _POSIX_Semaphore_Initialize( &the_semaphore->Semaphore, name, value );

  /*
   *  Make the semaphore available for use.
   */
  _Objects_Open_string(
    &_POSIX_Semaphore_Information,
    &the_semaphore->Object,
    name
  );

  return &the_semaphore->Semaphore;
}

/*
 *  sem_open
 *
 *  Opens a named semaphore.  Used in conjunction with the sem_close
 *  and sem_unlink commands.
 *
 *  11.2.3 Initialize/Open a Named Semaphore, P1003.1b-1993, p.221
 *
 *  NOTE: When oflag is O_CREAT, then optional third and fourth
 *        parameters must be present.
 */
sem_t *sem_open(
  const char *name,
  int         oflag,
  ...
  /* mode_t mode, */
  /* unsigned int value */
)
{
  /*
   * mode is set but never used. GCC gives a warning for this
   * and we need to tell GCC not to complain. But we have to
   * have it because we have to work through the variable
   * arguments to get to attr.
   */
  mode_t                     mode RTEMS_UNUSED;

  va_list                    arg;
  unsigned int               value = 0;
  POSIX_Semaphore_Control   *the_semaphore;
  size_t                     name_len;
  Objects_Get_by_name_error  error;
  sem_t                     *sem;

  if ( oflag & O_CREAT ) {
    va_start(arg, oflag);
    mode = va_arg( arg, mode_t );
    value = va_arg( arg, unsigned int );
    va_end(arg);
  }

  _Objects_Allocator_lock();
  the_semaphore = _POSIX_Semaphore_Get_by_name( name, &name_len, &error );

  /*
   *  If the name to id translation worked, then the semaphore exists
   *  and we can just return a pointer to the id.  Otherwise we may
   *  need to check to see if this is a "semaphore does not exist"
   *  or some other miscellaneous error on the name.
   */

  if ( the_semaphore == NULL ) {

    /*
     * Unless provided a valid name that did not already exist
     * and we are willing to create then it is an error.
     */

    if ( !( error == OBJECTS_GET_BY_NAME_NO_OBJECT && (oflag & O_CREAT) ) ) {
      _Objects_Allocator_unlock();
      rtems_set_errno_and_return_value(
        _POSIX_Get_by_name_error( error ),
        SEM_FAILED
      );
    }
  } else {

    /*
     * Check for existence with creation.
     */

    if ( (oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL) ) {
      _Objects_Allocator_unlock();
      rtems_set_errno_and_return_value( EEXIST, SEM_FAILED );
    }

    the_semaphore->open_count += 1;
    _Objects_Allocator_unlock();
    return &the_semaphore->Semaphore;
  }

  /*
   *  At this point, the semaphore does not exist and everything has been
   *  checked. We should go ahead and create a semaphore.
   */

  sem = _POSIX_Semaphore_Create_support(
    name,
    name_len,
    value
  );

  _Objects_Allocator_unlock();
  return sem;
}

static void _POSIX_Semaphore_Manager_initialization( void )
{
  _Objects_Initialize_information( &_POSIX_Semaphore_Information );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Semaphore_Manager_initialization,
  RTEMS_SYSINIT_POSIX_SEMAPHORE,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
