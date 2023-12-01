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

#pragma config	CCP2MX = ON // Pino RC1 utilizado em CCP2

unsigned int razao = 0, atualiza = 0, contaccp1 = 0, atualizaccp1 = 0;
int contador = 5;
void main (void){

    TRISCbits.RC1 = 0;
    
    //Ativa as interrupções
    
    RCONbits.IPEN = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    PIR1bits.CCP1IF = 0;
    PIE1bits.CCP1IE = 1;
    
    //Configuração para o PWM
    T2CONbits.TMR2ON = 1;
    T2CONbits.T2CKPS1 = 1;
    PR2 = 124;
    CCP2CONbits.CCP2M3 = 1;
    CCP2CONbits.CCP2M2 = 1;
    
    //Configuração para o algoritmo de variação da razão cíclica
    
    T1CONbits.RD16 = 1;
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.TMR1ON = 1;
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.CCP1M2 = 0;
    CCP1CONbits.CCP1M1 = 1;
    CCP1CONbits.CCP1M0 = 1;
    CCPR1L = 0x12;
    CCPR1H = 0x7A;
    
    //Inicialização do LCD
    OpenXLCD(FOUR_BIT & LINES_5X7); // Modo 4 bits de dados e caracteres 5x7
    WriteCmdXLCD(0x01); // Limpa o LCD com retorno do cursor
    __delay_ms(2);
    
    WriteCmdXLCD(0x82);
    
    WriteCmdXLCD(0x0C); // Desliga cursor
    
    putrsXLCD ("Razao Ciclica"); // 
    WriteCmdXLCD(0xC4); 

    putrsXLCD ("PWM");
    
    WriteCmdXLCD(0xC8); 
    putcXLCD (0x30 + contador/10);
    putcXLCD (0x30 + (int)(contador));
    putcXLCD (0x30);
    
    WriteCmdXLCD(0xCB); 
    putrsXLCD ("%");
    razao = (int)(contador * 50);
    
    CCPR2L = razao >> 2;
    CCP2CONbits.DC2B1 = (razao>>1)%2;
    CCP2CONbits.DC2B0 = razao%2;
    
    
    while(1){
        
        if(atualizaccp1){
            atualizaccp1 = 0;
            if(contaccp1 > 19){
                contador++;
                atualiza = 1;
                contaccp1 = 0;
            }
        }
        
        if(atualiza){
            atualiza = 0;
            razao = (int)(contador * 50);
            if (contador == 10){
                
                CCPR2L = razao >> 2;
                CCP2CONbits.DC2B1 = (razao>>1)%2;
                CCP2CONbits.DC2B0 = razao%2;
        
                WriteCmdXLCD(0xC8); 
                putcXLCD (0x31);
                putcXLCD (0x30);
                putcXLCD (0x30);
    
                WriteCmdXLCD(0xCB); 
                putrsXLCD ("%");
                contador = -1;
            }else{
                CCPR2L = razao >> 2;
                CCP2CONbits.DC2B1 = (razao>>1)%2;
                CCP2CONbits.DC2B0 = razao%2;
        
                WriteCmdXLCD(0xC8); 
                putcXLCD (0x30 + contador/10);
                putcXLCD (0x30 + (int)(contador));
                putcXLCD (0x30);
    
                WriteCmdXLCD(0xCB); 
                putrsXLCD ("%");

                }
            }
        }
    
}


void interrupt NoPriorityISR(void)
{
	    PIR1bits.CCP1IF = 0;
        contaccp1++; 
        atualizaccp1 = 1;
}