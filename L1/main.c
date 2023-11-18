#include <xc.h>
#include <stdio.h>
#pragma config FOSC = HS // Fosc = 20MHz; Tcy = 200ns
#pragma config CPUDIV = OSC1_PLL2 // OSC/1 com PLL off
#pragma config WDT = OFF // Watchdog desativado
#pragma config LVP = OFF // Desabilita gravação em baixa
#pragma config DEBUG = ON // Habilita debug
#pragma config PBADEN = OFF // Habilita MCLR
#pragma config MCLRE = ON // Habilita MCLR

void putch(unsigned char data) {
    while (!PIR1bits.TXIF) // wait until the transmitter is ready
        continue;
    TXREG = data; // send one character
}

void init_uart(void) {
    TXSTAbits.TXEN = 1; // enable transmitter
    RCSTAbits.SPEN = 1; // enable serial port
}
// Exercício 0:
void main(void) {
    init_uart();

    while (1) {
        printf("\n\nApenas um exemplo de uso do printf().\n");
    }
}

// Exercício 1:
void main(void)
{
 init_uart();

 int vet[3] = {3, 5, 7};
 for(int i = 0; i < 3; i++){
     printf("\nO quadrado de %d é: %.0f\n", vet[i], (vet[i]*vet[i]));
 }

}

// Exercício 2:
void main(void)
{
 init_uart();
 int soma = 0;
 for(int i = 1; i <= 10; i++){
     soma+=i;
 }
 printf("%d\n\n", soma);

}

// Exercício 3:
void main(void)
{
 init_uart();
 int num = 4321;
 int milhar = num/1000;
 num = num-(milhar*1000);
 int centena = num/100;
 num = num-(centena*100);
 int dezena = num/10;
 num = num-(dezena*10);
 int unidade = num;
 printf("Milhar = %d\nCentena = %d\nDezena = %d\nUnidade = %d\n\n\n", milhar, centena, dezena, unidade);
 
}
