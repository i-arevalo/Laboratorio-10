/**
 * Programador: Israel Arévalo
 *
 * Material: Electrónica digital 2
 *
 * Nombre del archivo: main.c
 *
 * Referencia: Se tomó como referencia la función de enviarcomandoAT de Diego Equité para hacer la función de comandoAT. También se utilizó como referencia
 *             los ejemplos dados por pablo mazariegos en la clase de ESP
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
uint32_t i = 0;

void comandoAT(char *cmd);

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
    for(i=0; i<10000; i++);

    while(1){
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) != 16)
        {
            cont++;
            while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) != 16);//Antirebote del push para cambiar de color
            if (cont == 8)//Sentencia para saber cuando acaba el ciclo de colores
                cont = 0;
            switch (cont){
            case 0://negro
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
                break;
            case 1://Azul
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);//rojo
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xff);//azul
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);//verde
                break;
            case 2://Verde
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
            case 7://Blanco
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xff);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xff);
                break;

            }
        }
        if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){
            SysCtlDelay(500000);//Delay del antirrebote
            while(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0));
            switch(cont)
            {
            case 0:
                comandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("AT+CIPSEND=166");
                SysCtlDelay(4000000);//Delay para que se ejecute la instrucción
                comandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 37\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18267&id_color=000&color=Negro\r\n");//verificado
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                break;
            case 1:
                comandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(4000000);//Delay para que se ejecute la instrucción
                comandoAT("AT+CIPSEND=165");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 36\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18267&id_color=001&color=Azul\r\n");//verificado
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                break;
            case 2:
                comandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("AT+CIPSEND=166");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 37\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18267&id_color=010&color=Verde\r\n");//verificado
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                break;
            case 3:
                comandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("AT+CIPSEND=169");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 40\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18267&id_color=011&color=Turquesa\r\n");//verificado
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                break;
            case 4:
                comandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("AT+CIPSEND=165");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 36\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18267&id_color=100&color=Rojo\r\n");//verificado
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                break;
            case 5:
                comandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("AT+CIPSEND=168");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 39\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18267&id_color=101&color=Violeta\r\n");//verificado
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                break;
            case 6:
                comandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("AT+CIPSEND=169");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 39\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18267&id_color=110&color=Amarillo\r\n");//verificado
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                break;
            case 7:
                comandoAT("AT+CIPSTART=\"TCP\",\"192.168.1.9\",80");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("AT+CIPSEND=167");
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                comandoAT("POST /index.php HTTP/1.0\r\nHost: 192.168.1.9\r\nAccept: */*\r\nContent-Length: 38\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18267&id_color=111&color=Blanco\r\n");//verificado
                SysCtlDelay(40000000);//Delay para que se ejecute la instrucción
                break;
            }
        }
    }
}

//Función para enviar los comandos AT por el UART 1
void comandoAT(char *cmd)
{
    while(UARTBusy(UART1_BASE));//Ciclo para esperar a que el UART1 esté disponible
    while(*cmd != '\0')
    {
        UARTCharPut(UART1_BASE, *cmd++);//Se envía la instrucción caracter por caracter
    }
    //Saldo de línea para terminar la instruccion AT
    UARTCharPut(UART1_BASE, '\r');
    UARTCharPut(UART1_BASE, '\n');

}
