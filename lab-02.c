/*
 * File:            lab-02   
 * Author:          Rodrigo Alejandro Cruz Fagiani
 * Description:     ADC value and counter to LCD
 * Created:         January 31, 2022
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
//Public Libraries
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>
//Private Libraries
#include "lib_initADC.h"
#include "lib_initUART.h"
#include "lib_initLCD.h"

//******************************************************************************
// Consntants
//******************************************************************************
#define _XTAL_FREQ 4000000
#define _LCDPORT PORTD
#define _RS PORTBbits.RB0
#define _EN PORTBbits.RB1

//******************************************************************************
// Variables
//******************************************************************************
uint8_t _adresh_one = 0;
uint8_t _adresh_two = 0;
float _sensor_one = 0;
float _sensor_two = 0;
uint8_t _counter = 0;
uint8_t _counter_units = 0;
uint8_t _counter_tens = 0;
uint8_t _counter_hundreds = 0;

unsigned char sensors[5] = {};
//unsigned char adc_channel_list[2] = {0,1};

//******************************************************************************
// Function Prototypes
//******************************************************************************
void initPorts (void);
void initClock (void);
void initInterrupts (void);
void txfunc(char data);
char rxfunc();
void string(char *str);
void counterBreakdown(void);

//******************************************************************************
// Interrupts
//******************************************************************************
void __interrupt () isr () {
    
    if (PIR1bits.RCIF) {
        if (RCREG == '+') {
            _counter++;
        } else if (RCREG == '-') {
            _counter--;
        }
        PIR1bits.RCIF = 0;
    }
    
    if(PIR1bits.ADIF) {
        if (ADCON0bits.CHS == 8) {
            _adresh_one = ADRESH;
        } else if (ADCON0bits.CHS == 9) {
            _adresh_two = ADRESH;
        }
        PIR1bits.ADIF = 0;
        return;
    }
}

//******************************************************************************
// Main
//******************************************************************************
void main(void) {
    initPorts();
    initClock();
    initInterrupts();
    initADC(4);
    initAsyncUART(4);
    initLCD();
    //**************************************************************************
    //Loop Principal
    //**************************************************************************
    clearLCD();
    setCursor(1,1);
    writeString("S1:   S2:   S3: ");
    ADCON0bits.GO = 1;
    while(1) {
        adcChannelChange();
        counterBreakdown();
        _sensor_one = (_adresh_one * 0.0196);
        _sensor_two = (_adresh_two * 0.0196);
        PORTA = _adresh_one;
        
        setCursor(2,1);
        sprintf(sensors, "%.2fV ", _sensor_one);
        writeString(sensors);
        sprintf(sensors, "%.2fV", _sensor_two);
        writeString(sensors);
        sprintf(sensors, " %d", _counter_hundreds);
        writeString(sensors);
        sprintf(sensors, "%d", _counter_tens);
        writeString(sensors);
        sprintf(sensors, "%d ", _counter_units);
        writeString(sensors);
    }
}

//******************************************************************************
// Setup Functions
//******************************************************************************
void initPorts (void) {
    ANSEL = 0x00;
    ANSELH = 0b011;
    
    TRISA = 0x00;
    TRISB = 0x0C;
    TRISC = 0b10000000;
    TRISD = 0;
    TRISE = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
}
void initClock (void) {
    OSCCONbits.IRCF = 0b110;    //4MHz
    OSCCONbits.SCS = 1;         //internal oscillator
}
void initInterrupts (void) {
    INTCONbits.GIE = 1;         // global interrupts
    INTCONbits.PEIE = 1;        // peripheral interrupts
    PIE1bits.ADIE = 1;          // ADC interrupt enable
    PIR1bits.ADIF = 0;          // ADC flag cleaned
    PIE1bits.RCIE = 1;          // enable RX interrupt
    PIR1bits.RCIF = 0;          // clear RX flag
}
//******************************************************************************
// Functions
//******************************************************************************
void txfunc(char data) {
    while(TXSTAbits.TRMT == 0);
    TXREG = data;
}
char rxfunc() {
    return RCREG;
}
void string(char *str) {
    while(*str != '\0')
    {
        txfunc(*str);
        str++;
    }
}
void counterBreakdown (void) {
    _counter_units = _counter%10;
    _counter_tens = (_counter%100 - _counter_hundreds)/10;
    _counter_hundreds = (_counter - _counter_tens*10 - _counter_hundreds)/100;
}