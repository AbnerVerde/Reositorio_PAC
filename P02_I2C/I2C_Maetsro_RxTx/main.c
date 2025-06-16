//**************************
//  Maestro Transmisor y Receptor.
//  Si el maestro recibe el dato almacenado en RXCompare enciende led
//  El maestro mandara informacion en cada ciclo
//                                /|\  /|\
//                   Arduino      10k  10k     MSP430F5529
//                   esclavo       |    |        maestro
//             -----------------   |    |   -----------------
//           -|XIN   PA5/UCB0SDA|<-|----+->|P3.0/UCB0SDA  XIN|-
//            |                 |  |       |                 | 32kHz
//           -|XOUT             |  |       |             XOUT|-
//            |      PA4/UCB0SCL|<-+------>|P3.1/UCB0SCL     |
//            |                 |          |             P1.0|--> LED
//**************************

#include <msp430.h>

unsigned char TXData = 0x01;    // Dato a enviar
unsigned char TXByteCtr = 1;    // Número de bytes a enviar

unsigned char RXData = 0x00;    // Dato recibido
unsigned char RXCompare = 0x41; // Valor esperado del esclavo

const int direcciones[] = {0x48, 0x44, 0x40}; // Direcciones esclavas
int i = 0;
int estado = 0;  // 0 = TX, 1 = RX

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;     // Desactiva watchdog
    P1DIR |= BIT0;                // LED como salida
    P1OUT &= ~BIT0;               // Apaga LED

    P3SEL |= BIT0 + BIT1;         // SDA y SCL

    UCB0CTL1 |= UCSWRST;                  // Reset para configurar
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC; // Modo maestro, I2C, síncrono
    UCB0CTL1 = UCSSEL_2 + UCSWRST;        // SMCLK como fuente
    UCB0BR0 = 12;                          // SCL = SMCLK / 12
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                 // Sal de reset
    UCB0IE |= UCTXIE + UCRXIE;            // Habilita TX y RX

    while (1)
    {
        // --- Transmisión ---
        estado = 0;                         // Estado TX
        TXByteCtr = 1;

        while (UCB0CTL1 & UCTXSTP);        // Espera STOP previo
        UCB0I2CSA = direcciones[i];        // Dirección del esclavo
        UCB0CTL1 |= UCTR + UCTXSTT;        // Modo TX + START

        __bis_SR_register(LPM0_bits + GIE); // Espera interrupción

        i = (i + 1) % 3;
        if (i == 0)
            TXData++; // Después de cada ciclo completo, incrementa

        __delay_cycles(100000); // Retardo entre TX y RX

        // --- Recepción ---
        estado = 1;                    // Estado RX

        while (UCB0CTL1 & UCTXSTP);    // Espera STOP previo
        UCB0I2CSA = 0x40;              // Dirección del esclavo a leer
        UCB0CTL1 &= ~UCTR;             // Modo recepción
        UCB0CTL1 |= UCTXSTT;           // Enviar START

        while (UCB0CTL1 & UCTXSTT);    // Espera a que se complete el START
        UCB0CTL1 |= UCTXSTP;           // Enviar STOP después del byte

        __bis_SR_register(LPM0_bits + GIE); // Espera interrupción RX

        // --- Comparación ---
        if (RXData != RXCompare)
        {
            P1OUT ^= BIT0; // Cambia el estado del LED si es incorrecto
            __delay_cycles(1000000);
        }

        __delay_cycles(1000000); // Espera entre rondas
    }
}

// ISR común para TX y RX
#if defined(_TI_COMPILER_VERSION) || defined(IAR_SYSTEMS_ICC_)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(_GNUC_)
void _attribute_ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR(void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(UCB0IV, 12))
    {
    case 0: break;
    case 2: break;
    case 4: break;
    case 6: break;
    case 8: break;
    case 10: // RXIFG
        RXData = UCB0RXBUF;
        __bic_SR_register_on_exit(LPM0_bits);// Sale de LPM al final de interrupción
        break;
    case 12: // TXIFG
        if (TXByteCtr)
        {
            UCB0TXBUF = TXData; //Cargar el dato al buffer
            TXByteCtr--; //Decrementa contador de bytes
        }
        else
        {
            UCB0CTL1 |= UCTXSTP; //Genera STOP
            UCB0IFG &= ~UCTXIFG; // Limpia la bandera TX
            __bic_SR_register_on_exit(LPM0_bits);// Sale de LPM al final de interrupción
        }
        break;
    default: break;
    }
}
