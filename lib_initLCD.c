#include "lib_initLCD.h"

void initLCD (void) {
    //Start LCD
    setCmd(0x00);
    __delay_ms(20);
    setCmd(0x30);
    __delay_ms(5);
    setCmd(0x30);
    __delay_ms(10);
    setCmd(0x30);
    __delay_us(100);
    //8-bit mode
    setCmd(0x38);
    //Turn on screen
    setCmd(0x0C);
    //Clear screen
    setCmd(0x01);
    //Set character writting mode
    setCmd(0x06);
    __delay_ms(100);
}

void setCmd (char a) {
    _RS = 0;
    _LCDPORT = a;
    _EN = 1;
    __delay_ms(4);
    _EN = 0;
}

void setCursor (char x, char y) {
    char a;
    if (x == 1) {
        a = 0x80 + y - 1;
        setCmd(a);
    }
    else if (x == 2) {
        a = 0xC0 + y - 1;
        setCmd(a);
    }
}

void clearLCD (void) {
    setCmd(0);
    setCmd(1);
}

void writeChar (char a) {
    _RS = 1;
    _LCDPORT = a;
    _EN = 1;
    __delay_us(40);
    _EN = 0;
}

void writeString (char *a) {
    char i;
    for (i = 0; a[i] != '\0'; i++) {
        writeChar(a[i]);
    }
}