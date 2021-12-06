
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#include "ssd.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Uno_Nano_utils.h" 

/* ************************************************************************** */
//Sirul care contine decodificarea HEX-7 Segmente
// Bit 0 - Catod A: 0 pentru a aprinde, 1 pentru a stinge
// Bit 1 - Catod B: 0 pentru a aprinde, 1 pentru a stinge
// Bit 2 - Catod C: 0 pentru a aprinde, 1 pentru a stinge
// Bit 3 - Catod D: 0 pentru a aprinde, 1 pentru a stinge
// Bit 4 - Catod E: 0 pentru a aprinde, 1 pentru a stinge
// Bit 5 - Catod F: 0 pentru a aprinde, 1 pentru a stinge
// Bit 6 - Catod G: 0 pentru a aprinde, 1 pentru a stinge
// Bit 7 - Catod DP - punctul zecimal, va fi tratat separat
const unsigned char digitSegments[]= {
    0b1000000, // 0
    0b1111001, // 1
    0b0100100, // 2
    0b0110000, // 3
    0b0011001, // 4
    0b0010010, // 5
    0b0000010, // 6
    0b1111000, // 7
    0b0000000, // 8
    0b0010000, // 9
    0b0001000, // A
    0b0000011, // b
    0b1000110, // C
    0b0100001, // d
    0b0000110, // E
    0b0001110, // F
    0b0001001  // H
};

/* ************************************************************************** *

//Variabile globale pentru afisajul de 7 Segmente

* ************************************************************************** */
//valoarea celor 4 digiti, care vor fi afisati
volatile uint8_t digits[4];
//Contor pentru digit curent in intreruperea Timer
volatile uint8_t CurrentDigitIndex = 0;

//1 logic pe bitii [3:0] ai DecimalPoint 
//aprinde punctul zecimal pe digitul respectiv
//exemplu: 0b0010 aprinde DP pe Digit 1
//0b0110 aprinde DP pe Digit 1 si 2
volatile uint8_t DecimalPoint;

//1 logic pe bitii [3:0] ai BlankDigit stinge digitul respectiv
//exemplu: 0b1000 stinge digitul cel mai semnificativ,
//0b1001 stinge digitii 3 si 0 samd.
volatile uint8_t BlankDigit = 0;

//Pentru a genera o pauza intre aprinderea,
//pe rand, a anozilor. Trebuie doar in simularea Proteus!
volatile uint8_t AnodePause = 0;
//Endisplay = 0 stinge, 1 aprinde afisajul
volatile uint8_t EnDisplay = 0;


/* ************************************************************************** *

Corpurile functiilor

* ************************************************************************** */

/* ************************************************************************** *
 * void init_Display()
 * Configurarea iesirilor si a Timer-ului
 * ************************************************************************** */
void init_Display(){
//Alocarea pinilor in aceasta biblioteca
//Segmenti
// DP  G   F   E   D   C   B   A
//PC1 PB1 PB3 PB2 PD7 PD6 PD5 PD4
//Anozi
//AN3 AN2 AN1 AN0
//PC2 PC3 PC4 PC5
    
//Setam pinii mentionati ca iesire    
//PORTC  bitii 5..1 - iesiri
   DDRC = DDRC | (1<<DDC5) | (1<<DDC4) 
		       | (1<<DDC3) | (1<<DDC2) 
               | (1<<DDC1);
//Intial, 0 logic pe anozi
//si 1 logic pe DP, 
//ca la inceput sa avem segmentii stinsi
   PORTC = PORTC & 0xC3;//0xC3 = 0b11000011
   PORTC = PORTC | (1<<PORTC1);   
   
//PORTB bitii 3, 2, 1, - iesiri
   DDRB = DDRB | (1<<DDB0) | (1<<DDB2) 
               | (1<<DDB1);
//initial: 1 logic, ca segmentii sa fie stinsi
   PORTB = PORTB | (1<<PORTB0) | (1<<PORTB2) 
                 | (1<<PORTB1);

 //PORTD bitii 7..4 - iesiri
   DDRD = DDRD | 0xF0;
//initial: 1 logic, ca segmentii sa fie stinsi
   PORTD = PORTD | 0xF0;

//Configuram Timer0 pentru a genera intreruperi
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0; 
// Valideaza Timer1 Interrupt pe OverFlow
BitSet(TIMSK0, TOIE0);


//Prescaler = 8 - rezulta intreruperi Timer 
//(in simulare Proteus) la 2.048ms = 488Hz
BitSet(TCCR0B, CS01);


sei();

}


