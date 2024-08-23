#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/touch_pad.h>
#include <DHT.h>

// Definir los datos de conexión Wi-Fi
const char* ssid = "Yesenia";
const char* password = "samara2mg";
 
// Definir la dirección del broker MQTT
const char* mqtt_server = "test.mosquitto.org";

// Inicializar el cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Definir los pines del sensor de temperatura DHT11
#define TEMP_PIN 18      // Pin GPIO donde está conectado el sensor DHT11
#define DHTTYPE DHT11    // Definir el tipo de sensor DHT (DHT11, DHT22)

// Crear objetos para los sensores
DHT dht(TEMP_PIN, DHTTYPE);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MPU6050 mpu;

// Definir el tamaño de la pantalla OLED (128x64 por defecto)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Pin de reset (opcional)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definir umbrales de touch
#define THRESHOLD 150 // Aumenta el umbral para reducir la sensibilidad

// Pines touch asignados
#define TEMP_SENSOR_PIN TOUCH_PAD_NUM8
#define THERMO_SENSOR_PIN TOUCH_PAD_NUM9
#define STEP_SENSOR_PIN TOUCH_PAD_NUM6

// Pines de actuadores
#define BUZZER_PIN 4
#define VIBRATOR_PIN 5

// Pines del sensor de temperatura
#define TEMP_PIN 18
#define DHTTYPE DHT11   // Define el tipo de sensor DHT

// Variables para el contador de pasos
int stepCount = 0;
int threshold = 2500;  // Umbral más alto para evitar detecciones falsas 2800
int lastZ = 0;
int movingAverage = 0;
const int numSamples = 5;
int samples[numSamples];
int sampleIndex = 0;
bool stepDetected = false;
unsigned long lastStepTime = 0;
unsigned long stepDelay = 1000;  // Retardo mínimo entre pasos en milisegundos 1700

// Variables para debounce
unsigned long lastDebounceTimeStep = 0;
unsigned long lastDebounceTimeTemp = 0;
unsigned long lastDebounceTimeThermo = 0;
const unsigned long debounceDelay = 500; // Aumenta el tiempo de debounce en ms

// Estado del conteo de pasos y de los sensores
bool stepSensorActive = false;
bool tempSensorActive = false;
bool thermoSensorActive = false;

// Variables para almacenar los valores del sensor
float tempValue = 0;
float thermoValue = 0;

// Buffer para almacenar las temperaturas recientes
#define NUM_TEMP_VALUES 10
float tempValues[NUM_TEMP_VALUES];
float thermoValues[NUM_TEMP_VALUES];
int tempIndex = 0;

// Variables de tiempo para separar tareas
unsigned long lastTempReadTime = 0;
unsigned long lastStepReadTime = 0;
const unsigned long tempReadInterval = 2000; // Intervalo para leer temperatura (en ms)
const unsigned long stepReadInterval = 100;  // Intervalo para conteo de pasos (en ms)

void setup() {
  Serial.begin(115200);

   // Conectar a la red Wi-Fi
  setup_wifi();

    // Configurar el cliente MQTT
  client.setServer(mqtt_server, 1883);

  // Inicializar la pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error al inicializar la pantalla OLED"));
    for(;;);
  }
  display.clearDisplay();

  // Inicializar los sensores
  dht.begin();
  mlx.begin();
  mpu.initialize();

  // Inicializar el buffer de temperaturas y muestras para el filtro de promedio móvil
  for(int i = 0; i < NUM_TEMP_VALUES; i++) {
    tempValues[i] = 0.0;
    thermoValues[i] = 0.0;
  }
  for (int i = 0; i < numSamples; i++) {
    samples[i] = 0;
  }

  // Configurar los pines del buzzer y el vibrador como salida
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VIBRATOR_PIN, OUTPUT);

  // Asegurarse de que el buzzer y el vibrador estén apagados al inicio
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(VIBRATOR_PIN, LOW);

  // Configurar los pines touch
  touch_pad_init();
  touch_pad_config(TEMP_SENSOR_PIN, THRESHOLD);
  touch_pad_config(THERMO_SENSOR_PIN, THRESHOLD);
  touch_pad_config(STEP_SENSOR_PIN, THRESHOLD);

  // Mostrar mensaje inicial "Wearable Fit" en la pantalla
  mostrarMensajeInicial();

   // Conectar al broker MQTT
  reconnect();
}

