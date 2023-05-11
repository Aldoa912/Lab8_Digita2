#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#define TARGET_IS_TM4C123_RA1
#define TIMER_PERIOD       (SysCtlClockGet() / 2)
static bool state = false;
char color;

int main(void)
{
    // configuracion del reloj interno
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // se habilita el reloj para el puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // se habilitan los pines 1, 2 y 3 como outputs del port F
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1);

    // se habilita y configura el timer0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerDisable(TIMER0_BASE, TIMER_BOTH);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, TIMER_PERIOD);
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);

    // se habilita la interrupcion del timer0
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_BOTH);

    // se habilita y configura el UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE) );

    // se habilita la interrupcion del UART0
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    while(1)
    {



    }

}

void Timer0IntHandler(void){
    // se baja la bandera de la interrupcion
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // cada vez que ocurra la interrupcion, la variable de state cambiara de valor
    state = !state;

    // se coloca el valor de state en el pin 1 del puerto B
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, state ? GPIO_PIN_1 : 0);
    TimerLoadSet(TIMER0_BASE, TIMER_A, TIMER_PERIOD);
}

void UARTIntHandler(void){
    // se baja la bandera de la interrupcion
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);

    // se le asigna el valor que reciba a la variable de color
    color = UARTCharGet(UART0_BASE);

    // se comparan los datos recibidos y si recibe una r, g o b se enciende la led correspondiente en la tiva
    if (color == 'r'){
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
        }
        else {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);
        }
    }
    if (color == 'g'){
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
        }
        else {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08);
        }
    }
    if (color == 'b'){
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
        }
        else {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);
        }
    }
}

