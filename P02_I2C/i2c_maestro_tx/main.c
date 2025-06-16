//  Maestro tx.
//                                /|\  /|\
//                Arduino Uno     10k  10k     MSP430F5529
//                   esclavo       |    |         maestro
//             -----------------   |    |   -----------------
//           -|XIN   PA5/UCB0SDA|<-|----+->|P3.0/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |             XOUT|-
//            |      PA4/UCB0SCL|<-+------>|P3.1/UCB0SCL     |
//            |                 |          |                 |
//**************************

#include <msp430.h>

/* Variables globales */
unsigned char TXData;       // Dato que se transmitirá
unsigned char TXByteCtr;    // Contador de bytes a transmitir
const int Arduino1 = 0x48;  //Direccion arduino numero 1
const int Arduino2 = 0x44;  //Direccion arduino numero 2
const int Arduino3 = 0x40;  //Direccion arduino numero 3
const int direcciones[] = {Arduino1, Arduino2, Arduino3}; // Arreglo de direcciones del arduino

int i = 0; // Índice del esclavo actual

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Detiene el watchdog
    P3SEL |= 0x03;                            // P3.0 y P3.1 como I2C (SDA y SCL)

    UCB0CTL1 |= UCSWRST;                      // Reset del módulo para configuración
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // Modo maestro, I2C, síncrono
    UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Fuente de reloj: SMCLK
    UCB0BR0 = 12;                             // Frecuencia de SCL = SMCLK / 12
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                     // Sal del reset para activar módulo
    UCB0IE |= UCTXIE;                         // Habilita interrupción por transmisión

    TXData = 0x01;                            // Primer dato a enviar

    while (1)
    {
        TXByteCtr = 1;                        // Enviar 1 byte

        while (UCB0CTL1 & UCTXSTP);           // Espera si hay STOP pendiente

        UCB0I2CSA = direcciones[i];           // Dirección del esclavo actual
        UCB0CTL1 |= UCTR + UCTXSTT;           // Modo transmisión y START

        __bis_SR_register(LPM0_bits + GIE);   // LPM0 con interrupciones habilitadas
        __no_operation();                     // NOP para debug

        i = (i + 1) % 3;                      // Pasa al siguiente esclavo

        if (i == 0)
            TXData++;                         // Después de 3 envíos, incrementa dato

        __delay_cycles(1000000);              // Retardo entre transmisiones
    }
}

// Interrupción del módulo USCI_B0
#if defined(_TI_COMPILER_VERSION) || defined(IAR_SYSTEMS_ICC_)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(_GNUC_)
void _attribute_ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(UCB0IV,12))
    {
    case  0: break;                           // Sin interrupciones
    case  2: break;                           // ALIFG
    case  4: break;                           // NACKIFG
    case  6: break;                           // STTIFG
    case  8: break;                           // STPIFG
    case 10: break;                           // RXIFG
    case 12:                                  // TXIFG (listo para transmitir)
        if (TXByteCtr)
        {
            UCB0TXBUF = TXData;               // Carga el dato al buffer de TX
            __delay_cycles(100);              // Pequeño retardo
            TXByteCtr--;                      // Decrementa contador de bytes
        }
        else
        {
            UCB0CTL1 |= UCTXSTP;              // Genera STOP
            UCB0IFG &= ~UCTXIFG;              // Limpia la bandera TX
            __bic_SR_register_on_exit(LPM0_bits); // Sale de LPM al final de interrupción
        }
        break;
    default: break;
    }
}
