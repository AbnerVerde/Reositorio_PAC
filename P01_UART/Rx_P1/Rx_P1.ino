#define BUFFER_SIZE 64  // Tamaño del buffer de recepción
#define TIMEOUT_MS 1000 // Tiempo máximo de espera en milisegundos en este caso 1 sefundo

char buffer[BUFFER_SIZE];  // se usa un vector para almacenar los caracteres en la variable buffer
int i = 0; //indice para movernos por el vector buffer 
unsigned long lastReceiveTime = 0; // variable para medir el tiempo 

void setup() {
  //inicializamos la comunicacion serial a 9600 baudios 
    Serial.begin(9600);
}

void loop() {
  //verificacion de la disponiviladad de nuestros puertos Tx, Rx
    if (Serial.available()) {
        char receivedChar = Serial.read();
        buffer[i++] = receivedChar;
        lastReceiveTime = millis();  // Reinicia el temporizador

        // Condición 1: Fin de línea detectado
        if (receivedChar == '\n') { // Detecta el salto de lina en la cadena del buffer 
            buffer[i] = '\0';  // Termina la cadena
            Serial.print("Mensaje recibido: "); //imprime en la consola "mensaje recibido"  
            Serial.println(buffer);//imprime en la consola lo que recibio el buffer
            i = 0;  // Reiniciar índice
            return;
        }

        // Condición 3: Desbordamiento de buffer
        if (index >= BUFFER_SIZE - 1) { //verifica cuando el indice llega hasta el tamañano del buffer
            buffer[i] = '\0';  // Termina la cadena
            Serial.println(buffer);//imprime la cadena cuando detecta el desvordamiento 
            i = 0;//reinicia el indice 
            return;
        }
    }

    // Condición 2: Tiempo 
    if (i > 0 && (millis() - lastReceiveTime > TIMEOUT_MS)) {//verifica cuando se pase un tiempo determinado 
        buffer[i] = '\0';  // Termina la cadena
        Serial.println("Mensaje recibido por tiempo:"); //imprime un indicador para saber que el mensaje se imprimio por tiempo 
        Serial.println(buffer);//imprime el buffer 
        i = 0;//reinicia el indice 
    }
}
