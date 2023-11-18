
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

unsigned int contador = 0, atualiza=0;
unsigned int contat1 = 0, atualizat1 = 0;

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
    
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR1IF = 0;
    IPR1bits.TMR1IP = 1;
    
    PIE2bits.TMR3IE = 1;
    PIR2bits.TMR3IF = 0;
    IPR2bits.TMR3IP = 0;
    
    //Configurar o TIMER1
    
    T1CONbits.RD16 = 1;
    T1CONbits.T1CKPS1 = 0;
    T1CONbits.T1CKPS0 = 0;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;
    
    TMR1H = 0;             // preset for timer1 MSB register
    TMR1L = 0xB4;             // preset for timer1 LSB register
    
    //Configurar o TIMER3
    
    T3CONbits.RD16 = 1;
    T3CONbits.T3CKPS1 = 1;
    T3CONbits.T3CKPS0 = 1;
    T3CONbits.TMR3CS = 0;
    T3CONbits.TMR3ON = 1;

    TMR3L = 0xDC;
    TMR3H = 0x0B;
    
    
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
    if(INTCONbits.INT0IF){
        INTCONbits.INT0IF = 0;  // Limpa flag do INT0
        contador++;
        if(contador>999){
            contador = 999;
        }
    
        WriteCmdXLCD(0xC7); // Manda o cursor para o meio da segunda linha

        putcXLCD (0x30 + contador/100);
        putcXLCD (0x30 + contador/10);
        putcXLCD (0x30 + contador%10);
    }
    if(PIR1bits.TMR1IF){
        PIR1bits.TMR1IF = 0;
        TMR1H = 0;             // preset for timer1 MSB register
        TMR1L = 0xB4;             // preset for timer1 LSB register
        contat1++;
        if(contat1 == 10){
            contat1 = 0;
            PORTCbits.RC0 = !PORTCbits.RC0;
            //atualizat1 = 1;
        }
    }
}

void interrupt low_priority LowPriorityISR(void)
{
    if (INTCON3bits.INT1IF){
        INTCON3bits.INT1IF = 0;  // Limpa flag do INT1
        if(contador == 0){
            contador = 0;
        } else {
            contador--;
        }
    
        WriteCmdXLCD(0xC7); // Manda o cursor para o meio da segunda linha

        putcXLCD (0x30 + contador/100);
        putcXLCD (0x30 + contador/10);
        putcXLCD (0x30 + contador%10);
    }
    if(PIR2bits.TMR3IF){
        PIR2bits.TMR3IF = 0;
        TMR3L = 0xDC;
        TMR3H = 0x0B;
        PORTCbits.RC1 = !PORTCbits.RC1;
    }
    
}