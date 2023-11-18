#include <xc.h>
#include "nxlcd.h"
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config CPUDIV = OSC1_PLL2 // OSC/1 com PLL off
#pragma config WDT = OFF // Watchdog desativado
#pragma config LVP = OFF // Desabilita gravação em baixa
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR
//#pragma config CCP1MX = ON // Pino RC1 utilizado em CCP2
#define _XTAL_FREQ 20000000 // uC opera com cristal de 20 MHz

void interrupt NoPriorityISR(void) {
    if(INTCONbits.TMR0IF)
        INTCONbits.TMR0IF = 0; // limpa a flag
}


void main(void) {
    
    //Configuracao entradas e saidas
    TRISA = 0xFF; // RA0 e RA3 como entradas
    TRISC = 0x00; // RC1 e RC2 como saidas
    TRISB = 0XFF // RB0 e RB1 como entradas
    
    // Configurar interrupcoes externas INT1 e INT2
    INTCON3bits.INT1IE = 1; // Habilitar INT1
    INTCON3bits.INT2IE = 1; // Habilitar INT2
    INTCON2bits.INTEDG1 = 0; // Configurar borda de descida para INT1
    INTCON2bits.INTEDG2 = 0; // Configurar borda de descida para INT2
    INTCON3bits.INT1IF = 0; // Limpar flag INT1
    INTCON3bits.INT2IF = 0; // Limpar flag INT2
    INTCON3bits.INT1IP = 0; // Baixa do INT1 prioridade
    INTCON3bits.INT2IP = 0; // Baixa do INT2 prioridade
    
    // Configuracao e habilita interrupcoes globais
    RCONbits.IPEN = 0;    // Desabilitar niveis prioridades de interrupcao
    INTCONbits.GIE = 1;   // Habilitar interrupcoes globais
    INTCONbits.PEIE = 1;  // Habilitar interrupcoes de periféricos
    
    // Configuracao conversor A/D
    PIE1bits.ADIE = 1; // ativa o bit de interrupcao
    PIR1bits.ADIF = 0; // limpa a flag
    IPR1bits.ADIP = 1; // prioridade alta do A/D
    
    // Configuracao do Timer0
    T0CONbits.TMR0ON = 1;
    T0CONbits.T08BIT = 0;   // Timer0 configurado como contador de 16 bits
    T0CONbits.T0CS = 0;     // Fonte do clock é a instrução de máquina (Fosc/4)
    T0CONbits.PSA = 0;      // O prescaler é atribuído ao Timer0
    T0CONbits.T0PS2 = 0;    // Configura o prescaler para 1:2
    T0CONbits.T0PS1 = 0;
    T0CONbits.T0PS0 = 0;
    INTCONbits.TMR0IF = 0; // zera flag
    INTCONbits.TMR0IE = 1; // habilita interrupcao do timer
    INTCON2bits.TMR0IP = 1; // nivel de prioridade alta

    // Freq = Fosc / (4 * prescaler * (65536 - TMR0))
    // O valor de TMR0 = 65536 - (Fosc / (4 * prescaler * Freq))
    //tmr0 = 65536 - (20000000 / (4 * 2 * 1500));
    TMR0H = 0xF9;  // Configura a parte alta do registrador TMR0
    TMR0L = 0x7D;  // Configura a parte baixa do registrador TMR0

    //Configuração para o CCP2 em modo PWM
    CCP2CONbits.CCP2M3 = 1;
    CCP2CONbits.CCP2M2 = 1;
    
    //Configuração para o algoritmo de variação da razão cíclica
    // PR2 = 20000000 / (4 * 6500 * 16) - 1 = 47
    T2CONbits.TMR2ON = 1;
    T2CONbits.T2CKPS1 = 1;
    PR2 = 47;
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.CCP1M2 = 0;
    CCP1CONbits.CCP1M1 = 1;
    CCP1CONbits.CCP1M0 = 1;
    CCPR1L = 0x12;
    CCPR1H = 0x7A;
    
    //Inicialização do LCD
    OpenXLCD(FOUR_BIT & LINES_5X7); 
    WriteCmdXLCD(0x01); 
    __delay_ms(2);

    while(1){
        WriteCmdXLCD(0x80);
        putsXLCD("Ref: ");
        WriteCmdXLCD(0x89);
        putsXLCD("Atual: ");
        WriteCmdXLCD(0xC3);
        putsXLCD("Razao: ");
        __delay_ms(33); // 33 ms => 30Hz
    }
    
}
