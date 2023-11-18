#include <xc.h>
#pragma config PLLDIV = 5 // PLL para 20MHz
#pragma config CPUDIV = OSC1_PLL2 // Fosc/1 e PLL desligado
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config WDT = OFF // Watchdog desativado
#pragma config PBADEN = OFF // Pinos do PORTB come�am como digitais
#pragma config LVP = OFF // Desabilita grava��o em baixa tens�o
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR e desabilita RE3 como I/O
#define _XTAL_FREQ 20000000 // uC opera com cristal de 20 MHz

// Fun��es das ISRs
void interrupt NoPriorityISR(void)
{
    if(INTCONbits.TMR0IF == 1){
        PORTDbits.RD0 ^= 1; // Acende D0
        INTCONbits.TMR0IF = 0; // Limpa flag do TMR0
        TMR0H = 0x67; // CARGA INICIAL PARA 500ms
        TMR0L = 0x6A; // CARGA INICIAL PARA 500ms
    }
    if(PIR2bits.TMR3IF == 1){
        PORTDbits.RD1 ^= 1; // Acende D1
        PIR2bits.TMR3IF = 0; // Limpa flag do TMR3
        TMR3H = 0x3C; // CARGA INICIAL PARA 80ms
        TMR3L = 0xB0; // CARGA INICIAL PARA 80ms
    }
}

// Fun��o Principal
void main(void)
{
    TRISD = 0;
    PORTDbits.RD0 = 1;
    PORTDbits.RD1 = 1;
    
    INTCONbits.TMR0IF = 0; // Limpa flag do TMR0
	INTCONbits.TMR0IE = 1; // Habilita interrup��o do TMR0
    
    PIR2bits.TMR3IF = 0; // Limpa flag do TMR3
    PIE2bits.TMR3IE = 1; // Habilita interrup��o do TMR3
    
    RCONbits.IPEN = 0; // Desabilita prioridades de interrup��o
    INTCONbits.GIEH = 1; // Habilita as interrup��es
    INTCONbits.PEIE = 1; // Habilita prioridades dos perif�ricos
    
    // TIMER 0 CONFIGS
    T0CON = 0b10010101; //Timer 0 on + 16 bits + borda de descida + Prescaler ON + (Prescaler=64)
    TMR0H = 0x67; // CARGA INICIAL
	TMR0L = 0x6A; // CARGA INICIAL
    
    // TIMER 3 CONFIGS
    T3CON = 0b10110001; // Timer 3 on + 16 bits + borda de descida + Prescaler ON + (Prescaler=8)
    TMR3H = 0x3C; // CARGA INICIAL
	TMR3L = 0xB0; // CARGA INICIAL
    
    while(1);
}