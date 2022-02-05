#include "lib_initUART.h"

void initAsyncUART (uint8_t freq) {
    //Asynchronous Communication Setup
    TXSTAbits.SYNC = 0;         //asynchronous communication
    BAUDSelect (freq);
    RCSTAbits.SPEN = 1;         //enable SPEN bit
    //Asynchrounous Transimission Setup
    TXSTAbits.TX9 = 0;
    TXSTAbits.TXEN = 1;         //enable transmition
    PIR1bits.TXIF = 1;          //clear flag
    //Asynchrounous Reception Setup
    RCSTAbits.RX9 = 0;          //9 bits communication disabled
    RCSTAbits.CREN = 1;         //enable reception
    PIR1bits.TXIF = 1;          //clear flag
    return;
}
void BAUDSelect (uint8_t freq) {
    switch(freq) {
        case 4:
            TXSTAbits.BRGH = 1;
            BAUDCTLbits.BRG16 = 1;      //BAUD 9,600
            SPBRG = 103;
            SPBRGH = 0;
            break;
        case 8:
            TXSTAbits.BRGH = 0;
            BAUDCTLbits.BRG16 = 0;      //BAUD 9,600
            SPBRG = 12;
            SPBRGH = 0;
            break;
        default:
            return;
    }
    return;
}