/*
 * File:            lab-01
 * Author:          Rodrigo Alejandro Cruz Fagiani
 * Description:     Alarm on when ADC is bigger than 8-bit counter
 * Created:         January 24, 2022
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

#include "myLibraries.h"
//******************************************************************************
// Consntants
//******************************************************************************
#define _XTAL_FREQ 4000000
#define _tmr0_value 10

//******************************************************************************
// Variables
//******************************************************************************
uint8_t _inc_debouncer = 0;
uint8_t _dec_debouncer = 0;
uint8_t _adresh_saver = 0;

//******************************************************************************
// Function Prototypes
//******************************************************************************
void initPorts (void);
void initClock (void);
void initTimer0 (void);
void initADC (void);
void initInterrupts (void);
void initVars (void);

//******************************************************************************
// Interrupts
//******************************************************************************
void __interrupt () isr () {
    // TMR0 Interrupt
    if (INTCONbits.T0IF) {
        if (PORTD == 1) {
            PORTD = 2;
            PORTC = highHex(_adresh_saver);
        } else {
            PORTD = 1;
            PORTC = lowHex(_adresh_saver);
        }
        INTCONbits.T0IF = 0;
        TMR0 = _tmr0_value;
    }
    // Weak Pull-ups Interrupt
    if (INTCONbits.RBIF) {
        if (!PORTBbits.RB0) {
            _inc_debouncer = 1;
        } else if (!PORTBbits.RB1) {
            _dec_debouncer = 1;
        }
        INTCONbits.RBIF = 0;
        return;
    }
    // ADC Interrupt
    if (PIR1bits.ADIF) {
        _adresh_saver = ADRESH;
        PIR1bits.ADIF = 0;
        return;
    }
}

//******************************************************************************
// Main
//******************************************************************************
void main(void) {
    //**************************************************************************
    // Main Setup
    //**************************************************************************
    initPorts();
    initClock();
    initTimer0();
    initADC();
    initInterrupts();
    //**************************************************************************
    // Main Loop 
    //**************************************************************************
    ADCON0bits.GO = 1;
    while(1) {
        if (!ADCON0bits.GO) {
            ADCON0bits.CHS = 8;
            ADCON0bits.GO = 1;
            __delay_us(50);
        }
        if (PORTBbits.RB0 && _inc_debouncer == 1) {
            _inc_debouncer = 0;
            PORTA++;
        }
        if (PORTBbits.RB1 && _dec_debouncer == 1) {
            _dec_debouncer = 0;
            PORTA--;
        }
        if (_adresh_saver >= PORTA) {
            PORTBbits.RB3 = 1;
        } else {
            PORTBbits.RB3 = 0;
        }
    }
}

//******************************************************************************
// Setup Functions
//******************************************************************************
void initPorts (void) {
    // IO Ports
    ANSEL = 0b10000000;
    ANSELH = 0;
    // Inputs or Outputs
    TRISA = 0;
    TRISB = 0b00000111;
    TRISC = 0;
    TRISD = 0;
    // Cleaning Ports
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 1;
    // Weak Pull-ups
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0b00000011;
    IOCB = 0b00000011;
}
void initClock (void) {
    OSCCONbits.IRCF = 0b110;    //4MHz
    OSCCONbits.SCS = 1;         //internal oscillator
}
void initTimer0 (void) {
    OPTION_REGbits.T0CS = 0;    // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0;    // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;     // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;     // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 0;
    TMR0 = _tmr0_value;         // preset for timer register to 5 ms
}
void initADC (void) {
    ADCON1bits.ADFM = 0;        //Justtify to the left
    ADCON1bits.VCFG0 = 0;       //Power as reference
    ADCON1bits.VCFG1 = 0;       //Ground as reference
    ADCON0bits.ADCS = 0b01;     //Fosc/32
    ADCON0bits.CHS = 8;         //Channel 8 Select
    ADCON0bits.ADON = 1;
    __delay_us(50);
}
void initInterrupts (void) {
    INTCONbits.GIE = 1;         // global interrupts
    INTCONbits.T0IE = 1;        // TMR0 interrupt enable
    INTCONbits.T0IF = 0;        // TMR0 flag cleaned
    INTCONbits.RBIE = 1;        // weak pull-ups interrupt enabled
    INTCONbits.RBIF = 0;        // weak pull-ups flag cleaned
    INTCONbits.PEIE = 1;        // peripheral interrupts
    PIE1bits.ADIE = 1;          // ADC interrupt enable
    PIR1bits.ADIF = 0;          // ADC flag cleaned
}
//******************************************************************************
// Main Functions
//******************************************************************************