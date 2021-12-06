
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SSD_H
#define	SSD_H

#include <avr/io.h>

//Alocarea pinilor in aceasta biblioteca
//Segmenti
// DP  G   F   E   D   C   B   A
//PC1 PB1 PB3 PB2 PD7 PD6 PD5 PD4
//Anozi
//AN3 AN2 AN1 AN0
//PC2 PC3 PC4 PC5


void init_Display();

void DisplaySingleDigit(uint8_t Number, uint8_t DP);

void DisplayRawNumber(uint16_t Number, uint8_t DP);

uint16_t Format4DigitBCDNumber(uint16_t Number);

void DisplayOff();

void DisplayOn();

void TurnOffDigit(uint8_t Digit_Num);

void TurnOnDigit(uint8_t Digit_Num);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

