#include <msp430.h>

const char buffer[10];
int f2=0;
int  f=0;
int c =0;
int t=0;
void imprimir(const char mensaje[],int longitud); //mp=menu principal
int i = 0;
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P4DIR |= BIT7; // configuramos P4.7 como salida
          //  P4OUT &= ~BIT7; // apagamos el led verde

    P4SEL |= BIT4 + BIT5 ; //ACTIVAMOS LA FUNCION RX Y TX EN P4.5 Y P4.4
    UCA1CTL1 |= UCSSEL__SMCLK; //CONECTA LA FUENTE DE RELOJ A
    UCA1BR0=109; //SONFIGURACIN DE VELOCIDAD PARTE 1
    UCA1MCTL |= UCBRS_2;//CONFIGURACION DE VELOCIDAD PARTE 2
    //8 BITS E DATOS SIN PARIEDAD, UN BIT DE PARO -----> 10 BAUDIOS
    UCA1CTL1 &= ~UCSWRST;//INICIA COMUNICACION
    UCA1IE |= UCRXIE ;//HABILITO INTERRUCION

    P4DIR |= BIT7; // configuramos P4.7 como salida
              //  P4OUT &= ~BIT7; // apagamos el led verde

        P3SEL |= BIT4 + BIT3 ; //ACTIVAMOS LA FUNCION RX Y TX EN P4.5 Y P4.4

        UCA0CTL1 |= UCSSEL__SMCLK; //CONECTA LA FUENTE DE RELOJ A
        UCA0BR0=109; //SONFIGURACIN DE VELOCIDAD PARTE 1
        UCA0MCTL |= UCBRS_2;//CONFIGURACION DE VELOCIDAD PARTE 2
        //8 BITS E DATOS SIN PARIEDAD, UN BIT DE PARO -----> 10 BAUDIOS
        UCA0CTL1 &= ~UCSWRST;//INICIA COMUNICACION
        UCA0IE |= UCRXIE ;//HABILITO INTERRUCION
        __bis_SR_register(GIE);
        TA0R=0;
        TA2CTL |= TASSEL__SMCLK + MC__UP;
               TA2CCR0=1997;
               TA2CCTL0 |= CCIE ;
    __bis_SR_register(GIE);


    while (1){

        if (f==1){
            imprimir(buffer, 10);
        f=0;
        }
    }

}

void imprimir(const char mensaje[],int longitud)
{
    for ( i=1; i<longitud; i++){
        UCA1TXBUF = mensaje[i];
        __delay_cycles(10000);
    }

}



#pragma vector=USCI_A0_VECTOR
__interrupt void FCCG (void)
{
if ((UCA0RXBUF=='\n') && (f2==0)){
    f2=1;
    c=0;
    t=t;
}
    else if (f2==1){
    if ((c < sizeof(buffer)) && t==1){
         UCA0RXBUF==buffer;
         c++;
         t=0;
     }
 }
    else if ((c == sizeof(buffer)) || (t==10) ) {
         f=1;
         f=0;
         TA2CTL &= ~TASSEL__SMCLK + MC__UP;

                       TA2CCTL0 =~ ! CCIE ;
         }

}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void FCCccG (void)
{
    t=t+1;
    }

#pragma vector=USCI_A1_VECTOR
__interrupt void FCCGGG (void)
{
    UCA1RXBUF=UCA1TXBUF ;
    }

