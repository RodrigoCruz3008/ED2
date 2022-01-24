/*
 * File:            lab-00.c
 * Author:          Rodrigo Alejandro Cruz Fagiani
 * Description:     Racing-Lights Game
 * Created:         January 17, 2022
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

#include <xc.h>
#include <stdint.h>

/*Consntants*/
#define _XTAL_FREQ 250000
#define _tmr0_value 11

/*Arrays*/
unsigned char sevenSegment [6] = {0b11000000,0b11111001,0b10100100,0b10110000, 0b11111001, 0b10100100};

/*Variables*/
uint8_t _player_one_score;
uint8_t _player_two_score;
uint8_t _debouncer_one_flag;    //Debouncer for player one button   
uint8_t _debouncer_two_flag;    //Debouncer for player two button 
uint8_t _game_flag;             //Game ON or OFF
uint8_t _race_flag;             //Race ONGOING or NOT
uint8_t _countdown_flag;        //Race Countdown Flag
uint8_t _countdown_timer;       //Race Countdown Timer

/*Function Prototypes*/
void setup(void);       //Initial Setup
void initVal(void);     //Initial Values for Variables and Registers
void startGame(void);   //Values for Variables and Registers when Game Starts
void endGame(void);     //Values for Variables and Registers when Game Ends

/*Interrupts*/
void __interrupt()isr (void) {
    if (T0IF) {
        //Seconds Coundown Timer when Countdown is Enabled
        if (_countdown_flag == 1) {
            if (_countdown_timer >= 1) {
                _countdown_timer = _countdown_timer - 1;
                PORTC = sevenSegment[_countdown_timer];
                if (_countdown_timer == 1) {
                    PORTA = PORTA * 2;
                }
                if (_countdown_timer == 0) {
                    PORTA = PORTA * 2;
                    _countdown_flag = 0;
                    _race_flag = 1;
                }
            }
        }
        INTCONbits.T0IF = 0;
        TMR0 = _tmr0_value;
    }
}

/*Main*/
void main (void) {
    setup();
    initVal();
    while(1) {
        //Start Game
        if (PORTAbits.RA3 == 0 && _game_flag == 0) {
            startGame();
        }
        //Race
        while (_race_flag == 1) {
            //Player 1 Controls
            if (PORTAbits.RA4 == 0 || _debouncer_one_flag == 1) {
                _debouncer_one_flag = 1;
                if (_debouncer_one_flag == 1 && PORTAbits.RA4) {
                    _debouncer_one_flag = 0;
                    _player_one_score = _player_one_score * 2;
                    PORTB = _player_one_score;
                }
                if (_player_one_score == 128) {
                    PORTB = 255;
                    PORTC = sevenSegment[4];
                    PORTD = 0;
                    __delay_ms(2000);
                    endGame();
                }
            }
            //Player 2 Controls
            if (PORTAbits.RA5 == 0 || _debouncer_two_flag == 1) {
                _debouncer_two_flag = 1;
                if (_debouncer_two_flag == 1 && PORTAbits.RA5) {
                    _debouncer_two_flag = 0;
                    _player_two_score = _player_two_score * 2;
                    PORTD = _player_two_score;
                }
                if (_player_two_score == 128) {
                    PORTB = 0;
                    PORTC = sevenSegment[5];
                    PORTD = 255;
                    __delay_ms(2000);
                    endGame();
                }
            }
        }
    }
}

/*Functions*/
void setup(void) {
    //IO Configuration
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0b00111000;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    //Clock Configuration
    OSCCONbits.IRCF = 0b010;    //250kHz
    OSCCONbits.SCS = 1;         //Internal oscillator is used
    
    //Timer0 Registers Prescaler= 256 - TMR0 Preset = 61 - Freq = 10.02 Hz - Period = 0.099840 seconds
    OPTION_REGbits.T0CS = 0;    // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0;    // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;     // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;     // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    TMR0 = _tmr0_value;         // preset for timer register
    
    //Interrupt Configurations
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    
    return;
}
void initVal(void) {
    _game_flag = 0;
    PORTC = 255;
}
void startGame(void) {
    PORTA = 1;
    _player_one_score = 1;
    _player_two_score = 1;
    _debouncer_one_flag = 0;      
    _debouncer_two_flag = 0;
    _game_flag = 1;
    _race_flag = 0;
    _countdown_flag = 1;
    _countdown_timer = 4;
}
void endGame(void) {
    PORTA = 0;
    PORTB = 0;
    PORTC = 255;
    PORTD = 0;
    _player_one_score = 1;
    _player_two_score = 1;
    _game_flag = 0;
    _race_flag = 0;
}