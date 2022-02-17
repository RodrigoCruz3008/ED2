/*
 * File:            main.c
 * Author:          Rodrigo Alejandro Cruz Fagiani
 * Description:     proyect master
 * Created:         February 14, 2022
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//******************************************************************************
// Libraries
//******************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>

#include "LCD.h"
#include "SPI.h"
#include "UART.h"

//******************************************************************************
// Consntants
//******************************************************************************
#define _XTAL_FREQ 8000000
#define _LCDPORT PORTD
#define _RS PORTBbits.RB0
#define _EN PORTBbits.RB1

//******************************************************************************
// Variables
//******************************************************************************
uint8_t _pot_read;
uint8_t _count_read;
uint8_t _temp_read;

uint8_t _counter_units;
uint8_t _counter_tens;
uint8_t _counter_hundreds;

uint8_t _temp_units;
uint8_t _temp_tens;
uint8_t _temp_hundreds;

float _voltage;
float _temp;

unsigned char sensors[5] = {};
unsigned char uart [5] = {};

//******************************************************************************
// Function Prototypes
//******************************************************************************
void initPorts (void);
void initClock (void);
void initInterrupts (void);
void potSlave (void);
void countSlave (void);
void tempSlave (void);
void counterBreakdown (void);

//******************************************************************************
// Interrupts
//******************************************************************************

//******************************************************************************
// Main
//******************************************************************************
void main(void) {
    initPorts();
    initClock();
    initLCD();
    initAsyncUART(8);
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    //**************************************************************************
    // Main Loop
    //**************************************************************************
    clearLCD();
    setCursor(1,1);
    writeString ("VOL: NUM: TEMP: ");
    while(1) {
        potSlave();
        countSlave();
        tempSlave();
        
        _voltage = (_pot_read * 0.0196);
        
        counterBreakdown();
        
        _temp = (_temp_read * 1.16) + 2;
        
        setCursor(2,1);
        sprintf(sensors, "%.1fV", _voltage);
        writeString(sensors);
        string("VOL: ");
        string(sensors);
        sprintf(sensors, " %d", _counter_hundreds);
        writeString(sensors);
        string(" NUM: ");
        string(sensors);
        sprintf(sensors, "%d", _counter_tens);
        writeString(sensors);
        string(sensors);
        sprintf(sensors, "%d ", _counter_units);
        writeString(sensors);
        string(sensors);
        sprintf(sensors, " %.1f", _temp);
        writeString(sensors);
        string("TEMP: ");
        string(sensors);
        string("\n");
        writeString(" C");
    }
}

//******************************************************************************
// Setup Functions
//******************************************************************************
void initPorts (void) {
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0;
    TRISD = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    
    TRISC0 = 0;
    TRISC1 = 0;
    TRISC2 = 0;
    
    PORTCbits.RC0 = 1;
    PORTCbits.RC1 = 1;
    PORTCbits.RC2 = 1;
}
void initClock (void) {
    OSCCONbits.IRCF = 0b111;    //8MHz
    OSCCONbits.SCS = 1;         //internal oscillator
}
void potSlave (void) {
    PORTCbits.RC0 = 0;
    __delay_ms(1);
    spiWrite(1);
    _pot_read = spiRead();
    __delay_ms(1);
    PORTCbits.RC0 = 1;
    __delay_ms(250);
}
void countSlave (void) {
    PORTCbits.RC1 = 0;
    __delay_ms(1);
    spiWrite(1);
    _count_read = spiRead();
    __delay_ms(1);
    PORTCbits.RC1 = 1;
    __delay_ms(250);
}
void tempSlave (void) {
    PORTCbits.RC2 = 0;
    __delay_ms(1);
    spiWrite(_temp);
    _temp_read = spiRead();
    __delay_ms(1);
    PORTCbits.RC2 = 1;
    __delay_ms(250);
}
void counterBreakdown (void) {
    _counter_units = _count_read%10;
    _counter_tens = (_count_read%100 - _counter_hundreds)/10;
    _counter_hundreds = (_count_read - _counter_tens*10 - _counter_hundreds)/100;
}