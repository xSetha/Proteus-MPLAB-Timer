#include <avr/io.h>
#include <avr/interrupt.h>
#include "Buttons.h"

volatile enum BtnFSMStates BtnFSMState[NumButtons];

volatile enum BtnStates BtnState[NumButtons];

volatile enum BtnFlagTypes BtnFlags[NumButtons];

volatile unsigned short BTNTimeCount[NumButtons];


uint8_t  * BTN_PIN_REGS[NumButtons]={(uint8_t *)_SFR_ADDR(PINC)};

const uint8_t  BTN_PINS[NumButtons] ={PINC0};





ISR(PCINT1_vect){
enum Buttons Btn;
    for (Btn = StartStop; Btn < NumButtons; Btn++){
        if (BTNTimeCount[Btn] >= STABLE_TIME_TICKS){
            if ( (BtnState[Btn] == Released) &&
                 (bit_is_set_onAddr(BTN_PIN_REGS[Btn], BTN_PINS[Btn])) ){
                BtnState[Btn]=Pressed;
                BTNTimeCount[Btn]=0;
            }
            if ( (BtnState[Btn] == Pressed) &&
                 (bit_is_clear_onAddr(BTN_PIN_REGS[Btn], BTN_PINS[Btn])) ){
                BtnState[Btn]=Released;
                BTNTimeCount[Btn]=0;
            }
        }
    }
}


void BtnFSM(enum Buttons Btn){
    switch (BtnFSMState[Btn]){
    case stIdle: if ( BtnState[Btn] == Pressed ) BtnFSMState[Btn] = stPressed;
                   break;
    case stPressed: if ( BtnState[Btn] == Released ) BtnFSMState[Btn] = stClick;
                    else if (BTNTimeCount[Btn] >= LONGPRESS_TICKS) {
                            BtnFlags[Btn] = LongPress;   
                            BtnFSMState[Btn] = stLongPress;
                        }
                    break;
    case stClick: BtnFlags[Btn] = Click;
                  BtnFSMState[Btn] = stIdle;
                  break;
    case stLongPress: if ( BtnState[Btn] == Released ) BtnFSMState[Btn] = stIdle;
                       break;
    default : break;
    } 
}

void Init_Buttons(){
    enum Buttons Btn;
    //Initializam toate variabilele legate de butoane
    for (Btn = StartStop; Btn < NumButtons; Btn++){
        BtnFSMState[Btn]= stIdle;
        BtnState[Btn]=Released;
        BtnFlags[Btn]=Idle;
        BTNTimeCount[Btn]=0;
    } 
    
    BitSet(PCMSK1, PCINT8); //PCINT8 = PC0, StartStop

    
    BitSet(PCICR, PCIE1);
    
}


