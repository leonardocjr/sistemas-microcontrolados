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
#define _XTAL_FREQ 20000000

void main(void) {
    ADCON1 = 0x0F; // PORT A e B são I/O digital

    TRISC = 0xFF; // PORTC é entrada

    TRISD = 0; // PORTD é saída
    PORTD = 0b11111111; // LED L0 inicia apagado
    int key = 0;
    while (1) // Loop infinito
    {
        if (PORTCbits.RC0 == 0) // Polling (nível lógico baixo)
        {
            if (key <= 3) {
                switch (key) {
                    __delay_ms(200);
                    case 0:
                        PORTDbits.RD0 = 0;
                        break;
                    case 1:
                        PORTDbits.RD1 = 0;
                        break;
                    case 2:
                        PORTDbits.RD2 = 0;
                        break;
                    case 3:
                        PORTDbits.RD3 = 0;
                        break;
                }
                key++;
            }

        } else if (PORTCbits.RC1 == 0) {
            if (key >= 0){
                __delay_ms(200);
                switch (key) {
                    case 3:
                        PORTDbits.RD3 = 1;
                        break;
                    case 2:
                        PORTDbits.RD2 = 1;
                        break;
                    case 1:
                        PORTDbits.RD1 = 1;
                        break;
                    case 0:
                        PORTDbits.RD0 = 1;
                        break;
                }
            key--;
        }
    }
}
}

