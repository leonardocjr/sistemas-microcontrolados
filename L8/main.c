#include <xc.h>
#include "nxlcd.h"
#pragma config PLLDIV = 5 // PLL para 20MHz
#pragma config CPUDIV = OSC1_PLL2 // Fosc/1 e PLL desligado
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config WDT = OFF // Watchdog desativado
#pragma config PBADEN = OFF // Pinos do PORTB começam como digitais
#pragma config LVP = OFF // Desabilita gravação em baixa tensão
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR e desabilita RE3 como I/O
#define _XTAL_FREQ 20000000 // uC opera com cristal de 20 MHz

unsigned char auxs = 0xFF;
unsigned char auxm = 0xFF;
unsigned char auxh = 0xFF;
int sec = 0;
int min = 0;
int hor = 0;
int x = 0;

// Funções das ISRs
void interrupt NoPriorityISR(void)
{
    if(PIR1bits.CCP1IF == 1){
        PIR1bits.CCP1IF = 0; // Limpa flag do CCP1IF
        x++;
    }
}

// Função Principal
void main(void)
{
    TRISD = 0;				// PORT D é todo saída
    //Inicialização do LCD
    OpenXLCD(FOUR_BIT & LINES_5X7); 
    WriteCmdXLCD(0x01); 
    __delay_ms(2); 
    
    WriteCmdXLCD(0x0C);
    
    WriteCmdXLCD(0x84);
    putsXLCD (" RELOGIO ");
    

    
    RCONbits.IPEN = 0; // Desabilita prioridades de interrupção
    INTCONbits.GIE = 1; // Habilita as interrupções
    INTCONbits.PEIE = 1; // Habilita prioridades dos periféricos
    
    T1CONbits.RD16 = 0;
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.T1RUN = 1;
    T1CONbits.T1OSCEN = 1;
    T1CONbits.T1SYNC = 0;
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;
    
    T3CONbits.T3CCP2 = 0;
    T3CONbits.T3CCP1 = 0;
    
    // Configura CCP1
	// 0,1s = 200ns * 8 * 62.500
	CCPR1H = 0xF4;
	CCPR1L = 0x24;
	CCP1CON = 0b00001011;	// Compara com Special Event Trigger // zerando TMR3H e TMR3L
 	        
	// Interrupção de CCP1
	PIE1bits.CCP1IE = 1;	// Habilita interrupção do CCP1
	while(1){
       WriteCmdXLCD(0xC7);
       putcXLCD (auxs);
        if(x == 10){
            x = 0;
            sec++;
            auxs = sec;
        }
        if(sec == 60){
            sec = 0;
            min++;
            auxm = min;
        }
        if(min == 60){
            min = 0;
            hor++;
            auxh = hor;
        }
        if(hor == 24){
            x = 0;
            sec = 0;
            min = 0;
            hor = 0;
        }
    };        // Loop infinito vazio
}