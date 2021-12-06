#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "Uno_Nano_utils.h"
#include "ssd.h"
#include "Buttons.h"

#define TIMEBASE_TICKS_MS 1000
#define FIVE_SEC_DELAY_MS 5000

/*
 * Button elements
 */
extern enum BtnFSMStates BtnFSMState[NumButtons];
extern enum BtnFlagTypes BtnFlags[NumButtons];
extern unsigned short BTNTimeCount[NumButtons];

//TimebaseCount will count more than Timebase Ticks
//to generate BLINK period too
volatile unsigned short TimebaseCount=0;
volatile unsigned short DelayCount = 0;
//Flag for TimerBase, meaning that 1/100 Second passed
volatile uint8_t Timebase_Tick = 0;
volatile uint8_t FiveSecDelay = 0;

/*
 * Time for countdown timer will be stored in  an integerthe Chrono structure
 */
volatile uint8_t CdTimer; 

/*
 * CdTimer  State Elements
 */
enum CdTimerStates{stReset, stDelay, stRun, stAlarm, stRestart};
volatile enum CdTimerStates CdTimerState = stReset;

void CdTimerFSM();

//Elementele pentru alarma
#define SOUND_TIME_PERIOD_MS 1000 //periood of the sound generated
#define SHORT_PAUSE_PERIOD_MS 50
#define LONG_PAUSE_START_MS 500

//AlarmTimeCount for Alarm period
volatile unsigned short AlarmTimeCount=0;
//Flags for Sound Period, short and long pause period 
volatile uint8_t SoundPeriod_Tick = 0;
volatile uint8_t ShortPausePeriod_Tick = 0;
volatile uint8_t LongPausePeriod_Tick = 0;

enum AlarmStates{stStop, stSound, stShortPause, stLongPause};
volatile enum AlarmStates AlarmState = stStop;

void AlarmFSM();

void ConfigureandStartTimer1();
void ConfigureTimer2();

void FormatAndDisplay(uint8_t Numar);

int main(void) {

   Init_Buttons();
   init_Display();
   DisplayOn();

   TurnOffDigit(2);
   TurnOffDigit(3);
   
   //Configurarea timerului/pinului pentru alarma
   ConfigureTimer2();
    BitSet(DDRB, DDB3);  //OC2A: Output
    BitClear(PORTB, PORTB3);

   ConfigureandStartTimer1();

   sei();
   
   CdTimer=90;
     FormatAndDisplay(CdTimer);   
while(1) {

     if ((BtnFlags[StartStop] == Click) && (CdTimerState == stDelay)){
	if(CdTimer ==90 ){
		       CdTimer = 60;
			FormatAndDisplay(CdTimer);
			DelayCount = 0;
			Timebase_Tick = 0;
			  FiveSecDelay = 0;
		       BtnFlags[StartStop] = Idle;
		       }
		       else if(CdTimer == 60){
		       CdTimer = 45;
			FormatAndDisplay(CdTimer);
			  DelayCount = 0;
			   Timebase_Tick = 0;
			   FiveSecDelay = 0;
		       BtnFlags[StartStop] = Idle;
		       }
		        else if(CdTimer == 45){
		       CdTimer = 30;
			FormatAndDisplay(CdTimer);
			   DelayCount = 0;
			   Timebase_Tick = 0;
			   FiveSecDelay = 0;
		       BtnFlags[StartStop] = Idle;
		       }
		        else if(CdTimer == 30){
			CdTimer = 15;
			   FormatAndDisplay(CdTimer);
			   DelayCount = 0;
			   Timebase_Tick = 0;
			    FiveSecDelay = 0;
		       BtnFlags[StartStop] = Idle;
		       }
		       else {
			  CdTimer = 0;
			  FormatAndDisplay(CdTimer);
			  DelayCount = 0;
			   Timebase_Tick = 0;
			   FiveSecDelay = 0;
			  BtnFlags[StartStop] = Idle;
			  CdTimerState = stRestart;
		       }
      }
   
    if ( (Timebase_Tick) && (CdTimerState == stRun) ){
           if (CdTimer >0) CdTimer--; 
           FormatAndDisplay(CdTimer);
           Timebase_Tick = 0;
       }
}

}


ISR (TIMER1_COMPA_vect){
      enum Buttons Btn;
   
      TimebaseCount++;
      DelayCount++;
   
    //Setam Flag-ul Timebase_Tick
    if (TimebaseCount >= TIMEBASE_TICKS_MS){
        TimebaseCount = 0;
        Timebase_Tick =1;
    }
    
     if(DelayCount >= FIVE_SEC_DELAY_MS){
       DelayCount = 0;
      FiveSecDelay = 1;
       }
       
       for (Btn = StartStop; Btn < NumButtons; Btn++){
        BTNTimeCount[Btn]++;
       BtnFSM(Btn);
    }
    
    AlarmTimeCount++;
    if ((AlarmTimeCount % SHORT_PAUSE_PERIOD_MS) == 0){
        ShortPausePeriod_Tick = 1;
    }
    if (AlarmTimeCount == LONG_PAUSE_START_MS){
        LongPausePeriod_Tick = 1;
    }
    
    if (AlarmTimeCount >= SOUND_TIME_PERIOD_MS){
        AlarmTimeCount = 0;
        SoundPeriod_Tick =1;
    } 
    
    CdTimerFSM();
    AlarmFSM();
    
}

