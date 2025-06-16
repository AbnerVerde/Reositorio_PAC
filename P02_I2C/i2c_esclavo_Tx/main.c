//******************************************************************************
//  Esclavo transmisor
//                                /|\  /|\
//               MSP430F5529      10k  10k     MSP430F5529
//                   esclavo       |    |        maestro
//             -----------------   |    |   -----------------
//           -|XIN  P3.0/UCB0SDA|<-|----+->|P3.0/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |             XOUT|-
//            |     P3.1/UCB0SCL|<-+------>|P3.1/UCB0SCL     |
//            |                 |          |                 |
//******************************************************************************

#include <msp430.h>

unsigned char TXData;//declaracion de variable del trasmisor
unsigned char i=0;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;// se deurme el perro guardian

  P3SEL |= 0x03;                            // se declara 3.3 como salida
  UCB0CTL1 |= UCSWRST;                      // Habilita el USCI en el estado del reset
  UCB0CTL0 = UCMODE_3 + UCSYNC;             // Habilita la comunicacion i2c
  UCB0I2COA = 0x48;                         // escribe la direccion de la tarjeta
  UCB0CTL1 &= ~UCSWRST;                     //  desactiva el reset de el USCI
  UCB0IE |= UCTXIE + UCSTTIE + UCSTPIE;     // interrupcion del trasmisor habilitada, condicion inicial y de paro de interrupcion activada
  TXData = 0;                               // valor cero de la variable

  __bis_SR_register(LPM0_bits + GIE);       // habilitacion de la interrupcion
  __no_operation();                         //
}

// Funcion de servicio de interrupcion USCI_B0 (Hoja de datos)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6:                                  // Vector  6: STTIFG
     UCB0IFG &= ~UCSTTIFG;                  //
     break;
  case  8:                                  // Vector  8: STPIFG
    TXData++;                               //
    UCB0IFG &= ~UCSTPIFG;                   //
    break;
  case 10: break;                           // Vector 10: RXIFG
  case 12:                                  // Vector 12: TXIFG
    UCB0TXBUF = TXData;                     // Dato en el buffer de transmision i2c
    break;
  default: break;
  }
}
