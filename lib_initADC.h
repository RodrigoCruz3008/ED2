/* 
 * File:                lib_initADC.h
 * Author:              Rodrigo Cruz
 * Comments:            Library to initalize and use ADC
 * Last revision:       January 31, 2022
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LIB_INITADC_H
#define	LIB_INITADC_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000
#endif

//------------------------------------------------------------------------------
// Libraries
//------------------------------------------------------------------------------
#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>

//------------------------------------------------------------------------------
// ADC Configuration
//------------------------------------------------------------------------------
/* Explanation:
 * Function used to for the configuration of the ADC. Take in consideration
 * that interrupt flags aren´t setup here.
 */
void initADC (char freq);        //ADC configurations
void adcFoscSelect (char freq);  //ADC fosc Select

//------------------------------------------------------------------------------
// ADC Channel Change
//------------------------------------------------------------------------------
/* Explanation:
 * In this function, arr[] stands for the array that contains all the channels 
 * that are going to be used in our code. In the other hand, "n" stands for the 
 * number of channels that are goind to be used and are specified inside arr[]. 
 * This variable uses numbers x > 0 (position 0 is not taken in consideration).
 */
/* Requirements:
 * Send an array with all channels to use and number of channels. Also the
 * function shall be inside a if(!ADCON0bits.GO) statement.
 */
uint8_t _adc_channel_counter = 0;
void adcChannelChange (void);   //ADC channel selection

#endif	