void loop() {
  // Asegurarse de que el cliente MQTT esté conectado
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer el estado de los pines táctiles
  verificarToqueStep();
  verificarToqueTemp();
  verificarToqueThermo();

  // Separar la lógica de lectura de temperatura y conteo de pasos
  unsigned long currentMillis = millis();

  // Leer y mostrar temperatura a intervalos definidos
  if (currentMillis - lastTempReadTime >= tempReadInterval) {
    lastTempReadTime = currentMillis;
    if (tempSensorActive) {
      mostrarTemperatura();
    } else if (thermoSensorActive) {
      mostrarTermometro();
    }
  }

  // Leer y mostrar conteo de pasos a intervalos definidos
  if (currentMillis - lastStepReadTime >= stepReadInterval) {
    lastStepReadTime = currentMillis;
    if (stepSensorActive) {
      mostrarConteoPasos();
    }
  }

  // Publicar los valores de los sensores
  publicarValoresSensores();

  // Mostrar mensaje inicial si ningún sensor está activo
  if (!stepSensorActive && !tempSensorActive && !thermoSensorActive) {
    mostrarMensajeInicial();
  }

  delay(10); // Pequeño retardo para estabilizar
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
}

void reconnect() {
  // Solo intentamos reconectar si no estamos conectados
  if (client.connected()) {
    return;
  }

  // Loop hasta que estemos conectados
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    // Intentar conectarse
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
    } else {
      Serial.print("fallido, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando nuevamente en 5 segundos");
      delay(5000);
    }
  }
}

float lastSentTempValue = 0.0;
float lastSentThermoValue = 0.0;
int lastSentStepCount = 0;

void publicarValoresSensores() {
   // Verificar y publicar la temperatura desde DHT11
   if (tempSensorActive) {
       float tempDHT11 = dht.readTemperature();
       if (!isnan(tempDHT11) && tempDHT11 != lastSentTempValue) {
           lastSentTempValue = tempDHT11;
           char tempStr[8];
           dtostrf(tempDHT11, 6, 2, tempStr);
           client.publish("sensor/temperature/dht11", tempStr);
       }
   }

   // Verificar y publicar la temperatura desde MLX90614
   if (thermoSensorActive) {
       float tempMLX90614 = mlx.readObjectTempC();
       if (!isnan(tempMLX90614) && tempMLX90614 != lastSentThermoValue) {
           lastSentThermoValue = tempMLX90614;
           char tempStr[8];
           dtostrf(tempMLX90614, 6, 2, tempStr);
           client.publish("sensor/temperature/mlx90614", tempStr);
       }
   }

   // Verificar y publicar los pasos desde MPU6050
   if (stepSensorActive) {
       int steps = stepCount;
       if (steps != lastSentStepCount) {
           lastSentStepCount = steps;
           char stepsStr[8];
           itoa(steps, stepsStr, 10);
           client.publish("sensor/steps", stepsStr);
       }
   }
}

