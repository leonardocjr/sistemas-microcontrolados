#include <xc.h>
#include "nxlcd.h"
#include <stdio.h>
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config CPUDIV = OSC1_PLL2 // OSC/1 com PLL off
#pragma config WDT = OFF // Watchdog desativado
#pragma config LVP = OFF // Desabilita grava��o em baixa
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR
#define _XTAL_FREQ 20000000 // uC opera com cristal de 20 MHz
#pragma config CCP2MX = ON // Pino RC1 utilizado em CCP2

unsigned char contador = 0;
unsigned long int result = 0;
unsigned long int result2 = 0;
unsigned long int valorconv = 0;
unsigned long int dconst = 768; //(PR2+1)*4

int dc = 0;
float dc2 = 0.0;
unsigned long int vent = 0;
unsigned long int pot = 0;
unsigned int leiturapotenciometro = 0;
unsigned long int temp = 350;

void interrupt HighPriorityISR(void) {
    INTCONbits.TMR0IF = 0; // limpa a flag
    TMR0L = 100;
    ADCON0bits.GO_DONE = 1;
    if (contador == 100) { // 30Hz
        contador = 1;
        leiturapotenciometro = 1;
        ADCON0bits.CHS1 = 1;
        ADCON0bits.CHS0 = 1;
        ADCON0bits.GO_DONE = 1;
        //Voltar para o AN0
    } else {
        ADCON0bits.CHS1 = 0;
        ADCON0bits.CHS0 = 0;
        ADCON0bits.GO_DONE = 1; // INICIA a conversao
    }
}

void interrupt low_priority LowPriorityISR(void) {
    PIR1bits.ADIF = 0;
    contador++;
    valorconv = 256 * ADRESH + ADRESL;
    if (leiturapotenciometro) {
        leiturapotenciometro = 0;
        result2 = valorconv;
    } else {
        result = valorconv * 4.887585533;
    }

    if (INTCON3bits.INT1IF) {
        INTCON3bits.INT1IF = 0; // Limpa flag do INT1
        if (temp < 500)
            temp=temp+10;
            __delay_ms(500);
    }
    if (INTCON3bits.INT2IF) {
        INTCON3bits.INT2IF = 0; // Limpa flag do INT2
        if (temp > 350)
            temp=temp-10;
            __delay_ms(500);
        }
}

void main(void) {
    int cent, aux, dez, uni;
    int dez2, aux2, uni2;
    int porcentagem, cent3, aux3, dez3, uni3;
    //Configuracao entradas e saidas
    TRISA = 0xFF; // RA0 e RA3 como entradas
    TRISC = 0x00; // RC1 e RC2 como saidas
    TRISB = 0XFF; // RB1 e RB2 como entradas

    //Configura��o para o algoritmo de varia��o da raz�o c�clica (CCP1)
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
    

    // Configuracao conversor A/D
    PIE1bits.ADIE = 1; // ativa o bit de interrupcao
    PIR1bits.ADIF = 0; // limpa a flag
    IPR1bits.ADIP = 0; // prioridade baixa do A/D

    //Configura o A/D estar no RA0/AN0
    ADCON0bits.CHS3 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;
    ADCON0bits.ADON = 1;

    // Configuracao e habilita interrupcoes globais
    RCONbits.IPEN = 1; // Habilitar niveis prioridades de interrupcao
    INTCONbits.GIEH = 1; // Habilitar interrupcoes globais
    INTCONbits.GIEL = 1; // Habilitar interrupcoes de perif�ricos

    // Configurar interrupcoes externas INT1 (RB1) e INT2 (RB2)
    INTCON3bits.INT1IE = 1; // Habilitar INT1
    INTCON3bits.INT2IE = 1; // Habilitar INT2
    INTCON2bits.INTEDG1 = 0; // Configurar borda de descida para INT1
    INTCON2bits.INTEDG2 = 0; // Configurar borda de descida para INT2
    INTCON3bits.INT1IF = 0; // Limpar flag INT1
    INTCON3bits.INT2IF = 0; // Limpar flag INT2
    INTCON3bits.INT1IP = 0; // Baixa do INT1 prioridade
    INTCON3bits.INT2IP = 0; // Baixa do INT2 prioridade
    INTCON2bits.RBPU = 0; // Pull-up habilitado no PORTB

    // Configuracao do Timer0
    T0CON = 0b11000101; // timer on, 8 bits, clock interno, borda de subida, pre scaler de 64
    TMR0L = 100;
    TRISAbits.TRISA0 = 1; // input
    ADCON1 = 0b00001011;
    ADCON2 = 0b10010101; // justifica a direita
    INTCONbits.TMR0IF = 0; // zera flag
    INTCONbits.TMR0IE = 1; // habilita interrupcao do timer
    INTCON2bits.TMR0IP = 1; // nivel de prioridade alta

    // Freq = Fosc / (4 * prescaler * (65536 - TMR0))
    // O valor de TMR0 = 65536 - (Fosc / (4 * prescaler * Freq))
    //tmr0 = 65536 - (20000000 / (4 * 2 * 1500));
    TMR0L = 100;

    //Configura��o para o CCP2 em modo PWM
    CCP2CONbits.CCP2M3 = 1;
    CCP2CONbits.CCP2M2 = 1;

    //Inicializa��o do LCD
    OpenXLCD(FOUR_BIT & LINES_5X7);
    WriteCmdXLCD(0x01);
    __delay_ms(2);


    //WriteCmdXLCD(0x89);
    //putsXLCD("Atual: ");


    while (1) {
        if (contador == 100) {
            
            pot = 537 + (0.22 * result2); // (((PR2+1)*4)-((PR2+1)*4)*0.7) / 1023
            CCPR2L = (char) pot >> 2;
            CCP2CONbits.DC2B1 = (pot >> 1) % 2;
            CCP2CONbits.DC2B0 = pot % 2;
            
            cent = result / 100;
            aux = result % 100;
            dez = aux / 10;
            uni = aux % 10;
            WriteCmdXLCD(0x80);
            putsXLCD("Ta:");
            putcXLCD(0x30 + cent);
            putcXLCD(0x30 + dez);
            putcXLCD(',');
            putcXLCD(0x30 + uni);
            
            dez2 = temp/100;
            aux2 = temp%100;
            uni2 = aux2/10;
            WriteCmdXLCD(0x89);
            putsXLCD("Tr:");
            putcXLCD(0x30 + dez2);
            putcXLCD(0x30 + uni2);
            
            dc = (result-temp);
            if(dc<=0)
                dc = 0;
            else if(dc>1)
                dc = 1;
            
            vent = 768*dc;
            CCPR1L = (char) (vent >> 2);
            CCP1CONbits.DC1B0 = vent % 2;
            CCP1CONbits.DC1B1 = (vent >> 1) % 2;
            
            porcentagem = (-(result-temp))*100;
            WriteCmdXLCD(0xC3);
            putsXLCD("Razao:");
            cent3 = porcentagem / 100;
            aux3 = porcentagem % 100;
            dez3 = aux3 / 10;
            uni3 = aux3 % 10;
            putcXLCD(0x30 + cent3);
            putcXLCD(0x30 + dez3);
            putcXLCD(0x30 + uni3);
            putcXLCD('%');
        }
    }
}
