#include <msp430.h> //incluye libreria del MSP

int i;//indice
int f=0;//bandera 1
int f2=0;//bandera 2
int of=0;//desbordamiento en el buffer
unsigned int c=0;//contador
int t=0;//contador para el desbordamiento
char buffer[30 ];//vector buffer con tamaño de 30

void imprimir(char mensaje[], unsigned int longitud);//declaracion de funcion para imprimir

//inicia main

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P1DIR |= BIT0; //se declara el pin uno como salida

    P4SEL |= BIT5 | BIT4; //Activar la funcion de comunicacion en Tx y Rx en P4.5 y P4.4

    P3SEL |= BIT3 | BIT4;//Activar la funcion de comunicacion en Tx y Rx en P3.3 y P3.4

    UCA1CTL1 |= UCSWRST; //Habilitamos el reset en USCIAAAA
    UCA1CTL1 |= UCSSEL__SMCLK;//Conectamos la fuente SMCLK
    UCA1BR0 |= 109; //Establecemos la velocidad de 9600
    UCA1MCTL |= UCBRS_2; //Establecemos la velocidad de 9600
    UCA1CTL1 &= ~UCSWRST;
    UCA1IE |= UCRXIE; // Habilitamos interrupcion de buffer

    UCA0CTL1 |= UCSWRST; //Habilitamos el reset en USCIAAAA
    UCA0CTL1 |= UCSSEL__SMCLK;//Conectamos la fuente SMCLK a la USCI
    UCA0BR0 |= 109; //Establecemos la velocidad de 9600 bds
    UCA0MCTL |= UCBRS_2; //Establecemos la velocidad de 9600 bds
    UCA0CTL1 &= ~UCSWRST;
    UCA0IE |= UCRXIE; // Habilitamos interrupcion de buffer

    TA0CTL |= TASSEL__SMCLK + MC_0; //seleccionamos el reloj y su modo en este caso continuo
    TA0CCR0 = 65535;//ponemos el valor en el que llega la interrupcion
    TA0CCTL0 |= CCIE;//habilitamos interrupcion de reloj

    __bis_SR_register(GIE);//funcion para poder entrar a las interrupciones

    while(1){//inicia un ciclo while para revisar todo el tiempo si f=1
    if(f == 1){//cuando la bandera 1 entra en la funcion if
       imprimir(buffer,sizeof(buffer));//imprime el buffer y con ayuda de la funcion sizeof obtenemos el numero de caracteres
       f = 0;//mandamos flag 1 a false
       c=0;//reseteamos el contador
    }
    }
}
//funcion de imprimir
    void imprimir(char mensaje[],unsigned int longitud){
        for (i=1; i<longitud; i++){//ciclo for para moverse a lo largo del vector mensaje
         UCA1TXBUF = mensaje[i];//Tx buffer igualado a mensaje
         __delay_cycles(10000);//delay para no saturar el buffer
        }
    }
//interrupciones

    //interrupcion de UART en pin 3.3 y 3.4
    #pragma vector=USCI_A0_VECTOR
    __interrupt void FCCG(void){

        if(UCA0RXBUF == '\n' && f2 == 0){ //si existe un salto de linea y nuestra bandera 2 es falsa  estra a la funcion
            //imprimiendo el buffer
            f2=1;//Ponemos la bandera 2 a true
            c=0;//Mandamos el contador a 0

        }

        if(f2 == 1){//cuando la bandera 2 es true entra en esta funcion
            if(c <= 8 && of == 0){//revisa que no exista un desbordamiento en el buffer

                buffer[c]= UCA0RXBUF; // Metemos los carateres recibidos en el vector buffer
                c++;//iniciamos el contador

                if(UCA0RXBUF == '\n'){// detecta un salto de lina
                    f = 1;//pone en uno la bandera 1 mandado a imprimir el buffer
                    c = 0;//reinicia el contador
                    TA0R = 0; //se reinicia el timer
                }
            }
        }

        if(c >= 9 || of == 1){//si existe un desbordamiento entra en la funcion
        f = 1;//manda imprimir el buffer
        f2 = 0;//pone en falso la bandera 2
        c=0;//reinicia el contador
        }
    }
//rutina de interrupcion para el timer
    #pragma vector=TIMER0_A0_VECTOR
    __interrupt void FCCGG(void){
    t++;//añade un 1 en el desbordamiento del contador

        if(t >= 100){ //cuando se cumplan 100 ciclos de interrupcion
            of=1;// se pone la bandera 3 en 1
            t=0;//se reinicia el contador para el desbordamiento
        }
    }

