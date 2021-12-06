/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Digilent

  @File Name
    utils.h

  @Description
    This file groups the declarations of the functions that implement
        the utils library (defined in utils.c).
        Include the file in the project when this library is needed.
        Use #include "utils.h" in the source files where the functions are needed.
 */
/* ************************************************************************** */

#ifndef _UNO_NANO_UTILS_H    /* Guard against multiple inclusion */
#define _UNO_NANO_UTILS_H

#include <avr/io.h>

#define BitSet(sfr, bit) _SFR_BYTE(sfr) = _SFR_BYTE(sfr)| _BV(bit)
#define BitClear(sfr, bit) _SFR_BYTE(sfr) = _SFR_BYTE(sfr)& (~(_BV(bit)))

#define BitSet_onAddr(sfr_addr, bit) _MMIO_BYTE(sfr_addr) = _MMIO_BYTE(sfr_addr)| _BV(bit)
#define BitClear_onAddr(sfr_addr, bit) _MMIO_BYTE(sfr_addr) = _MMIO_BYTE(sfr_addr) & (~(_BV(bit)))

#define bit_is_set_onAddr(sfr_addr, bit) (_MMIO_BYTE(sfr_addr) & _BV(bit))

#define bit_is_clear_onAddr(sfr_addr, bit) (!(_MMIO_BYTE(sfr_addr) & _BV(bit)))

void DelayAprox10Us( uint32_t t10usDelay );

#endif /* _UTILS_H */

/* *****************************************************************************
 End of File
 */
