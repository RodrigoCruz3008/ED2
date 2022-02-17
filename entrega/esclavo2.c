/*
 * File:            main.c
 * Author:          Rodrigo Alejandro Cruz Fagiani
 * Description:     Counter slave
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
#include "SPI.h"

//******************************************************************************
// Consntants
//******************************************************************************
#define _XTAL_FREQ 8000000

//******************************************************************************
// Variables
//******************************************************************************
uint8_t _inc_flag = 0;
uint8_t _dec_flag = 0;
uint8_t _counter = 0;

//******************************************************************************
// Function Prototypes
//******************************************************************************
void initPorts (void);
void initClock (void);
void initInterrupts (void);

//******************************************************************************
// Interrupts
//******************************************************************************
void __interrupt () isr () {
    if(PIR1bits.SSPIF){
        spiWrite(_counter);
        SSPIF = 0;
    }
}

//******************************************************************************
// Main
//******************************************************************************
void main(void) {
    initPorts();
    initClock();
    initInterrupts();
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    //**************************************************************************
    //Loop Principal
    //**************************************************************************
    while(1) {
        
        if (!PORTBbits.RB0) {
            _inc_flag = 1;
        }
        if (!PORTBbits.RB1) {
            _dec_flag = 1;
        }
        if (PORTBbits.RB0 && _inc_flag == 1) {
            _counter++;
            _inc_flag = 0;
        }
        if (PORTBbits.RB1 && _dec_flag == 1) {
            _counter--;
            _dec_flag = 0;
        }
        PORTD = _counter;
    }
}

//******************************************************************************
// Setup Functions
//******************************************************************************
void initPorts (void) {
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0xFF;
    //TRISC = 0;
    TRISD = 0;
    
    TRISAbits.TRISA5 = 1;
    
    PORTA = 0;
    PORTB = 0;
    //PORTC = 0;
    PORTD = 0;
    
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0b00000011;
}
void initClock (void) {
    OSCCONbits.IRCF = 0b111;    //8MHz
    OSCCONbits.SCS = 1;         //internal oscillator
}
void initInterrupts (void) {
    INTCONbits.GIE = 1;         // global interrupts
    INTCONbits.PEIE = 1;        // peripheral interrupts
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupción MSSP
}