/**
 * Programador: Israel Arévalo
 *
 * Material: Electrónica digital 2
 *
 * Nombre del archivo: main.c
 *
 * Referencia:
 */


//librerías
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/TM4C123GH6PM.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"//librería de interrupciones
#include "driverlib/uart.h"//librería de comunicación UART


int cont = 0;
uint32_t x = 0;

void enviarcomandoAT(char *cmd);

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);//Se configura el reloj oscilador
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);//Se habilita el puerto F como salida
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));//Se espera a que se termine de habilitar el puerto F
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R = 0X1;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);//Se habilita el pin0 como entrada
    //Configuracion de botonnes como pull-up
    GPIOPadConfigSet (GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet (GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //Configuración del UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
    UARTConfigSetExpClk(UART1_BASE, 16000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART1_BASE);
    //enviarcomandoAT("AT");
    for(x=0;x<10000;x++);

    while(1){
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) != 16)
        {
            cont++;
            while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) != 16);
            if (cont == 8)
                cont = 0;
            switch (cont){
            case 0://negro
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
                break;
            case 1:
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);//rojo
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xff);//azul
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);//verde
                break;
            case 2:
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xff);
                break;
            case 3:
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xff);
                break;
            case 4:
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
                break;
            case 5:
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
                break;
            case 6:
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xff);
                break;
            case 7:
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xff);
                break;

            }
        }
        if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){
            SysCtlDelay(500000);//ESPERO 200 MS PARA EL ANTIRREBOTE
            while(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0));
            switch(cont)
            {
            case 0:
                enviarcomandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);
                enviarcomandoAT("AT+CIPSEND=166");
                SysCtlDelay(4000000);
                enviarcomandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 36\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\carnet=18267&id_color=000&color=Negro\r\n");
                SysCtlDelay(40000000);
                break;
            case 1:
                enviarcomandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(4000000);
                enviarcomandoAT("AT+CIPSEND=165");
                SysCtlDelay(40000000);
                enviarcomandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 35\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\carnet=18267&id_color=001&color=Azul\r\n");
                SysCtlDelay(40000000);
                break;
            case 2:
                enviarcomandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);
                enviarcomandoAT("AT+CIPSEND=166");
                SysCtlDelay(40000000);
                enviarcomandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 36\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\carnet=18267&id_color=010&color=Verde\r\n");
                SysCtlDelay(40000000);
                break;
            case 3:
                enviarcomandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);
                enviarcomandoAT("AT+CIPSEND=169");
                SysCtlDelay(40000000);
                enviarcomandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 39\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\carnet=18267&id_color=011&color=Turquesa\r\n");
                SysCtlDelay(40000000);
                break;
            case 4:
                enviarcomandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);
                enviarcomandoAT("AT+CIPSEND=165");
                SysCtlDelay(40000000);
                enviarcomandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 35\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\carnet=18267&id_color=100&color=Rojo\r\n");
                SysCtlDelay(40000000);
                break;
            case 5:
                enviarcomandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);
                enviarcomandoAT("AT+CIPSEND=168");
                SysCtlDelay(40000000);
                enviarcomandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 38\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\carnet=18267&id_color=101&color=Violeta\r\n");
                SysCtlDelay(40000000);
                break;
            case 6:
                enviarcomandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);
                enviarcomandoAT("AT+CIPSEND=169");
                SysCtlDelay(40000000);
                enviarcomandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 39\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\carnet=18267&id_color=110&color=Amarillo\r\n");
                SysCtlDelay(40000000);
                break;
            case 7:
                enviarcomandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);
                enviarcomandoAT("AT+CIPSEND=167");
                SysCtlDelay(40000000);
                enviarcomandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 37\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\carnet=18267&id_color=111&color=Blanco\r\n");
                SysCtlDelay(40000000);
                break;
            }
        }
    }
}

void enviarcomandoAT(char *cmd)
{
    while(UARTBusy(UART1_BASE));
    while(*cmd != '\0')
    {
        UARTCharPut(UART1_BASE, *cmd++);
    }
    UARTCharPut(UART1_BASE, '\r'); //CR \r\n
    UARTCharPut(UART1_BASE, '\n'); //LF

}
