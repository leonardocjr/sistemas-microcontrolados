#include <xc.h>
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

// Funções das ISRs
void interrupt HighPriorityISR(void)
{
    INTCONbits.INT0IF = 0;  // Limpa flag do INT0
    PORTD = 0x5B;
    __delay_ms(1000); // 1000ms
}
void interrupt low_priority LowPriorityISR(void)
{
    INTCON3bits.INT1IF = 0; // Limpa flag do INT1
    PORTD = 0x06;
    __delay_ms(1000); // 1000ms
}
// Função Principal
void main(void)
{
    TRISA = 0x00; // PORTA DE SAIDA A
    TRISB = 0x03; // PORTA DE ENTRADA B
    TRISD = 0x00; // PORTA DE SAIDA D
    TRISE = 0x00; // PORTA DE SAIDA E
    
    INTCON2bits.RBPU = 0;  // Pull-up habilitado no PORTB
    
    // INT0 é de alta prioridade
	INTCONbits.INT0IF = 0;   // Limpa flag do INT0
	INTCON2bits.INTEDG0 = 0; // Transição de descida
	INTCONbits.INT0IE = 1;   // INT0

	// INT1 é de baixa prioridade
	INTCON3bits.INT1IF = 0;  // Limpa flag do INT1
	INTCON2bits.INTEDG1 = 0; // Transição de descida
	INTCON3bits.INT1IP = 0;
	INTCON3bits.INT1IE = 1;

	RCONbits.IPEN = 1;   // Habilita prioridades de interrupção
	INTCONbits.GIEL = 1; // Habilita interrupção de baixa prior.
	INTCONbits.GIEH = 1; // Habilita interrupção de alta prior.
    
    
    PORTEbits.RE2 = 1;
    while(1)
    {
        PORTD = 0x3F;
    }
    
}