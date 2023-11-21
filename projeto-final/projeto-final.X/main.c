#include <xc.h>
#include "nxlcd.h"
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config CPUDIV = OSC1_PLL2 // OSC/1 com PLL off
#pragma config WDT = OFF // Watchdog desativado
#pragma config LVP = OFF // Desabilita gravação em baixa
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR
#define _XTAL_FREQ 20000000 // uC opera com cristal de 20 MHz
unsigned char contador = 0;
unsigned int valorconv = 0;
int duty = 768; // *porcentagem

// 1500hz
void interrupt HighPriorityISR(void){
    INTCONbits.TMR0IF = 0; // limpa a flag
    ADCON0bits.GO_DONE = 1; // INICIA a conversao
    valorconv = 256 * ADRESH + ADRESL;
    TMR0H =  0xF9;
    TMR0L = 0x7D;

    contador++;
    if(contador == 50){
        contador = 0;

    }
}

void main(void) {
    int mil, cent, dez, uni;
    int aux = 0;
    //Configuracao entradas e saidas
    TRISA = 0xFF; // RA0 e RA3 como entradas
    TRISC = 0x00; // RC1 e RC2 como saidas
    TRISB = 0XFF; // RB0 e RB1 como entradas
    
    //Configuração para o algoritmo de variação da razão cíclica (CCP1)
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.CCP1M2 = 1;
    CCP1CONbits.CCP1M1 = 0;
    CCP1CONbits.CCP1M0 = 0;
    
    // Configuracao timer 2 e postscale
    T2CONbits.T2OUTPS3 = 0;
    T2CONbits.T2OUTPS2 = 0;
    T2CONbits.T2OUTPS1 = 0;
    T2CONbits.T2OUTPS0 = 0; 
    T2CONbits.TMR2ON = 1;
    T2CONbits.T2CKPS1 = 0;
    T2CONbits.T2CKPS0 = 1;
    // PR2 = (Tpwm / 4 x Tosc x (Preescaler do |TMR2)) - 1
    // (2^8)*(200*(10^-9))*4
    // PR2 = (1/6500)/(200*(10^-9)*4)-1 = 191
    PR2 = 191;
    
    
    CCPR1L = (char)(duty >> 2);
    CCP1CONbits.DC1B0 = duty%2;
    CCP1CONbits.DC1B1 = (duty >> 1)%2;

    // Configuracao conversor A/D
    PIE1bits.ADIE = 1; // desativa o bit de interrupcao
    PIR1bits.ADIF = 0; // limpa a flag
    IPR1bits.ADIP = 0; // prioridade baixa do A/D
    
    //Configura o A/D estar no RA0/AN0
    ADCON0bits.CHS3 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;
    
    // Configuracao e habilita interrupcoes globais
    RCONbits.IPEN = 1;    // Habilitar niveis prioridades de interrupcao
    INTCONbits.GIEH = 1;   // Habilitar interrupcoes globais
    INTCONbits.GIEL = 1;  // Habilitar interrupcoes de periféricos
    
    // Configurar interrupcoes externas INT1 e INT2
    INTCON3bits.INT1IE = 1; // Habilitar INT1
    INTCON3bits.INT2IE = 1; // Habilitar INT2
    INTCON2bits.INTEDG1 = 0; // Configurar borda de descida para INT1
    INTCON2bits.INTEDG2 = 0; // Configurar borda de descida para INT2
    INTCON3bits.INT1IF = 0; // Limpar flag INT1
    INTCON3bits.INT2IF = 0; // Limpar flag INT2
    INTCON3bits.INT1IP = 0; // Baixa do INT1 prioridade
    INTCON3bits.INT2IP = 0; // Baixa do INT2 prioridade
    
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
    
    //Inicialização do LCD
    OpenXLCD(FOUR_BIT & LINES_5X7); 
    WriteCmdXLCD(0x01); 
    __delay_ms(2);

    WriteCmdXLCD(0x80);
    putsXLCD("Ref: ");
    WriteCmdXLCD(0x89);
    putsXLCD("Atual: ");
    WriteCmdXLCD(0xC3);
    putsXLCD("Razao: ");
    
    while(1){
        long int resultado = 0;
        resultado = (valorconv * 4.89);
        mil = resultado / 1000;
        aux = resultado % 1000;
        cent = aux / 100;
        aux = aux % 100;
        dez = aux / 10;
        uni = aux % 10;
       

    }
    
}
