#include "lib_initADC.h"

//------------------------------------------------------------------------------
// ADC Configuration
//------------------------------------------------------------------------------
void initADC (char freq) {
    ADCON1bits.ADFM = 0;        //Justtify to the left
    ADCON1bits.VCFG0 = 0;       //Power as reference
    ADCON1bits.VCFG1 = 0;       //Ground as reference
    adcFoscSelect (freq);
    ADCON0bits.CHS = 8;         //Channel 0 Select
    ADCON0bits.ADON = 1;
    __delay_us(50);
    return;
}
void adcFoscSelect (char freq) {
    //Frequency must be 1MHz, 4MHz or 8MHz
    switch (freq) {
        case 1:
            ADCON0bits.ADCS = 0b000;    // Fosc/2
            break;
        case 4:
            ADCON0bits.ADCS = 0b001;    // Fosc/8
            break;
        case 8:
            ADCON0bits.ADCS = 0b010;    // Fosc/32
            break;
        default:
            ADCON0bits.ADCS = 0b001;    // Fosc/8
    }
    return;
}

//------------------------------------------------------------------------------
// ADC Channel Change
//------------------------------------------------------------------------------
void adcChannelChange (void) {
    /*
    uint8_t i;
    for (i = 0; i < n; i++) {
        if (ADCON0bits.CHS == arr[_adc_channel_counter]) {
            _adc_channel_counter++;
            __delay_us(50);
            ADCON0bits.CHS = arr[_adc_channel_counter];
            i = n;
        }
    }
    if (_adc_channel_counter >= (n-1)) {
        _adc_channel_counter = 0;
    }
    */
    if (ADCON0bits.GO == 0) {
        if (ADCON0bits.CHS == 8) {
            __delay_us(50);
            ADCON0bits.CHS = 9;
        } else {
            __delay_us(50);
            ADCON0bits.CHS = 8;
        }
        __delay_us(50);
        ADCON0bits.GO = 1;
    }
    return;
}
