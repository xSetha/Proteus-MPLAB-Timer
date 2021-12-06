/* ************************************************************************** */
/** Descriptive File Name

  @Company
    UTCN

  @File Name
    Uno_Nano_utils.c

  @Description
        This library implements the delay functionality used in other libraries.  
        The delay is implemented using loop, so the delay time is not exact. 
        For exact timing use timers.
        Include the file in the project, together with utils.h, when this library is needed	
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */

#include "Uno_Nano_utils.h"
/* ************************************************************************** */

/* ------------------------------------------------------------ */
/***    DelayAprox10Us
**
**	Synopsis:
**		DelayAprox10Us(100)
**
**	Parameters:
**		t10usDelay - the amount of time you wish to delay in tens of microseconds
**
**	Return Values:
**      none
**
**	Errors:
**		none
**
**	Description:
**		This procedure delays program execution for the specified number
**      of microseconds. This delay is not precise.
**		
**	Note:
**		This routine was determined experimentally. Assuming that the clock frequency is 16MHz
*/
void DelayAprox10Us( uint32_t t10usDelay )
{
    int j;
    while ( 0 < t10usDelay )
    {
        t10usDelay--;
        j = 5;
        while ( 0 < j )
        {
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
        asm volatile("nop"); // do nothing
         j--;
        }   // end while
         
    }   // end while
}

/* *****************************************************************************
 End of File
 */