/*
void publicarValoresSensores() {
  // Leer temperatura desde DHT11
  float tempDHT11 = dht.readTemperature();
  if (!isnan(tempDHT11)) {
    char tempStr[8];
    dtostrf(tempDHT11, 6, 2, tempStr);
    client.publish("sensor/temperature/dht11", tempStr);
  }

  // Leer temperatura desde MLX90614
  float tempMLX90614 = mlx.readObjectTempC();
  if (!isnan(tempMLX90614)) {
    char tempStr[8];
    dtostrf(tempMLX90614, 6, 2, tempStr);
    client.publish("sensor/temperature/mlx90614", tempStr);
  }

  // Leer pasos desde MPU6050
  int steps = stepCount;  // Usar tu variable de conteo de pasos
  char stepsStr[8];
  itoa(steps, stepsStr, 10);
  client.publish("sensor/steps", stepsStr);
}
*/
// Función para verificar el estado del sensor de conteo de pasos
void verificarToqueStep() {
  uint16_t touchValue;
  touch_pad_read(STEP_SENSOR_PIN, &touchValue);

  if (touchValue < THRESHOLD) {
    if ((millis() - lastDebounceTimeStep) > debounceDelay) {
      stepSensorActive = !stepSensorActive; // Cambiar estado del sensor
      lastDebounceTimeStep = millis();

      if (stepSensorActive) {
        Serial.println("Conteo de pasos activado");
        tempSensorActive = false;  // Desactivar otros sensores
        thermoSensorActive = false;
      } else {
        Serial.println("Conteo de pasos desactivado");
      }
    }
  }
}

// Función para verificar el estado del sensor de temperatura DHT11
void verificarToqueTemp() {
  uint16_t touchValue;
  touch_pad_read(TEMP_SENSOR_PIN, &touchValue);

  if (touchValue < THRESHOLD) {
    if ((millis() - lastDebounceTimeTemp) > debounceDelay) {
      tempSensorActive = !tempSensorActive; // Cambiar estado del sensor
      lastDebounceTimeTemp = millis();

      // Desactivar el otro sensor si está activo
      if (tempSensorActive) {
        stepSensorActive = false;
        thermoSensorActive = false;
        Serial.println("Sensor de temperatura activado");
      } else {
        Serial.println("Sensor de temperatura desactivado");
      }
    }
  }
}

// Función para verificar el estado del termómetro
void verificarToqueThermo() {
  uint16_t touchValue;
  touch_pad_read(THERMO_SENSOR_PIN, &touchValue);

  if (touchValue < THRESHOLD) {
    if ((millis() - lastDebounceTimeThermo) > debounceDelay) {
      thermoSensorActive = !thermoSensorActive; // Cambiar estado del sensor
      lastDebounceTimeThermo = millis();

      // Desactivar el otro sensor si está activo
      if (thermoSensorActive) {
        stepSensorActive = false;
        tempSensorActive = false;
        Serial.println("Termómetro activado");
      } else {
        Serial.println("Termómetro desactivado");
      }
    }
  }
}

// Añadir el buffer para almacenar el histórico del conteo de pasos
#define NUM_STEP_VALUES 10
int stepValues[NUM_STEP_VALUES];
int stepIndex = 0;

// Función para mostrar el conteo de pasos en la pantalla OLED
void mostrarConteoPasos() {
  // Variables para almacenar los datos del giroscopio
  int16_t gx, gy, gz;

  // Leer los datos del giroscopio
  mpu.getRotation(&gx, &gy, &gz);

  // Actualizar el filtro de promedio móvil
  movingAverage -= samples[sampleIndex];
  samples[sampleIndex] = gz;
  movingAverage += samples[sampleIndex];
  sampleIndex = (sampleIndex + 1) % numSamples;

  int filteredZ = movingAverage / numSamples;

  // Obtener el tiempo actual
  unsigned long currentTime = millis();

  // Implementar la lógica para contar pasos basados en el eje Z del giroscopio
  if (abs(filteredZ - lastZ) > threshold && !stepDetected) {
    if (currentTime - lastStepTime > stepDelay) {
      stepCount++;
      stepDetected = true;
      lastStepTime = currentTime;
      Serial.print("Conteo de pasos: ");
      Serial.println(stepCount);

      // Activar el buzzer y el vibrador cada 5 pasos
      if (stepCount % 5 == 0) {
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(VIBRATOR_PIN, HIGH);
        delay(3000);  // Mantener el buzzer y vibrador activos por 3000 ms
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(VIBRATOR_PIN, LOW);
      }

      // Almacenar el conteo de pasos en el buffer
      stepValues[stepIndex] = stepCount;
      stepIndex = (stepIndex + 1) % NUM_STEP_VALUES;
    }
  }

  // Restablecer la detección de paso cuando el valor cae por debajo del umbral
  if (abs(filteredZ - lastZ) < threshold) {
    stepDetected = false;
  }

  // Guardar el último valor de Z para la siguiente iteración
  lastZ = filteredZ;

  // Mostrar el conteo de pasos y la gráfica en la pantalla OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Conteo de pasos: ");
  display.print(stepCount);

  // Dibujar la gráfica de pasos
  dibujarGraficaPasos(stepValues);

  display.display();
}

