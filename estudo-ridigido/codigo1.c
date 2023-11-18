
#include <xc.h>
#include "nxlcd.c"
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

unsigned int contador = 0;

void main (void){
    //Definições dos modos de operação das portas digitais
    TRISCbits.RC0 = 0;
    TRISCbits.RC1 = 0;
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
        
    //Configurar Interrupções
    //Ativar interrupções e configurar se há prioridades
    
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    //Configurar as interrupções a serem utilizadas
    
    INTCON2bits.RBPU = 0;
    INTCONbits.INT0IE = 1;
    INTCONbits.INT0IF = 0;
    INTCON2bits.INTEDG0 = 0;
    
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IP = 0;
    INTCON2bits.INTEDG1 = 0;
    
    //Inicialização do LCD
    OpenXLCD(FOUR_BIT & LINES_5X7); // Modo 4 bits de dados e caracteres 5x7
    WriteCmdXLCD(0x01); // Limpa o LCD com retorno do cursor
    __delay_ms(2);
    
    WriteCmdXLCD(0x84);
    
    WriteCmdXLCD(0x0C); // Desliga cursor
    
    putrsXLCD ("Contador"); // 
    WriteCmdXLCD(0xC7); // Manda o cursor para o meio da segunda linha

    putcXLCD (0x30 + contador/100);
    putcXLCD (0x30 + contador/10);
    putcXLCD (0x30 + contador%10);
  
    while(1);
    
}

// Funções das ISRs
void interrupt HighPriorityISR(void)
{
    INTCONbits.INT0IF = 0;  // Limpa flag do INT0
    PORTCbits.RC0 = 0;
    contador++;
    if(contador>999){
        contador = 999;
    }
    
    WriteCmdXLCD(0xC7); // Manda o cursor para o meio da segunda linha

    putcXLCD (0x30 + contador/100);
    putcXLCD (0x30 + contador/10);
    putcXLCD (0x30 + contador%10);
    PORTCbits.RC0 = 1;
}

void interrupt low_priority LowPriorityISR(void)
{
    INTCON3bits.INT1IF = 0;  // Limpa flag do INT1
    PORTCbits.RC1 = 0;
    if(contador == 0){
        contador = 0;
    }else {
        contador--;
    }
    
    WriteCmdXLCD(0xC7); // Manda o cursor para o meio da segunda linha

    putcXLCD (0x30 + contador/100);
    putcXLCD (0x30 + contador/10);
    putcXLCD (0x30 + contador%10);
    PORTCbits.RC1 = 1;
}