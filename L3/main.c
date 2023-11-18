#include <xc.h>
#include "nxlcd.h"
// Configura��es
#pragma config PLLDIV = 5 // PLL para 20MHz
#pragma config CPUDIV = OSC1_PLL2 // PLL desligado
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config WDT = OFF // Watchdog timer desativado
#pragma config PBADEN = OFF // Pinos do PORTB come�am como digitais
#pragma config LVP = OFF // Desabilita grava��o em baixa tens�o
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR e desabilita RE3 como I/O
#define _XTAL_FREQ 20000000

void main(void){

    unsigned char x = 0, i = 0;
	TRISE = 0x00;
	TRISD = 0x00;

	//Inicializa��o do LCD
	OpenXLCD(FOUR_BIT & LINES_5X7); // inicia LCD
	WriteCmdXLCD(0x01);      // limpa LCD
	__delay_ms(2);           // aguarda limpar LCD
    WriteCmdXLCD(0x0C);  // desliga cursor
	
	WriteCmdXLCD(0x80);  // posi��o: inicio da 1� linha
	putsXLCD (" CONTADOR "); // escreve a string
	
	while(1)
	{
        if(PORTCbits.RC0 == 0){
            for(i=0;i<5;i++)
                __delay_ms(100);
            if(x < 99){
                x+=1;
            }
        }
        if(PORTCbits.RC1 == 0){
            for(i=0;i<5;i++)
                __delay_ms(100);
            if(x > 0){
                x-=1; 
            }
        }

		WriteCmdXLCD(0xC7); // posi��o: meio da 2� linha
		putcXLCD(0x30 + (x/10) ); // escreve 1� char
		putcXLCD(0x30 + (x%10) ); // escreve 2� char
	}
}