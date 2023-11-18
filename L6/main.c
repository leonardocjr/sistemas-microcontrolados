#include <xc.h>
#pragma config PLLDIV = 5 // PLL para 20MHz
#pragma config CPUDIV = OSC1_PLL2 // PLL desligado
#pragma config FOSC = HS // Fosc = 20MHz; Tcy    = 200ns
#pragma config WDT = OFF // Watchdog timer desativado
#pragma config PBADEN = OFF // Pinos do PORTB começam como digitais
#pragma config LVP = OFF // Desabilita gravação em baixa tensão
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR e desabilita RE3 como I/O
#define _XTAL_FREQ 20000000 // uC opera com cristal de 20 MHz

#define atraso 4 // Valor de update em ms

void escreve(int i) {
    switch (i) {
        case 0:
            PORTD = 0b00111111;
            break;
        case 1:
            PORTD = 0b00000110;
            break;
        case 2:
            PORTD = 0b01011011;
            break;
        case 3:
            PORTD = 0b01001111;
            break;
        case 4:
            PORTD = 0b01100110;
            break;
        case 5:
            PORTD = 0b01101101;
            break;
        case 6:
            PORTD = 0b01111101;
            break;
        case 7:
            PORTD = 0b00000111;
            break;
        case 8:
            PORTD = 0b01111111;
            break;
        case 9:
            PORTD = 0b01100111;
            break;
    }
}

void altera(int i) {

    PORTAbits.RA2 = 0;
    PORTEbits.RE0 = 0;
    PORTEbits.RE2 = 0;
    escreve(i / 1000);
    PORTAbits.RA5 = 1;
    i = i % 1000;
    __delay_ms(atraso);

    PORTAbits.RA5 = 0;
    PORTEbits.RE0 = 0;
    PORTEbits.RE2 = 0;
    escreve(i / 100);
    PORTAbits.RA2 = 1;
    i = i % 100;
    __delay_ms(atraso);

    PORTAbits.RA5 = 0;
    PORTAbits.RA2 = 0;
    PORTEbits.RE2 = 0;
    escreve(i / 10);
    PORTEbits.RE0 = 1;
    i = i % 10;
    __delay_ms(atraso);


    PORTAbits.RA5 = 0;
    PORTAbits.RA2 = 0;
    PORTEbits.RE0 = 0;
    escreve(i);
    PORTEbits.RE2 = 1;
    __delay_ms(atraso);
}

int n = 0;

void interrupt HighPriorityISR(void) {

    if (n + 10 <= 9999) n += 10;
    __delay_ms(100);
    INTCONbits.INT0IF = 0;

}

void interrupt low_priority LowPriorityISR(void) {
    if (n - 5 >= 0) n -= 5;
    __delay_ms(100);
    INTCON3bits.INT1IF = 0;
}

void main(void) {

    TRISD = 0x00;
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE0 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA5 = 0;
    TRISB = 0b00000011;

    INTCON2bits.RBPU = 0;
    PORTBbits.RB1 = 1;
    PORTBbits.RB0 = 1;

    PORTEbits.RE2 = 0;
    PORTEbits.RE0 = 0;
    PORTAbits.RA2 = 0;
    PORTAbits.RA5 = 0;

    INTCONbits.INT0IF = 0;
    INTCON2bits.INTEDG0 = 0;
    INTCONbits.INT0IE = 1;

    INTCON3bits.INT1IF = 0;
    INTCON2bits.INTEDG1 = 0;
    INTCON3bits.INT1IP = 0;
    INTCON3bits.INT1IE = 1;

    RCONbits.IPEN = 1;
    INTCONbits.GIEL = 1;
    INTCONbits.GIEH = 1;



    while (1) {
        altera(n); //__delay_ms(1000);
    }
}
