#include <Wire.h>
#include <WiFi.h>
#include <time.h>
#include <DHT.h>
#include <MPU6050.h>
#include <PubSubClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include "MAX30100_PulseOximeter.h"

// Configuración de WiFi
const char* ssid = "Redmi note 9";
const char* password = "wearable_fit";

// Configuración de MQTT
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic_dht11_temperatura = "sensor/dht11/temperatura";
const char* mqtt_topic_dht11_humedad = "sensor/dht11/humedad";
const char* mqtt_topic_mlx = "sensor/mlx/value";
const char* mqtt_topic_steps = "sensor/steps";
const char* mqtt_topic_buzzer = "sensor/buzzer";
const char* mqtt_topic_pot = "sensor/potentiometer/value";
const char* mqtt_topic_button = "sensor/button";
const char* mqtt_topic_pox = "sensor/max/pox";
const char* mqtt_topic_bpm = "sensor/max/bpm";

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración de pines del encoder rotativo
const int clkPin = 27;    // Pin del CLK
const int dtPin = 14;     // Pin del DT
const int swPin = 12;     // Pin del botón integrado en el encoder
const int buzzerPin = 15; // Pin del buzzer

// Potenciometro
int lastClkState;
int potValue = 0;
int lastPotValue = 0;

// Configuración del sensor DHT11
#define DHTPIN 18
#define DHTTYPE DHT11

// Configura los pines I2C
#define SDA_PIN 21
#define SCL_PIN 22

// Inicializar sensores
DHT dht(DHTPIN, DHTTYPE);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0; 
const long publishInterval = 5000; 

// Variables para el menú
String menuItems[] = {"Ver Temp y Hum", "Ver Temp IR", "Ver Pasos", "Ver Pulso y Oximetro"};
int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);
int currentSelection = 0;
bool buttonPressed = false;
bool inMenu = true; // Indica si estamos en el menú o mostrando una opción

// Variables para almacenar los últimos valores enviados
float lastTemperature = NAN;
float lastHumidity = NAN;
float lastMlxTemp = NAN;

const long gmtOffset_sec = -7200; // Ajuste según tu zona horaria
const int daylightOffset_sec = 3600; // Ajuste para horario de verano, si aplica

// MPU
MPU6050 mpu;
const int16_t stepThreshold = 1000;  // Valor umbral para la detección de pasos
bool stepDetected = false;
int stepCount = 0;  // Variable para almacenar el número de pasos detectados

// Vibrador
const int vibradorPin = 26;


// MAX
#define REPORTING_PERIOD_MS 5000
PulseOximeter pox;
uint32_t tsLastReport = 0;
float lastBpm = NAN;
float lastSpo2 = NAN;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP); // Botón con pull-up interno
  pinMode(vibradorPin, OUTPUT);
  
  // Configuración de la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Fallo en la inicialización de la pantalla OLED"));
    for (;;);
  }
  delay(100); 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Configuración WiFi y MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Configuración del bus I2C con los pines personalizados
  Wire.begin(SDA_PIN, SCL_PIN);

    // Intentar inicializar el sensor MLX90614
  bool mlxConnected = false;
  while (!mlxConnected) {
    mlxConnected = mlx.begin();
    if (!mlxConnected) {
      Serial.println("Error al conectar el sensor MLX90614. Intentando de nuevo en 5 segundos...");
      delay(5000); // Esperar 5 segundos antes de intentar de nuevo
    } else {
      Serial.println("Sensor MLX90614 conectado");
    }
  }

  dht.begin();
  setup_mpu();
  setupMax();

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");
  
  // Mostrar el menú inicial
  drawMenu();

  // Inicializar el estado del CLK
  lastClkState = digitalRead(clkPin);
}