void ConfigureandStartTimer1(){
//Stergem configuratiile si resetam Timer
TCCR1A = 0;
TCCR1B = 0;
TCNT1 = 0; 
//Setam Perioada
//F_CPU = 1MHz, avem nevoie de 1KHz => divizare de 1000
OCR1A = 999;

//Modul de lucru: CTC
BitSet(TCCR1B, WGM12);

// Valideaza Timer1 Interrupt pe Compare Match
BitSet(TIMSK1, OCIE1A);

//Alegem Prescaler=1 si pornim Timer
BitSet(TCCR1B, CS10);
}

void ConfigureTimer2(){
//Stergem configuratiile si resetam Timer
TCCR2A = 0;
TCCR2B = 0;
TCNT2 = 0; 
//Setam Perioada
//F_CPU = 8MHz. Vrem FOCR = 2KHz
//Folosim OC0A in Toggle Mode, rezulta Ftimer = 4KHz
//Pentru 4KHz avem nevoie de divizare de 2000
//Folosim Prescaler de 64, ramane OCR0A + 1 = 31.25
//Atunci 8000000/(64*31) = 4032Hz
OCR2A = 30;

//OC0A Toggle on Compare Match
//Atunci frecventa OCR0A va fi de 1/2 Frecventa Timer
BitSet(TCCR2A, COM2A0);


//Modul de lucru: CTC
BitSet(TCCR2A, WGM21);

}

void CdTimerFSM(){
    switch (CdTimerState){
    case stReset: //Initialize CdTimer
             CdTimer = 90;
		     FormatAndDisplay(CdTimer);
		     DelayCount = 0;                //Resetare flag si counter delay
		     FiveSecDelay = 0;
		     if(BtnFlags[StartStop] == Click){
		    CdTimerState = stDelay;
		    Timebase_Tick = 0;
		    BtnFlags[StartStop] = Idle;
		     }
		     break;
case stDelay:		   
			if(FiveSecDelay){
            Timebase_Tick = 0;
			FiveSecDelay = 0;
			BtnFlags[StartStop] = Idle;
			CdTimerState = stRun;
			   }
                  break;
    case stRun:  if ( BtnFlags[StartStop] == LongPress ){
                        CdTimerState = stReset;
                        BtnFlags[StartStop] = Idle;
                    }
                 else if (CdTimer == 0){
                     CdTimerState = stAlarm;
                 }
                  break;
   case stRestart: if(BtnFlags[StartStop] == Click){
			CdTimerState = stReset;
		     BtnFlags[StartStop] = Idle;
		  }
		  break;
case stAlarm:  if ( BtnFlags[StartStop] == LongPress ){
                        CdTimerState = stReset;
                        BtnFlags[StartStop] = Idle;
                    }
                  break;
    default : break;
    }
}

void AlarmFSM(){
    switch (AlarmState){
    case stStop: //Timer Stopped, sound off
                BitClear(TCCR2B, CS01); 
                BitClear(TCCR2B, CS00);
                BitClear(TCCR2A, COM2A0);
                if ( CdTimerState == stAlarm ){
                    AlarmState = stSound;
                    }
                  break;
    case stSound: //Timer Started, sound is on 
                BitSet(TCCR2B, CS21); 
                BitSet(TCCR2B, CS20);
                BitSet(TCCR2A, COM2A0);//OC0A: Toggle
                if ( CdTimerState != stAlarm){
                        AlarmState = stStop;
                    }
                 else if (ShortPausePeriod_Tick){
                     AlarmState = stShortPause;
                     ShortPausePeriod_Tick = 0;
                 }
                 else if (LongPausePeriod_Tick){
                     AlarmState = stLongPause;
                     LongPausePeriod_Tick = 0;
                 }
                break;
    case stShortPause: BitClear(TCCR2B, CS21); //Sound off
                       BitClear(TCCR2B, CS20);
                       BitClear(TCCR2A, COM2A0);//Cand e oprit, sa avem OC0A = 0;
                if ( CdTimerState != stAlarm){
                        AlarmState = stStop;
                    }
                 else if (ShortPausePeriod_Tick){
                     AlarmState = stSound;
                     ShortPausePeriod_Tick = 0;
                 }
                 else if (LongPausePeriod_Tick){
                     AlarmState = stLongPause;
                     LongPausePeriod_Tick = 0;
                 }
                break;
     case stLongPause: BitClear(TCCR2B, CS21); //Sound off
                       BitClear(TCCR2B, CS20);
                       BitClear(TCCR2A, COM2A0);//Cand e oprit, sa avem OC0A = 0;
                if ( CdTimerState != stAlarm){
                        AlarmState = stStop;
                    }
                 //Ne intoarcem in Sound, doar la sfarsitul
                 //perioadei
                 else if (SoundPeriod_Tick){
                     AlarmState = stSound;
                     SoundPeriod_Tick = 0;
                 }
                break;           
    default : break;
    }
}

void FormatAndDisplay(uint8_t Numar){
        
   if (Numar <10)  TurnOffDigit(1);
   else  TurnOnDigit(1);
    DisplayRawNumber(Format4DigitBCDNumber(Numar), 0);
    
}