/*******************************************************************
 Intreruperea Timer0, care asigura Refresh la digiti
 *******************************************************************/
ISR (TIMER0_OVF_vect){
 uint8_t index, CurrentDigit;

 
if (EnDisplay)   {//Aprindem afisajul, doar daca EnDisplay = 1
    if (!AnodePause){//Pentru a insera o pauza - necesar in simularea
                     //Proteus

       CurrentDigitIndex++;
       index = CurrentDigitIndex & 0x03;
       CurrentDigit = digits[index];
      //Trimitem pe catozi codul Seven Segment al digit-ului curent 
       DisplaySingleDigit(CurrentDigit, DecimalPoint & (1 << index));

       //Walk1 pe anozi
       //Stingem toti digiti, apoi aprindem digitul curent
       PORTC = PORTC & 0xC3;
       switch(index){
          //Alocarea anozilor: //AN3 AN2 AN1 AN0
                               //PC2 PC3 PC4 PC5
          //Daca bitul corespunzator pe BlankDigit e 1, aanodul ramane stins
          case 0: PORTC = (BlankDigit & 0x01)? PORTC & (~(1<< PORTC5)): PORTC | (1<< PORTC5) ; break;
          case 1: PORTC = (BlankDigit & 0x02)? PORTC & (~(1<< PORTC4)): PORTC | (1<< PORTC4) ; break;
          case 2: PORTC = (BlankDigit & 0x04)? PORTC & (~(1<< PORTC3)): PORTC | (1<< PORTC3) ; break;
          case 3: PORTC = (BlankDigit & 0x08)? PORTC & (~(1<< PORTC2)): PORTC | (1<< PORTC2) ; break;
       }
   
       AnodePause = 1;
     }

    else{
          AnodePause = 0;
        PORTC = PORTC & 0xC3;
        }
     }
 }
