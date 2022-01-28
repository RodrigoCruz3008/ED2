#include "myLibraries.h"

unsigned char lowHex (uint8_t adreshValue) {
    hexLowReg = adreshValue & 0b00001111;
    savingVar = sevenSegment[hexLowReg];
    return savingVar;
}
unsigned char highHex (uint8_t adreshValue) {
    hexHighReg = adreshValue & 0b11110000;
    hexHighReg = (hexHighReg>>4);
    savingVar = sevenSegment[hexHighReg];
    return savingVar;
}
