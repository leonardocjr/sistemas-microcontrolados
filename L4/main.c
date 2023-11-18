#include <xc.h>
#include "nxlcd.h"
// Configurações
#pragma config PLLDIV = 5 // PLL para 20MHz
#pragma config CPUDIV = OSC1_PLL2 // PLL desligado
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config WDT = OFF // Watchdog timer desativado
#pragma config PBADEN = OFF // Pinos do PORTB começam como digitais
#pragma config LVP = OFF // Desabilita gravação em baixa tensão
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR e desabilita RE3 como I/O
#define _XTAL_FREQ 20000000 // uC opera com cristal de 20 MHz

unsigned char teclado(void) {
    unsigned char t = 0xFF;

    PORTB = 0b11110111; // verifica as teclas da primeira coluna
    NOP();
    NOP();
    NOP();
    if (PORTBbits.RB4 == 0) {
        t = '1';
    }
    if (PORTBbits.RB5 == 0) {
        t = '4';
    }
    if (PORTBbits.RB6 == 0) {
        t = '7';
    }
    if (PORTBbits.RB7 == 0) {
        t = 'F';
    }

    PORTB = 0b11111011; // verifica as teclas da segunda coluna
    NOP();
    NOP();
    NOP();
    if (PORTBbits.RB4 == 0) {
        t = '2';
    }
    if (PORTBbits.RB5 == 0) {
        t = '5';
    }
    if (PORTBbits.RB6 == 0) {
        t = '8';
    }
    if (PORTBbits.RB7 == 0) {
        t = '0';
    }

    PORTB = 0b11111101; // verifica as teclas da terceira coluna
    NOP();
    NOP();
    NOP();
    if (PORTBbits.RB4 == 0) {
        t = '3';
    }
    if (PORTBbits.RB5 == 0) {
        t = '6';
    }
    if (PORTBbits.RB6 == 0) {
        t = '9';
    }
    if (PORTBbits.RB7 == 0) {
        t = 'E';
    }

    PORTB = 0b11111110; // verifica as teclas da quarta coluna
    NOP();
    NOP();
    NOP();
    if (PORTBbits.RB4 == 0) {
        t = 'A';
    }
    if (PORTBbits.RB5 == 0) {
        t = 'B';
    }
    if (PORTBbits.RB6 == 0) {
        t = 'C';
    }
    if (PORTBbits.RB7 == 0) {
        t = 'D';
    }

    if (t != 0xFF) // Debounce se alguma tecla foi pressionada
    {
        __delay_ms(300);
    }

    return t;
}

void main(void) {
    unsigned char tecla = 0;
    
    TRISE = 0x00;
    TRISD = 0x00;
    TRISB = 0b11110000;
    
    INTCON2bits.RBPU = 0;
    
    //Inicialização do LCD
    OpenXLCD(FOUR_BIT & LINES_5X7); 
    WriteCmdXLCD(0x01); 
    __delay_ms(2); 
    
    WriteCmdXLCD(0x0C);
    
    WriteCmdXLCD(0x84);
    putsXLCD (" TECLA: ");
    
    
    while (1) {
        tecla = teclado();
        if(tecla != 0xFF){
            WriteCmdXLCD(0xC7);
            putcXLCD(tecla);
        }
    }
}