// Callback de MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String message;

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Imprime el mensaje recibido para depuración
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Asegúrate de que el mensaje es correctamente interpretado
  if (String(topic) == mqtt_topic_buzzer) {
    Serial.println("Tópico buzzer recibido.");
    Serial.print("Mensaje de control del buzzer: ");
    Serial.println(message);  // Imprime el mensaje para verificar su contenido

    if (message == "ON") {
      // Encender buzzer
      digitalWrite(vibradorPin, HIGH);
      Serial.println("Buzzer ON");
    } else if (message == "OFF") {
      // Apagar buzzer
      digitalWrite(vibradorPin, LOW);
      Serial.println("Buzzer OFF");
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (inMenu) {
    // Leer el estado actual del CLK
    int currentClkState = digitalRead(clkPin);
    if (currentClkState != lastClkState) { // Cambio detectado
      if (digitalRead(dtPin) != currentClkState) {
        currentSelection = (currentSelection + 1) % menuLength; // Siguiente
      } else {
        currentSelection = (currentSelection - 1 + menuLength) % menuLength; // Anterior
      }
      drawMenu();
    }
    lastClkState = currentClkState;
  }

  // Leer el botón integrado del encoder
  if (digitalRead(swPin) == LOW && !buttonPressed) {
    buttonPressed = true;
    delay(200); // Debounce del botón

    if (inMenu) {
      // Si estamos en el menú, ejecutar la selección
      executeSelection();
      inMenu = false; // Cambiar al estado de visualización
    } else {
      // Si estamos mostrando una opción, volver al menú
      drawMenu();
      inMenu = true; // Cambiar al estado de menú
    }
  }

  // Liberar el botón
  if (digitalRead(swPin) == HIGH && buttonPressed) {
    buttonPressed = false;
  }

  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= publishInterval) {
    lastPublishTime = currentTime;
    read_and_publish_sensors();
    publishPoxData();
    read_and_publish_pot_button();
  }
  detect_and_publish_steps();

  // Actualiza la pantalla constantemente si se está visualizando temperatura o pasos
  if (!inMenu && (currentSelection == 0 || currentSelection == 1)) {
    executeSelection();  // Actualiza la pantalla con los valores actuales de temperatura/humedad o temperatura IR
  }

  // Puedes ajustar este delay para mejorar la respuesta
  delay(10);
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
  Serial.println("Conectado a la red WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado al broker MQTT");

      // Intenta suscribirte al tópico y muestra el resultado
      client.subscribe(mqtt_topic_buzzer);

    } else {
      Serial.print("Fallo de conexión, rc=");
      Serial.print(client.state());
      Serial.println(" intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void read_and_publish_pot_button() {
  // Leer el estado actual del CLK
  int currentClkState = digitalRead(clkPin);
  
    // Detectar la dirección de rotación comparando CLK con DT
    if (digitalRead(dtPin) != currentClkState) {
      potValue++; // Incrementar el valor del potenciómetro si se gira en una dirección
    } else {
      potValue--; // Decrementar el valor del potenciómetro si se gira en la otra dirección
    }

    Serial.print("Potenciometro: ");
    Serial.println(potValue);
    client.publish(mqtt_topic_pot, String(potValue).c_str());
    lastPotValue = potValue;
  // Actualizar el estado del CLK
    lastClkState = currentClkState;

}

// Variables globales para el buzzer
unsigned long buzzerOnTime = 0;
bool buzzerState = false;
const unsigned long buzzerInterval = 500; // Intervalo para alternar el buzzer (500 ms)

void read_and_publish_sensors() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius
  float mlxTemp = mlx.readObjectTempC(); // Celsius

  Serial.println(temperature);
  Serial.println(humidity);
  Serial.println(mlxTemp);

  // Gestión del buzzer
  if (mlxTemp >= 30) {
    unsigned long currentMillis = millis();
    
    if (!buzzerState) {
      // Encender el buzzer
      digitalWrite(buzzerPin, HIGH);
      buzzerState = true;
      buzzerOnTime = currentMillis; // Marca el momento en que se encendió el buzzer
    } else if (currentMillis - buzzerOnTime >= buzzerInterval) {
      // Apagar el buzzer
      digitalWrite(buzzerPin, LOW);
      buzzerState = false;
    }
  } else {
    // Si la temperatura es menor a 30°C, apaga el buzzer
    digitalWrite(buzzerPin, LOW);
    buzzerState = false;
  }

  // Publicar datos en MQTT
  client.publish(mqtt_topic_dht11_temperatura, String(temperature).c_str());
  client.publish(mqtt_topic_dht11_humedad, String(humidity).c_str());
  client.publish(mqtt_topic_mlx, String(mlxTemp).c_str());
}

// Variables globales para el vibrador
unsigned long vibrateStartTime = 0;
bool vibrateState = false;
const unsigned long vibrateDuration = 500; // Duración del vibrador (500 ms)

