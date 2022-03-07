/*
 * File:            slave.c
 * Author:          Rodrigo Alejandro Cruz Fagiani
 * Description:     Lights control slave
 * Created:         February 25, 2022
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
#include "I2C.h"

//******************************************************************************
// Consntants
//******************************************************************************
#define _XTAL_FREQ 8000000

//******************************************************************************
// Variables
//******************************************************************************
uint8_t z;
uint8_t _rx_data;
uint8_t _tx_data;

//******************************************************************************
// Function Prototypes
//******************************************************************************
void setup (void);

//******************************************************************************
// Interrupts
//******************************************************************************
void __interrupt () isr () {
    if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            _rx_data = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = _tx_data;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}

//******************************************************************************
// Main
//******************************************************************************
void main(void) {
    setup();
    //**************************************************************************
    // Main Loop
    //**************************************************************************
    while(1) {
        if (_rx_data == 1) {
            PORTB = 0x01;
        } else if (_rx_data == 2) {
            PORTB = 0x04;
        } else if (_rx_data == 3) {
            PORTB = 0x10;
        } else {
            PORTB = 0x00;
        }
    }
}

//******************************************************************************
// Setup Function
//******************************************************************************
void setup (void) {
    //**************************************************************************
    //  IO Configurations
    //**************************************************************************
    //Digital or Analog Inputs
    ANSEL = 0;
    ANSELH = 0;
    //Input or Output
    TRISB = 0;
    //Clear Ports
    PORTB = 0;
    
    //**************************************************************************
    //  Clock Configurations
    //**************************************************************************
    OSCCONbits.IRCF = 0b111;    //8MHz
    OSCCONbits.SCS = 1;         //internal oscillator
    
    //**************************************************************************
    //  Library Configurations
    //**************************************************************************
    I2C_Slave_Init(0x50);
}