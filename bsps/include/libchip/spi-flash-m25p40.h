/*
 * SPI driver for M25P40 like spi flash device
 *
 * FIXME: currently, this driver only supports read/write accesses
 * erase accesses are to be completed
 */

/*
 * Copyright (c) 2007 embedded brains GmbH. All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _LIBCHIP_SPI_FLASH_M25P40_H
#define _LIBCHIP_SPI_FLASH_M25P40_H

#include <libchip/spi-memdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * pass one of these descriptor pointers to rtems_libi2c_register_drv
 */
extern rtems_libi2c_drv_t *spi_flash_m25p40_rw_driver_descriptor;

extern rtems_libi2c_drv_t *spi_flash_m25p40_ro_driver_descriptor;

#ifdef __cplusplus
}
#endif

#endif /* _LIBCHIP_SPI_FLASH_M25P40_H */