void detect_and_publish_steps() {
  // Lee los valores del sensor MPU6050
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Verifica si se ha detectado un paso
  if (abs(ax) > stepThreshold) {
    if (!stepDetected) {
      stepDetected = true;
      stepCount++;
      
      // Publica el conteo total de pasos como una cadena
      client.publish(mqtt_topic_steps, String(stepCount).c_str());

      // Actualiza la pantalla OLED para mostrar los pasos
      if (currentSelection == 2 && !inMenu) {
        executeSelection();  // Actualiza la pantalla si está en la opción de pasos
      }

      // Si el número de pasos es un múltiplo de 5, activa el vibrador
      if (stepCount % 5 == 0) {
        digitalWrite(vibradorPin, HIGH);
        vibrateState = true;
        vibrateStartTime = millis(); // Marca el momento en que se encendió el vibrador

        // Imprime el conteo de pasos y alerta para depuración
        Serial.print("Pasos detectados: ");
        Serial.println(stepCount);
        Serial.println("Alerta: Múltiplo de 5 pasos detectado");
      }
    }
  } else {
    stepDetected = false;
  }

  // Desactiva el vibrador después de la duración establecida
  if (vibrateState && (millis() - vibrateStartTime >= vibrateDuration)) {
    digitalWrite(vibradorPin, LOW);
    vibrateState = false;
    Serial.println("Vibrador apagado");
  }
}


void drawMenu() {
  display.clearDisplay();

  // Mostrar el título "Wearable Fit"
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Wearable Fit");

  // Obtener el ancho del texto "Wearable Fit"
  int16_t titleX1, titleY1;
  uint16_t titleWidth, titleHeight;
  display.getTextBounds("Wearable Fit | ", 0, 0, &titleX1, &titleY1, &titleWidth, &titleHeight);
  
  // Mostrar la hora actual a la derecha del título
  display.setCursor(titleWidth + 2, 0); // 2 píxeles de margen a la derecha
  display.println(getFormattedTime());

  // Mostrar el menú debajo del título y la hora
  for (int i = 0; i < menuLength; i++) {
    display.setCursor(0, (i + 2) * 10); // Ajusta la posición vertical
    if (i == currentSelection) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Selección resaltada
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    display.println(menuItems[i]);
  }

  display.display();
}

String getFormattedTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Fallo al obtener la hora");
    return "00:00";  // Valor por defecto si falla
  }
  char buffer[10];
  strftime(buffer, sizeof(buffer), "%H:%M", &timeinfo);
  return String(buffer);
}

void executeSelection() {
  display.clearDisplay();
  display.setCursor(0, 0);

  if (currentSelection == 0) {
    display.println("Temp y Hum:");

    // Leer los valores de temperatura y humedad
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Verificar si las lecturas son válidas
    if (isnan(humidity) || isnan(temperature)) {
      display.println("Error al leer sensores");
    } else {
      // Mostrar temperatura
      display.print("Temp: ");
      display.print(temperature);
      display.print(" C");

      // Mover el cursor a la línea siguiente para humedad
      display.setCursor(0, 16); // Ajusta la posición vertical para que esté debajo de la temperatura
      display.print("Hum: ");
      display.print(humidity);
      display.print(" %");
    }
  } else if (currentSelection == 1) {
    display.println("Temp IR:");
    float mlxTemp = mlx.readObjectTempC();
    display.print("Temp IR: ");
    display.print(mlxTemp);
    display.print(" C");
  } else if (currentSelection == 2) {
    display.println("Pasos:");
    display.print("Total pasos: ");
    display.print(stepCount);  // Asegúrate de que se muestra el conteo de pasos actual
  } else if (currentSelection == 3) { // Nueva opción para MAX30100
    display.println("Pulsos y Oximetro:");
    display.print("Pulso: ");
    display.print(lastBpm); // Usa la última lectura almacenada
    display.print("Oximetro: ");
    display.print(lastSpo2); // Usa la última lectura almacenada
  }

  display.display();  // Asegúrate de que la pantalla se actualiza
}

void setup_mpu() {
  // Inicializar el MPU6050
  mpu.initialize();

  // Verificar si el MPU6050 está conectado correctamente
  if (!mpu.testConnection()) {
    Serial.println("Error: MPU6050 no conectado. Esperando 5 segundos...");
    delay(5000);  // Esperar 5 segundos antes de volver a intentar
  } else {
    Serial.println("MPU6050 conectado");
  }

  // Configurar la sensibilidad del MPU6050 (opcional)
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // ±2g
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250); // ±250°/s
}

void onBeatDetected() {
  Serial.println("Beat!");
}

void setupMax(){
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void publishPoxData() {
  pox.update();
  
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float bpm = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    if (!isnan(bpm) && !isnan(spo2)) {
      lastBpm = bpm;
      lastSpo2 = spo2;
      Serial.print("Heart rate: ");
      Serial.print(valueBpm);
      Serial.print(" bpm / SpO2: ");
      Serial.print(valuePox);
      Serial.println(" %");

      client.publish(mqtt_topic_bpm, String(bpm).c_str());
      client.publish(mqtt_topic_pox, String(spo2).c_str());

      tsLastReport = millis();
    } else {
      Serial.println("Error al leer el sensor MAX30100");
    }
  }
}
