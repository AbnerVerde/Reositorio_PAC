void setup() {
    Serial.begin(9600);//inicia la comunicacion serial a 9600 baudios
}

void loop() {
    Serial.println("Datos de prueba desde el transmisor");  // Envío de datos con salto de línea
    delay(500);  // delay entre envio y envio 
}
