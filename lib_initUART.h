/* 
 * File:                lib_initUSART.h
 * Author:              Rodrigo Cruz
 * Comments:            Library to initalize and use USART communication
 * Last revision:       January 31, 2022
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LIB_INITUART_H
#define	LIB_INITUART_H

//------------------------------------------------------------------------------
// Libraries
//------------------------------------------------------------------------------
#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>


//------------------------------------------------------------------------------
// USART Configuration
//------------------------------------------------------------------------------
void initAsyncUART (uint8_t freq);
void BAUDSelect (uint8_t freq);

#endif	

