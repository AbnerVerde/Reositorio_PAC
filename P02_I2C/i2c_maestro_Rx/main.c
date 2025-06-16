//******************************************************************************
//  Maestro Receptor.
//  Si el maestro recibe el dato almacenado en RXCompare
//                                /|\  /|\
//               MSP430F5529      10k  10k     MSP430F5529
//                   esclavo       |    |        maestro
//             -----------------   |    |   -----------------
//           -|XIN  P3.0/UCB0SDA|<-|----+->|P3.0/UCB0SDA  XIN|-
//            |                 |  |       |                 | 32kHz
//           -|XOUT             |  |       |             XOUT|-
//            |     P3.1/UCB0SCL|<-+------>|P3.1/UCB0SCL     |
//            |                 |          |             P1.0|--> LED
//******************************************************************************

#include <msp430.h>

unsigned char RXData;
unsigned char RXCompare;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // dormir el perro guardian
  P1OUT &= ~0x01;                           // delcara como entrada el pin 1.1
  P1DIR |= 0x01;                            // habilita el pin 1.1
  P3SEL |= 0x03;                            // declara como salida el pin 3.3
  UCB0CTL1 |= UCSWRST;                      // Habilita el USCI en el estado del reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     //Habilita la comunicacion 12c en modo maestro
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // selecciona el reloj aclk, habilita el USCI en el reset
  UCB0BR0 = 20;                             // Baudios a los que va a trabajar
  UCB0BR1 = 0;                              // baudios a los que va a trabajar
  UCB0I2CSA = 0x48;                         // direccion de la tarjeta
  UCB0CTL1 &= ~UCSWRST;                     //  dehabilita el reset
  UCB0IE |= UCRXIE;                         // habilita interrupcion
  RXCompare = 0x0;                          // le da valores iniciales a la variable RXcompare

  while (1)
  {
    while (UCB0CTL1 & UCTXSTP);             // hace una lectura de la direccion
    UCB0CTL1 |= UCTXSTT;                    // El esclavo manda la direccion y la condicion al maestro
    while(UCB0CTL1 & UCTXSTT);              //
    UCB0CTL1 |= UCTXSTP;                    // El esclavo manda la direccion y la condicion al maestro

    __bis_SR_register(LPM0_bits + GIE);     // habilita interrupciones
    __no_operation();                       //

    if (RXData != RXCompare)                //  Cuando las variables sean distintas entra en el ciclo
    {
      P1OUT |= 0x01;                        //pono en 1 logico el pin 1.1
      while(1);
    }
  }
}

// Funcion de servicio de interrupción USCI_B0 (Hoja de datos)
// Sirve para sacar al microcontrolador de bajo consumo
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
  case  6: break;                           // Vector  6: STTIFG
  case  8: break;                           // Vector  8: STPIFG
  case 10:                                  // Vector 10: RXIFG
    RXData = UCB0RXBUF;                     // Get RX data
    __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
    break;
  case 12: break;                           // Vector 12: TXIFG
  default: break;
  }
}