// Función para dibujar la gráfica de pasos en la pantalla OLED
void dibujarGraficaPasos(int values[]) {
  int maxBarHeight = 20;
  int barWidth = 10;
  int xOffset = 10;
  int yOffset = SCREEN_HEIGHT - maxBarHeight - 10;

  for (int i = 0; i < NUM_STEP_VALUES; i++) {
    int barHeight = map(values[i], 0, stepCount, 0, maxBarHeight); // Ajusta el rango según sea necesario
    int x = xOffset + i * (barWidth + 2);
    int y = yOffset + (maxBarHeight - barHeight);
    display.fillRect(x, y, barWidth, barHeight, SSD1306_WHITE);
  }
}

// Función para leer y mostrar los valores del sensor DHT11 en la pantalla OLED
void mostrarTemperatura() {
  tempValue = leerTemperatura();

  // Almacenar el valor en el buffer
  tempValues[tempIndex] = tempValue;
  tempIndex = (tempIndex + 1) % NUM_TEMP_VALUES;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temp DHT11: ");
  display.print(tempValue);
  display.print(" C");

  // Dibujar la gráfica
  dibujarGraficaTemperatura(tempValues);

  display.display();
}

// Función para leer y mostrar los valores del termómetro en la pantalla OLED
void mostrarTermometro() {
  thermoValue = leerTermometro();

  // Almacenar el valor en el buffer
  thermoValues[tempIndex] = thermoValue;
  tempIndex = (tempIndex + 1) % NUM_TEMP_VALUES;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Termometro: ");
  display.print(thermoValue);
  display.print(" C");

  // Dibujar la gráfica
  dibujarGraficaTemperatura(thermoValues);

  display.display();
}

// Función para dibujar una gráfica de barras de la temperatura
void dibujarGraficaTemperatura(float values[]) {
  int maxBarHeight = 20;
  int barWidth = 10;
  int xOffset = 10;
  int yOffset = SCREEN_HEIGHT - maxBarHeight - 10;

  for (int i = 0; i < NUM_TEMP_VALUES; i++) {
    int barHeight = map(values[i], 0, 40, 0, maxBarHeight); // Ajusta el rango de temperatura según sea necesario
    int x = xOffset + i * (barWidth + 2);
    int y = yOffset + (maxBarHeight - barHeight);
    display.fillRect(x, y, barWidth, barHeight, SSD1306_WHITE);
  }
}

// Función para mostrar mensaje "Wearable Fit" en la pantalla OLED
void mostrarMensajeInicial() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.print("Wearable");
  display.setCursor(40, 40);
  display.print("Fit");
  display.display();
}

// Función para leer el valor del sensor de temperatura DHT11
float leerTemperatura() {
  float t = dht.readTemperature();  // Lee la temperatura en grados Celsius
  
  if (isnan(t)) {
    Serial.println("Error al leer del sensor DHT!");
    return 0.0;
  }
  
  return t;
}

// Función para leer el valor del termómetro MLX90614
float leerTermometro() {
  float t = mlx.readObjectTempC();  // Lee la temperatura del objeto en grados Celsius
  
  if (isnan(t)) {
    Serial.println("Error al leer del termómetro!");
    return 0.0;
  }
  
  return t;
}