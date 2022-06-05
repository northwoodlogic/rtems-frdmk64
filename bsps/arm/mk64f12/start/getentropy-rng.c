#include <bsp.h>
#include <bsp/mk64f12.h>
#include <rtems.h>
#include <rtems/sysinit.h>

#include <sys/param.h>
#include <unistd.h>
#include <string.h>

static void mk64f12_rng_enable(void)
{
  RNGA_Init(RNG);
}

int getentropy(void *ptr, size_t n)
{
  RNGA_GetRandomData(RNG, ptr, n);
  return 0;
}

RTEMS_SYSINIT_ITEM(
  mk64f12_rng_enable,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
