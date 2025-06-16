#include <Wire.h>

const int I2C_ADDRESS = 0x44;  // Dirección I2C del esclavo
const int LED_PIN = 13;        // LED integrado (o usa otro pin)
const int BUFFER_SIZE = 32;    // Tamaño del buffer de recepción

byte receivedData[BUFFER_SIZE];
byte dataIndex = 0;
unsigned long lastReceiveTime = 0;
const unsigned long LED_TIMEOUT = 500; // Tiempo para apagar LED después de recibir (ms)

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.begin(9600);
  Serial.println("Esclavo I2C con LED indicador");
  
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  
  Serial.print("Esclavo listo en direccion 0x");
  Serial.println(I2C_ADDRESS, HEX);
}

void loop() {
  // Apagar el LED si no hay recepción reciente
  if (millis() - lastReceiveTime > LED_TIMEOUT) {
    digitalWrite(LED_PIN, LOW);
  }
  
  delay(100); // Hacer más lento el loop principal
}

void receiveEvent(int howMany) {
  // Encender LED al recibir datos
  digitalWrite(LED_PIN, HIGH);
  lastReceiveTime = millis();
  
  Serial.print("Datos recibidos (");
  Serial.print(howMany);
  Serial.println(" bytes):");
  
  dataIndex = 0;
  
  // Leer datos lentamente para apreciar el LED
  while (Wire.available()) {
    if (dataIndex < BUFFER_SIZE) {
      receivedData[dataIndex] = Wire.read();
      
      // Mostrar dato recibido
      Serial.print("0x");
      if (receivedData[dataIndex] < 0x10) Serial.print("0");
      Serial.print(receivedData[dataIndex], HEX);
      Serial.print(" ");
      
      dataIndex++;
      
      delay(100); // Retardo para hacer visible el LED por cada byte
    } else {
      Wire.read(); // Descartar si buffer lleno
    }
  }
  Serial.println(); // Nueva línea después de mostrar todos los bytes
  
  processReceivedData();
}

void processReceivedData() {
  // Ejemplo: mostrar como cadena
  Serial.print("Mensaje: ");
  for (int i = 0; i < dataIndex; i++) {
    Serial.print((char)receivedData[i]);
  }
  Serial.println("\n---");
  
  // Aquí puedes añadir tu lógica para procesar los datos
}
