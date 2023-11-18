#include "nxlcd.h"
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config CPUDIV = OSC1_PLL2 // OSC/1 com PLL off
#pragma config WDT = OFF // Watchdog desativado
#pragma config LVP = OFF // Desabilita gravação em baixa
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR

#define _XTAL_FREQ 20000000

unsigned char cont = 1;
unsigned int valorconv = 0;

void interrupt HighPriorityISR(void) {
    INTCONbits.TMR0IF = 0; // limpa a flag
    TMR0L = 100;
    ADCON0bits.GO_DONE = 1; // INICIA a conversao
    valorconv = 256 * ADRESH + ADRESL;
}

void interrupt low_priority LowPriorityISR(void) {
    PIR1bits.ADIF = 0; // limpa a flag
    cont++;
}

void main(void) {
    int mil, cent, dez, uni;
    int aux = 0;

    TRISD = 0x00;
    TRISE = 0x00;

    PIE1bits.ADIE = 1; // ativa o bit de interrupcao
    PIR1bits.ADIF = 0; // limpa a flag
    IPR1bits.ADIP = 0; // prioridade baixa do A/D

    RCONbits.IPEN = 1; // habilita niveis de interrupcoes
    INTCONbits.GIEL = 1; // habilita interrupcao baixa
    INTCONbits.GIEH = 1; //habilita interrupcao alta

    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1; // habilita interrupcao do timer
    INTCON2bits.TMR0IP = 1;

    OpenXLCD(FOUR_BIT & LINES_5X7);
    WriteCmdXLCD(0x01);
    __delay_ms(2);
    WriteCmdXLCD(0x01);

    TMR0L = 100;
    T0CON = 0b11010101; // timer on, 8 bits, clock interno, borda de subida, pre scaler de 64
    TRISAbits.TRISA0 = 1; // input

    ADCON1 = 0b00001110;
    ADCON0 = 0b00000001;
    ADCON2 = 0b10010101; // justifica a direita

    __delay_us(10);

    while (1) {
        if (cont == 100) {

            long int result = 0;
            cont = 1;
            PORTDbits.RD0 = !PORTDbits.RD0;

            result = (valorconv * 4.89); // aproximando 5000/1023

            mil = result / 1000;
            aux = result % 1000;
            cent = aux / 100;
            aux = aux % 100;
            dez = aux / 10;
            uni = aux % 10;

            WriteCmdXLCD(0x80);
            putsXLCD(" Conversor A / D ");

            WriteCmdXLCD(0xC0);
            putsXLCD("V_AN0 = ");
            putcXLCD(0x30 + mil);
            putcXLCD(',');
            putcXLCD(0x30 + cent);
            putcXLCD(0x30 + dez);
            putcXLCD(0x30 + uni);
            putsXLCD(" V");

        }
    }
}