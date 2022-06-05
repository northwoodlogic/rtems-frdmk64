/**
 * @file
 * @ingroup mk64fn_usart
 * @brief USART (universal synchronous/asynchronous receiver/transmitter) support.
 */

/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_MK64FN_USART_H
#define LIBBSP_ARM_MK64FN_USART_H

#include <libchip/serial.h>

/**
 * @defgroup mk64fn_usart USART Support
 * @ingroup RTEMSBSPsARMMK64FN
 * @brief USART Support
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* TODO: implement the console driver */
extern const console_fns mk64f12_usart_fns;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_MK64FN_USART_H */
