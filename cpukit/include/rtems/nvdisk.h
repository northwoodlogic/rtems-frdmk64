/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Non-volatile Disk Block Device Implementation
 */

/*
 * Copyright (C) 2007 Chris Johns
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

/**
 * The Non-volatile disk provides a simple directly mapped disk
 * driver with checksums for each. It is designed to provied a
 * disk that can survive a restart. Examples are EEPROM devices
 * which have byte writeable locations, or a battery backed up
 * RAM disk.
 *
 * The low level driver provides the physical access to the
 * hardware.
 */
#if !defined (_RTEMS_NVDISK_H_)
#define _RTEMS_NVDISK_H_

#include <stdint.h>
#include <sys/ioctl.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The base name of the nv disks.
 */
#define RTEMS_NVDISK_DEVICE_BASE_NAME "/dev/nvd"

/**
 * NV disk specific ioctl request types. To use open the
 * device and issue the ioctl call.
 *
 * @code
 *  int fd = open ("/dev/nvdisk0", O_WRONLY, 0);
 *  if (fd < 0)
 *  {
 *    printf ("driver open failed: %s\n", strerror (errno));
 *    exit (1);
 *  }
 *  if (ioctl (fd, RTEMS_NVDISK_IOCTL_ERASE_DISK) < 0)
 *  {
 *    printf ("driver erase failed: %s\n", strerror (errno));
 *    exit (1);
 *  }
 *  close (fd);
 * @endcode
 */
#define RTEMS_NVDISK_IOCTL_ERASE_DISK   _IO('B', 128)
#define RTEMS_NVDISK_IOCTL_MONITORING   _IO('B', 129)
#define RTEMS_NVDISK_IOCTL_INFO_LEVEL   _IO('B', 130)
#define RTEMS_NVDISK_IOCTL_PRINT_STATUS _IO('B', 131)

/**
 * NV Disk Monitoring Data allows a user to obtain
 * the current status of the disk.
 */
typedef struct rtems_nvdisk_monitor_data
{
  uint32_t block_size;
  uint32_t block_count;
  uint32_t page_count;
  uint32_t pages_available;
  uint32_t pages_used;
  uint32_t info_level;
} rtems_nvdisk_monitor_data;

/**
 * Return the number of kilo-bytes.
 */
#define RTEMS_NVDISK_KBYTES(_k) ((_k) * 1024)

/**
 * NV Low Level driver handlers.

 * Typically this structure is part of a table of handlers in the
 * device which is referenced in the nvdisk configuration table.
 * The reference is kept in the driver and used all the time to
 * manage the nv device, therefore it must always exist.
 */
typedef struct rtems_nvdisk_driver_handlers
{
  /**
   * Read data from the device into the buffer. Return an errno
   * error number if the data cannot be read.
   *
   * @param device The device to read data from.
   * @param flags Device specific flags for the driver.
   * @param base The base address of the device.
   * @param offset The offset in the segment to read.
   * @param buffer The buffer to read the data into.
   * @param size The amount of data to read.
   * @retval 0 No error.
   * @retval EIO The read did not complete.
   */
  int (*read) (uint32_t device, uint32_t flags, void* base,
               uint32_t offset, void* buffer, size_t size);

  /**
   * Write data from the buffer to the device. Return an errno
   * error number if the device cannot be written to.
   *
   * @param device The device to write data to.
   * @param flags Device specific flags for the driver.
   * @param base The base address of the device.
   * @param offset The offset in the device to write to.
   * @param buffer The buffer to write the data from.
   * @param size The amount of data to write.
   * @retval 0 No error.
   * @retval EIO The write did not complete or verify.
   */
  int (*write) (uint32_t device, uint32_t flags, void* base,
                uint32_t offset, const void* buffer, size_t size);

  /**
   * Verify data in the buffer to the data in the device. Return an
   * errno error number if the device cannot be read or the data verified.
   *
   * @param device The device to verify the data with.
   * @param flags Device specific flags for the driver.
   * @param base The base address of the device.
   * @param offset The offset in the device to verify.
   * @param buffer The buffer to verify the data in the device with.
   * @param size The amount of data to verify.
   * @retval 0 No error.
   * @retval EIO The data did not verify.
   */
  int (*verify) (uint32_t device, uint32_t flags, void* base,
                 uint32_t offset, const void* buffer, size_t size);

} rtems_nvdisk_driver_handlers;

/**
 * NV Device Descriptor holds the description of a device that is
 * part of the NV disk.
 *
 * Typically this structure is part of a table of the device which
 * is referenced in the nvdisk configuration table.
 * The reference is kept in the driver and used all the time to
 * manage the nv device, therefore it must always exist.
 */
typedef struct rtems_nvdisk_device_desc
{
  uint32_t                            flags;  /**< Private user flags. */
  void*                               base;   /**< Base address of the device. */
  uint32_t                            size;   /**< Size of the device. */
  const rtems_nvdisk_driver_handlers* nv_ops; /**< Device handlers. */
} rtems_nvdisk_device_desc;

/**
 * RTEMS Non-Volatile Disk configuration table used to initialise the
 * driver.
 */
typedef struct rtems_nvdisk_config
{
  uint32_t                        block_size;   /**< The block size. */
  uint32_t                        device_count; /**< The number of devices. */
  const rtems_nvdisk_device_desc* devices;      /**< The device descriptions. */
  uint32_t                        flags;        /**< Set of flags to control
                                                     driver. */
  uint32_t                        info_level;   /**< Default info level. */
} rtems_nvdisk_config;

/*
 * Driver flags.
 */

/**
 * Check the pages during initialisation to see which pages are
 * valid and which are not. This could slow down initialising the
 * disk driver.
 */
#define RTEMS_NVDISK_CHECK_PAGES (1 << 0)

/**
 * Non-volatile disk device driver initialization. Place in a table as the
 * initialisation entry and remainder of the entries are the RTEMS block
 * device generic handlers.
 *
 * @param major NV disk major device number.
 * @param minor Minor device number, not applicable.
 * @param arg Initialization argument, not applicable.
 * @return The rtems_device_driver is actually just
 *         rtems_status_code.
 */
rtems_device_driver
rtems_nvdisk_initialize (rtems_device_major_number major,
                         rtems_device_minor_number minor,
                         void*                     arg);

/**
 * External reference to the configuration. Please supply.
 * Support is present in confdefs.h for providing this variable.
 */
extern const rtems_nvdisk_config rtems_nvdisk_configuration[];

/**
 * External reference to the number of configurations. Please supply.
 * Support is present in confdefs.h for providing this variable.
 */
extern uint32_t rtems_nvdisk_configuration_size;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
