#include <bsp/flashconfig.h>

//
//*****************************************************************************
// Flash Configuration block : 16-byte flash configuration field that stores
// default protection settings (loaded on reset) and security information that
// allows the MCU to restrict access to the Flash Memory module.
// Placed at address 0x400 by the linker script.
//
// FOPT=0xF9 --> Disable NMI, Disable EzPort
// FSEC=0xFE
//*****************************************************************************
__attribute__ ((section(".FlashConfig")))
const struct FlashConfig FlashConfig = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFF9FE};
