#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

#define XTAL 16000000

//************************************************************************************************//
//  Variables                                                                                     //
//************************************************************************************************//
uint32_t i = 0;
uint8_t push_deb = 0;

//************************************************************************************************//
//  Function Prototypes                                                                           //
//************************************************************************************************//
void delay(uint32_t msec);
void delay1ms(void);
void colorChange(void);

//**************************************************************************************************************
// Funcion Principal
//**************************************************************************************************************
int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	//**********************************************************************************************************
   // Loop Principal
   //**********************************************************************************************************
	while (1) {
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 2);
	    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0) {
            push_deb = 1;
        }
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) && push_deb == 1) {
            colorChange();
            push_deb = 0;
        }
	}
}

void colorChange(void) {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 8);
    delay(3000);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
    delay(500);


    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 8);
    delay(500);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
    delay(500);


    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 8);
    delay(500);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 10);
    delay(3000);
}

//**************************************************************************************************************
// Funci n para hacer delay en milisegundos
//**************************************************************************************************************
void delay(uint32_t msec) {
    for (i = 0; i < msec; i++) {
        delay1ms();
    }

}
//**************************************************************************************************************
// Funci n para hacer delay de 1 milisegundos
//**************************************************************************************************************
void delay1ms(void) {
    SysTickDisable();
    SysTickPeriodSet(16000-1);
    SysTickEnable();

    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0); //Pg. 138

}
