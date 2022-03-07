/*
 * File:            main.c
 * Author:          Rodrigo Alejandro Cruz Fagiani
 * Description:     Master with I2C communication and LCD control
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
#include "LCD.h"
#include "UART.h"
#include "I2C.h"
#include "MPU6050.h"

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
uint8_t _lights = 0;
uint8_t _rx_data = 0;

//******************************************************************************
// Function Prototypes
//******************************************************************************
void setup (void);
uint8_t I2C_Write(uint8_t address, uint8_t data);
uint8_t I2C_Read_Func(uint8_t address);

//******************************************************************************
// Interrupts
//******************************************************************************
void __interrupt () isr () {
    if(PIR1bits.RCIF) {
        _rx_data = RCREG;
        _lights = _rx_data - 48;
        I2C_Write(0x50, _lights);
        PIR1bits.RCIF = 0;
    }
}

//******************************************************************************
// Main
//******************************************************************************
void main(void) {
    setup();
    //**************************************************************************
    //Loop Principal
    //**************************************************************************
    clearLCD();
    while(1) {
        //MPU6050 Reading
        MPU6050_Read();
        //LCD Writing
        setCursor(1,1);
        writeString("SEMF: ");
        if (_lights == 0) {
            writeString("OFF   ");
        } else if (_lights == 1) {
            writeString("RED   ");
        } else if (_lights == 2) {
            writeString("YLW   ");
        } else if (_lights == 3) {
            writeString("GRN   ");
        }
        __delay_ms(1000);
    }
}

//******************************************************************************
// Setup Functions
//******************************************************************************
void setup (void) {
    //**************************************************************************
    //  IO Configurations
    //**************************************************************************
    //Digital or Analog Inputs
    ANSEL = 0;
    ANSELH = 0;
    //Input or Output
    TRISA = 0;
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISD = 0x00;
    //Clear Ports
    PORTA = 0;
    
    //**************************************************************************
    //  Clock Configurations
    //**************************************************************************
    OSCCONbits.IRCF = 0b111;    //8MHz
    OSCCONbits.SCS = 1;         //internal oscillator
    
    //**************************************************************************
    //  Interrupts
    //**************************************************************************
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1;
    PIR1bits.RCIF = 0;
    
    //**************************************************************************
    //  Library Configurations
    //**************************************************************************
    initLCD();
    initUART(8);
    MPU6050_Init();
}

uint8_t I2C_Write(uint8_t address, uint8_t data) {
    I2C_Master_Start();
    I2C_Master_Write(address);
    I2C_Master_Write(data);
    I2C_Master_Stop();
    __delay_ms(50);
}
uint8_t I2C_Read_Func(uint8_t address) {
    uint8_t data = 0;
    I2C_Master_Start();
    I2C_Master_Write(address);
    data = I2C_Master_Read(0);
    I2C_Master_Stop();
    __delay_ms(50);
    return data;
}