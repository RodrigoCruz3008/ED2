/* 
 * File:                lib_initLCD.h
 * Author:              Rodrigo Cruz
 * Comments:            Library to initalize and use LCD display
 * Last revision:       January 31, 2022
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LIB_INITLCD_H
#define	LIB_INITLCD_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000
#endif

//------------------------------------------------------------------------------
// Initialize LCD
//------------------------------------------------------------------------------
#ifndef _LCDPORT
#define _LCDPORT PORTD
#endif

#ifndef _RS
#define _RS PORTBbits.RB0
#endif

#ifndef _EN
#define _EN PORTBbits.RB1
#endif

#ifndef D0
#define D0 PORTDbits.RD0
#endif

#ifndef D1
#define D1 PORTDbits.RD1
#endif

#ifndef D2
#define D2 PORTDbits.RD2
#endif

#ifndef D3
#define D3 PORTDbits.RD3
#endif

#ifndef D4
#define D4 PORTDbits.RD4
#endif

#ifndef D5
#define D5 PORTDbits.RD5
#endif

#ifndef D6
#define D6 PORTDbits.RD6
#endif

#ifndef D7
#define D7 PORTDbits.RD7
#endif

//------------------------------------------------------------------------------
// Libraries
//------------------------------------------------------------------------------
#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>

void initLCD (void);
void setCmd (char portValue);
void setCursor (char x, char y);
void clearLCD (void);
void writeChar (char a);
void writeString (char *a);

#endif	

