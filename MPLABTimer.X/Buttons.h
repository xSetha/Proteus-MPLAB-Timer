// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef BUTTONS_H
#define	BUTTONS_H

#include "Uno_Nano_utils.h"

#ifdef SIM_MPIDE
#define STABLE_TIME_TICKS 2
#define LONGPRESS_TICKS 10
#else
#define STABLE_TIME_TICKS 2
#define LONGPRESS_TICKS 1500
#endif


enum Buttons {StartStop=0, NumButtons};

enum BtnFSMStates{stIdle, stPressed, stClick, stLongPress};

enum BtnStates{Released, Pressed};

enum BtnFlagTypes {Idle = 0, Click, LongPress};


void BtnFSM(enum Buttons Btn);
void Init_Buttons();





#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

