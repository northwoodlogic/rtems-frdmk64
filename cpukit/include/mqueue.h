/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief POSIX Message Queues
 * 
 * This file contains the definitions related to POSIX Message Queues.
 * 
 * The structure of the routines is identical to that of POSIX
 * Message_queues to leave the option of having unnamed message
 * queues at a future date.  They are currently not part of the
 * POSIX standard but unnamed message_queues are.  This is also
 * the reason for the apparently unnecessary tracking of
 * the process_shared attribute.  [In addition to the fact that
 * it would be trivial to add pshared to the mq_attr structure
 * and have process private message queues.]
 *
 * This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 * time.
 */

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

#ifndef _MQUEUE_H
#define _MQUEUE_H


#include <unistd.h>

#if defined(_POSIX_MESSAGE_PASSING)

#include <sys/types.h>

/**
 * @defgroup POSIX_MQUEUE POSIX Message Queues
 *
 * @ingroup POSIXAPI
 * 
 */
/**@{**/

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  15.1.1 Data Structures, P1003.1b-1993, p. 271
 */

/**
 * Message queue id type.
 *
 * NOTE: Use uint32_t since all POSIX Ids are 32-bit currently.
 */
typedef uint32_t  mqd_t;

/**
 * This is the message queue attributes structure.
 */
struct mq_attr {
  /** This is the message queue flags */
  long  mq_flags;
  /** This is the maximum number of messages */
  long  mq_maxmsg;
  /** This is the maximum message size */
  long  mq_msgsize;
  /** This is the mumber of messages currently queued */
  long  mq_curmsgs;
};

/**
 * 15.2.2 Open a Message Queue, P1003.1b-1993, p. 272
 */
mqd_t mq_open(
  const char *name,
  int         oflag,
  ...
);

/**
 * 15.2.2 Close a Message Queue, P1003.1b-1993, p. 275
 */
int mq_close(
  mqd_t  mqdes
);

/**
 * @brief Remove a message queue.
 *
 * 15.2.2 Remove a Message Queue, P1003.1b-1993, p. 276
 *
 * NOTE:  The structure of the routines is identical to that of POSIX
 *        Message_queues to leave the option of having unnamed message
 *        queues at a future date.  They are currently not part of the
 *        POSIX standard but unnamed message_queues are.  This is also
 *        the reason for the apparently unnecessary tracking of
 *        the process_shared attribute.  [In addition to the fact that
 *        it would be trivial to add pshared to the mq_attr structure
 *        and have process private message queues.]
 *
 *        This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 *        time.
 */
int mq_unlink(
  const char *name
);

/**
 * 15.2.4 Send a Message to a Message Queue, P1003.1b-1993, p. 277
 *
 * NOTE; P1003.4b/D8, p. 45 adds mq_timedsend().
 */
int mq_send(
  mqd_t         mqdes,
  const char   *msg_ptr,
  size_t        msg_len,
  unsigned int  msg_prio
);

#if defined(_POSIX_TIMEOUTS)

#include <time.h>

/**
 * @brief Send a message to a message queue.
 * 
 * @see mq_send()
 */
int mq_timedsend(
  mqd_t                  mqdes,
  const char            *msg_ptr,
  size_t                 msg_len,
  unsigned int           msg_prio,
  const struct timespec *abstime
);

#endif /* _POSIX_TIMEOUTS */

/**
 * @brief Receive a message from a message queue. 
 * 
 * 15.2.5 Receive a Message From a Message Queue, P1003.1b-1993, p. 279
 *
 * NOTE: P1003.4b/D8, p. 45 adds mq_timedreceive().
 */
ssize_t mq_receive(
  mqd_t         mqdes,
  char         *msg_ptr,
  size_t        msg_len,
  unsigned int *msg_prio
);

#if defined(_POSIX_TIMEOUTS)

ssize_t mq_timedreceive(
  mqd_t                  mqdes,
  char                  *__restrict msg_ptr,
  size_t                 msg_len,
  unsigned int          *__restrict msg_prio,
  const struct timespec *__restrict abstime
);

#endif /* _POSIX_TIMEOUTS */

#if defined(_POSIX_REALTIME_SIGNALS)

/**
 * @brief Notify process that a message is available on a queue.
 *
 * 15.2.6 Notify Process that a Message is Available on a Queue,
 *        P1003.1b-1993, p. 280
 */
int mq_notify(
  mqd_t                  mqdes,
  const struct sigevent *notification
);

#endif /* _POSIX_REALTIME_SIGNALS */

/**
 * @brief Set message queue attributes.
 *
 * 15.2.7 Set Message Queue Attributes, P1003.1b-1993, p. 281
 */
int mq_setattr(
  mqd_t                 mqdes,
  const struct mq_attr *__restrict mqstat,
  struct mq_attr       *__restrict omqstat
);

/*
 *  15.2.8 Get Message Queue Attributes, P1003.1b-1993, p. 283
 */

int mq_getattr(
  mqd_t           mqdes,
  struct mq_attr *mqstat
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_MESSAGE_PASSING */

#endif
/* end of include file */
