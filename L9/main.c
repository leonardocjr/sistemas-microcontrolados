#include <xc.h>
#include "nxlcd.h"
// Configs
#define _XTAL_FREQ 20000000  // Frequência do cristal (20 MHz)
#define PWM_FREQ 4000        // Frequência desejada do PWM (4 kHz)
#define PWM_PERIOD ((_XTAL_FREQ / (4 * PWM_FREQ)) - 1)
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config CPUDIV = OSC1_PLL2 // OSC/1 com PLL off
#pragma config WDT = OFF // Watchdog desativado
#pragma config LVP = OFF // Desabilita gravação em baixa
#pragma config DEBUG = ON // Habilita debug
#pragma config MCLRE = ON // Habilita MCLR
#pragma config	CCP2MX = ON // Pino RC1 utilizado em CCP2

unsigned int dutyCycle = 0;
unsigned int lcd_updated = 1; // Inicializado como 1 para atualizar o LCD na inicialização
unsigned char valor = 0;

void interrupt NoPriorityISR(void) {
    if (INTCONbits.INT0IF) {
        dutyCycle += 10;
        if (dutyCycle > 100) {
            dutyCycle = 100;
        }
        valor = dutyCycle*312*0.01;
        CCPR2L = (char) (valor >> 2);
        lcd_updated = 1;
        INTCONbits.INT0IF = 0;
    }
    if (INTCON3bits.INT1IF) {
        dutyCycle -= 10;
        if (dutyCycle < 0) {
            dutyCycle = 0;
        }
        CCPR2L = (char) (valor >> 2);
        lcd_updated = 1;
        INTCON3bits.INT1IF = 0;
    }
}

void main() {
    // Configurar as portas como entrada ou saída
    TRISB = 0xFF; // RB0 e RB1 como entrada
    TRISC = 0x00; // RC1 como saída

    // Configurar interrupções externas INT0 e INT1
    INTCON2bits.INTEDG0 = 0; // INT0 na borda de descida
    INTCON2bits.INTEDG1 = 0; // INT1 na borda de descida
    INTCONbits.INT0IE = 1; // Habilitar INT0
    INTCONbits.INT0IF = 0;
    INTCON3bits.INT1IE = 1; // Habilitar INT1
    INTCON3bits.INT1IF = 0;
    RCONbits.IPEN = 0; // Interrupt Priority Enable bit
    INTCONbits.PEIE = 1; //Global Interrupt Enable bit
    INTCONbits.GIE = 1; //Peripheral Interrupt Enable bit

    // Configurar CCP2 para gerar o sinal PWM

    // PR2 = 250us / (4 * 50ns * 16) ? 1 = 77,125 = ~77
    T2CON = 0b00000111;
    PR2 = 77;
    CCP2CON = 0b00011100; // Modo PWM
    CCPR2L = (char) (valor >> 2);
    CCP2CONbits.DC2B0 = valor % 2;
    CCP2CONbits.DC2B1 = (valor >> 1) % 2;

    // Inicializar o LCD
    OpenXLCD(FOUR_BIT & LINES_5X7);
    WriteCmdXLCD(0x01); // Limpar LCD
    __delay_ms(2); // Aguardar limpar LCD
    WriteCmdXLCD(0x0C); // Desligar cursor

    // Exibir informações iniciais no LCD
    WriteCmdXLCD(0x80); // Posição: início da 1ª linha
    putsXLCD("12 Vdc Lampada");
    // Atualizar a porcentagem inicial no LCD
    WriteCmdXLCD(0xC0); // Posição: início da 2ª linha
    putrsXLCD("Duty-Cycle = 0%");

    // Inicializar dutyCycle com 0%
    dutyCycle = 0;

    while (1) {
        // Verificar se o LCD precisa ser atualizado
        if (lcd_updated) {
            WriteCmdXLCD(0xC0 + 12); // Posição: após "Duty-Cycle = "
            putcXLCD(0x30 + dutyCycle / 10);
            putcXLCD(0x30 + dutyCycle % 10);
            putrsXLCD("%");
            lcd_updated = 0;
        }

    }
}