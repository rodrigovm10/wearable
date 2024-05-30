#include <PulseSensorPlayground.h>     // Incluye la biblioteca PulseSensorPlayground

// Variables
const int PulseWire = 34;      // Pin analógico en la ESP32 donde se conecta el sensor de pulso
const int LED = 2;             // El LED incorporado en la ESP32, normalmente en el pin 2
int Threshold = 550;           // Umbral para contar una señal como un latido

PulseSensorPlayground pulseSensor;  // Crea una instancia del objeto PulseSensorPlayground

void setup() {
  Serial.begin(115200);          // Para el monitor serie

  // Configura el objeto PulseSensor asignando nuestras variables
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED);       // Hace que el LED de la ESP32 parpadee con cada latido
  pulseSensor.setThreshold(Threshold);   

  // Verifica que el objeto "pulseSensor" fue creado y comenzó a ver una señal
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object!");  // Esto se imprime una vez al encender o resetear la ESP32
  }
}

void loop() {
  if (pulseSensor.sawStartOfBeat()) {            // Verifica constantemente si "ocurrió un latido"
    int myBPM = pulseSensor.getBeatsPerMinute();  // Llama a la función en nuestro objeto pulseSensor que devuelve BPM como un "int"
    Serial.println("♥  A HeartBeat Happened! "); // Si la prueba es "verdadera", imprime un mensaje indicando que ocurrió un latido
    Serial.print("BPM: ");                       // Imprime la frase "BPM: " 
    Serial.println(myBPM);                       // Imprime el valor dentro de myBPM
  }

  delay(20);  // Se considera una buena práctica en un sketch simple
}
