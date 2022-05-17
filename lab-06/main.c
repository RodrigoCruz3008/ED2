#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"

#define XTAL 16000000

uint32_t ui32Period;
uint32_t i;
uint8_t UART_data;
uint8_t LED;

void Timer0IntHandler (void);
void UART0IntHandler (void);
void UARTStringPut (uint32_t UART_PORT_SELECT, char *UART_STRING);

int main(void)
{
    //System OSC 40MHZ
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //200MHZ / 5 = 40MHZ

    //Enable Interrupts
    IntMasterEnable();

    //LEDS AS OUTPUTS
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    //TMR0 CONFIG
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);       //Se le asigna el reloj al TIMER0
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);    //Se configura el TIMER0 como preiódico
    ui32Period = (SysCtlClockGet()) / 2;                //Se define un periodo de 500ms
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1); //Precargamos el valor y definimos un timer de 32 bits
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);

    //UART CONFIG
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);                    //UART0: PA_0<-RX & PA_1<-TX
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTFIFOEnable(UART0_BASE);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    UARTIntRegister(UART0_BASE, UART0IntHandler);
    UARTEnable(UART0_BASE);
    IntEnable(INT_UART0);

    while(1)
    {

    }
}

void Timer0IntHandler (void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);                             //Clear TIMER0 interrupt
    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3))
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, LED);
    }
}

void UART0IntHandler (void)
{
    UART_data = UARTCharGet(UART0_BASE);
    switch (UART_data) {
    case 'r':
        LED = 0x02;
        UARTStringPut(UART0_BASE, "RED ON\n");
        break;
    case 'g':
        LED = 0x08;
        UARTStringPut(UART0_BASE, "GREEN ON\n");
        break;
    case 'b':
        LED = 0x04;
        UARTStringPut(UART0_BASE, "BLUE ON\n");
        break;
    default:
        LED = 0x00;
        UARTStringPut(UART0_BASE, "ERROR\n");
        break;
    }
}

void UARTStringPut (uint32_t UART_PORT_SELECT, char *UART_STRING)
{
    while (*UART_STRING != '\0')
    {
        UARTCharPut(UART_PORT_SELECT, *UART_STRING);
        UART_STRING++;
    }
}
