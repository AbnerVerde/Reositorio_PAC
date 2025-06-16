//  Esclavo RX
//
//                                /|\  /|\
//                MSP430F5529     10k  10k     MSP430F5529
//                   esclavo       |    |         maestro
//             -----------------   |    |   -----------------
//           -|XIN  P3.0/UCB0SDA|<-|----+->|P3.0/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |             XOUT|-
//            |     P3.1/UCB0SCL|<-+------>|P3.1/UCB0SCL     |
//            |                 |          |                 |

#include <msp430.h> //libreria para el microcontrolador


volatile unsigned char RXData; //variable de datos del reseptor
//inicia main
int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;   // se duerme al perro guardian
  P3SEL |= 0x03; // se habilita el pin 3.0 y 3.1  como entrada
  UCB0CTL1 |= UCSWRST; // Habilita el USCI en el estado del reset
  UCB0CTL0 = UCMODE_3 + UCSYNC;// pone un modo sincrono y habilita la comunicacion i2c
  UCB0I2COA = 0x48; // Pone la direccion del controlador
  UCB0CTL1 &= ~UCSWRST; //deshabilita el reset del USCI
  UCB0IE |= UCRXIE;//interrupcion habilitada

  while (1)
  {
    __bis_SR_register(LPM0_bits + GIE);     // Interrupcion
    __no_operation();                       // Operacion nula como delay
  }                                         //
}

// Funcion de servicio de interrupcin USCI_B0
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__) //
#pragma vector = USCI_B0_VECTOR //
__interrupt void USCI_B0_ISR(void) //funcion de interrupcion
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB0IV,12))//ayuda a mejorar la interrupcion para limpiar banderas especificas
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6: break;                           // Vector  6: STTIFG
  case  8: break;                           // Vector  8: STPIFG
  case 10:                                  // Vector 10: RXIFG
    RXData = UCB0RXBUF;    // envia los datos recibidos por un buffer

    __bic_SR_register_on_exit(LPM0_bits);   //interrupcion
    break;
  case 12: break;                           // Vector 12: TXIFG
  default: break;
  }
}