/* ************************************************************************** *
 * void DisplaySingleDigit(uint8_t Number, uint8_t DP){
 * Decodifica si trimite codul de 7 segmenti pe catozi
 * Argumente:
 *  Number: de la 0 la 0xF, care va fi cofdificat in 7 segmente
 *  DP: pentru 1 aprinde, pentru 0 stinge puctul zecimal
* ************************************************************************** */
void DisplaySingleDigit(uint8_t Number, uint8_t DP){

//Alocarea segmentilor in aceasta biblioteca
// DP  G   F   E   D   C   B   A
//PC1 PB1 PB3 PB2 PD7 PD6 PD5 PD4

 //Number va fi index in sirul digitSegments[]
 //Nu are voie, sa depaseasca 15
 //Cel mai simplu: Mascam Number cu 0x0F
 uint8_t SegData =  digitSegments[Number & 0x0F];
   
 //Segmentul este catod: aprins, cand semnalul de iesire e 0
 //Rezulta: Daca bitul corespunzator e 0, trimitem pe port 0  
 //Segment G : PB1
 PORTB = (SegData & 0x40)? PORTB | (1<<PORTB1) : PORTB &(~(1<<PORTB1));
 //Segment F: PB3
 PORTB = (SegData & 0x20)? PORTB | (1<<PORTB0) : PORTB &(~(1<<PORTB0));
//Segment E : PB2
 PORTB = (SegData & 0x10)? PORTB | (1<<PORTB2) : PORTB &(~(1<<PORTB2));
   
//Ultimii 4 segmenti - D..A sunt conectati la biti consecutivi
//Mai exact la PD7..4 
//Atunci pot fi alocati in grup
 PORTD =  (PORTD & 0x0F) | ( (SegData & 0x0F)  << 4) ;
   
  //Decimal Point il vom trata ca 1 logic
  //dar catodul DP e mai departe activ pe 0 logic
  PORTC = (DP)? PORTC &  (~(1<<PORTC1)) : PORTC | (1<<PORTC1); 
   
}


 /* ***************************************************************************
 * void DisplayRawNumber(uint16_t Number, uint8_t DP)
 * Afiseaza un numar de 16 biti pe 4 digiti (neformatat, adica in hexazecimal)
 * Argumente:
 *  Number: fiecare grup de 4 biti ai acestuia va fi decodificat
 *          si afisat pe afisajul de 7 segmente, 4 digiti:
 *          bit [15:12]-Digit3, bit [11:8]-Digit2, bit [7:4]-Digit1,
 *          bit [3:0]-Digit0
 *  DP: bit 3 aprinde punctul zecimal de la Digit3
 *      bit 2 aprinde punctul zecimal de la Digit2 samd
* ***************************************************************************/
void DisplayRawNumber(uint16_t Number, uint8_t DP){
   uint8_t i;
     
   DecimalPoint = DP;
   
   for (i = 0; i <4; i++){
      digits[i] = (Number & (0x0F << (4*i)) ) >> (4*i);
   }
}

 /* ***************************************************************************
 * uint16_t Format4DigitBCDNumber(uint16_t Number)
 * Formateaza un numar pe 16 biti intr-un numar in BCD (zecimal)
 * Argumente:
 *  Number: Numarul de formatat
 * 
  * Valoarea de retur:
  *     Numarul in format BCD
  * Daca Number >9999, atunci numarul returnat va afisa doar primele
  * 3 cifre seminficative, iar pe bitii [15:12] va aparea 0xE (Eroare) 
* ***************************************************************************/
uint16_t Format4DigitBCDNumber(uint16_t Number){
   uint16_t Temp=Number, Num10Disp=0;
   uint8_t i = 0;
   
   while(Temp!=0){//Desfacem Temp in valori BCD
       //Valoarea BCD = restul impartirii la 10
       //Noua cifra ajunge pe pozitia cea mai semnificativa
       Num10Disp = Num10Disp | ((Temp % 10)<<12)  ;
       Temp = Temp /10;
       if (Temp!=0) Num10Disp = Num10Disp >> 4;
       i++;
   }
   if (i<=4){
   //Am avut o cifra  < 9999. Atunci deplasam Num10Disp, 
   //astfel ca unitatile sa ajunga pe pozitia cea mai putin semnificativa
       Num10Disp = Num10Disp >> (4 * (4-i));
   }
   else{
   ///Am avut o cifra  >= 10000. 
   //Atunci afisam doar 3 cifre cele mai semnificative, 
   //iar pe prima pozitie afisam E
       Num10Disp = (Num10Disp >> 4) | 0xE000;
   }
    
   return Num10Disp;
    
}

 /* ***************************************************************************
  * void DisplayOff()
  * Stinge afisajul 
* ***************************************************************************/
void DisplayOff(){

   EnDisplay = 0;
}
 /* ***************************************************************************
  * void DisplayOn()
  * Aprinde afisajul 
* ***************************************************************************/
void DisplayOn(){

   EnDisplay = 1;
}

 /* ***************************************************************************
  * void TurnOffDigit(uint8_t Digit_Num)
  * Stinge un digit din afisaj
  * Argumente:
  *     Digit_Num: Numarul digit-ului de stins, 0..3
  * Daca Digit_Num >3, functia nu va avea efect 
* ***************************************************************************/
void TurnOffDigit(uint8_t Digit_Num){
    if (Digit_Num < 4){
        BlankDigit = BlankDigit | (1 << Digit_Num);
    }
}

 /* ***************************************************************************
  * void TurnOnDigit(uint8_t Digit_Num)
  * Aprinde un digit din afisaj
  * Argumente:
  *     Digit_Num: Numarul digit-ului de aprins, 0..3
  * Daca Digit_Num >3, functia nu va avea efect 
* ***************************************************************************/
void TurnOnDigit(uint8_t Digit_Num){
    if (Digit_Num < 4){
        BlankDigit = BlankDigit & (~(1 << Digit_Num));
    }
}

