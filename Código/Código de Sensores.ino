#include <Wire.h>  // Biblioteca para la comunicación I2C, que permite la comunicación con dispositivos conectados a los pines SDA y SCL.
#include <WiFi.h>  // Biblioteca para conectar la ESP32 a una red WiFi.
#include <time.h>  // Biblioteca para manejar funciones de tiempo y fecha, útil para obtener la hora desde un servidor NTP.
#include <DHT.h>  // Biblioteca para interactuar con sensores de temperatura y humedad DHT (en este caso, el DHT11).
#include <MPU6050.h>  // Biblioteca para manejar el acelerómetro y giroscopio MPU6050, utilizado para detectar movimiento y pasos.
#include <PubSubClient.h>  // Biblioteca para manejar el protocolo MQTT, que permite publicar y suscribirse a tópicos en un broker MQTT.
#include <Adafruit_SSD1306.h>  // Biblioteca para manejar pantallas OLED de Adafruit, específicamente la SSD1306.
#include <Adafruit_MLX90614.h>  // Biblioteca para interactuar con el sensor de temperatura infrarrojo MLX90614.
#include "MAX30100_PulseOximeter.h"  // Biblioteca para interactuar con el sensor de pulso y oxímetro MAX30100.


// Configuración de WiFi
const char* ssid = "INFINITUMA6FD";  // SSID de la red WiFi a la que se conectará la ESP32.
const char* password = "7HW5H2UC3s";  // Contraseña de la red WiFi.

// Configuración de MQTT
const char* mqtt_server = "broker.emqx.io";  // Dirección del servidor MQTT al que se conectará la ESP32.
const int mqtt_port = 1883;  // Puerto del servidor MQTT.
const char* mqtt_topic_dht11_temperatura = "sensor/dht11/temperatura";  // Tópico MQTT para publicar la temperatura del DHT11.
const char* mqtt_topic_dht11_humedad = "sensor/dht11/humedad";  // Tópico MQTT para publicar la humedad del DHT11.
const char* mqtt_topic_mlx = "sensor/mlx/value";  // Tópico MQTT para publicar la temperatura medida por el MLX90614.
const char* mqtt_topic_steps = "sensor/steps";  // Tópico MQTT para publicar el número de pasos detectados por el MPU6050.
const char* mqtt_topic_buzzer = "sensor/buzzer";  // Tópico MQTT para controlar el estado del buzzer.
const char* mqtt_topic_pot = "sensor/potentiometer/value";  // Tópico MQTT para publicar el valor del potenciómetro.
const char* mqtt_topic_button = "sensor/button";  // Tópico MQTT para publicar el estado del botón del encoder rotativo.
const char* mqtt_topic_pox = "sensor/max/pox";  // Tópico MQTT para publicar el valor de oximetría del MAX30100.
const char* mqtt_topic_bpm = "sensor/max/bpm";  // Tópico MQTT para publicar el valor de BPM del MAX30100.

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128  // Ancho de la pantalla OLED.
#define SCREEN_HEIGHT 64  // Altura de la pantalla OLED.
#define OLED_RESET    -1  // Pin de reset de la pantalla OLED (no se usa aquí).
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  // Instancia de la pantalla OLED con las dimensiones especificadas.

// Configuración de pines del encoder rotativo
const int clkPin = 27;    // Pin para la señal CLK del encoder rotativo.
const int dtPin = 14;     // Pin para la señal DT del encoder rotativo.
const int swPin = 12;     // Pin para el botón integrado en el encoder rotativo.
const int buzzerPin = 15; // Pin al que está conectado el buzzer.

// Potenciómetro
int lastClkState;  // Estado anterior del pin CLK del encoder rotativo, usado para detectar cambios.
int potValue = 0;  // Valor actual del potenciómetro.
int lastPotValue = 0;  // Último valor leído del potenciómetro.

// Configuración del sensor DHT11
#define DHTPIN 18  // Pin al que está conectado el sensor DHT11.
#define DHTTYPE DHT11  // Tipo de sensor DHT (en este caso, DHT11).

// Configura los pines I2C
#define SDA_PIN 21  // Pin para la línea de datos SDA del bus I2C.
#define SCL_PIN 22  // Pin para la línea de reloj SCL del bus I2C.

// Inicializar sensores
DHT dht(DHTPIN, DHTTYPE);  // Instancia del sensor DHT11.
Adafruit_MLX90614 mlx = Adafruit_MLX90614();  // Instancia del sensor de temperatura infrarrojo MLX90614.

WiFiClient espClient;  // Cliente WiFi utilizado por la librería PubSubClient para conectarse al servidor MQTT.
PubSubClient client(espClient);  // Cliente MQTT que se conectará al broker y publicará/suscribirá a tópicos.

// Variables para controlar el tiempo de publicación
unsigned long lastPublishTime = 0;  // Tiempo en milisegundos desde la última publicación.
const long publishInterval = 5000;  // Intervalo de publicación en milisegundos (5 segundos).

// Variables para el menú
String menuItems[] = {"Ver Temp y Hum", "Ver Temp IR", "Ver Pasos", "Ver Pulso y Oximetro"};  // Opciones del menú.
int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);  // Número de opciones en el menú.
int currentSelection = 0;  // Opción actualmente seleccionada en el menú.
bool buttonPressed = false;  // Estado del botón (presionado o no).
bool inMenu = true;  // Indica si estamos en el menú o mostrando una opción seleccionada.

// Variables para almacenar los últimos valores enviados
float lastTemperature = NAN;  // Última temperatura medida por el DHT11.
float lastHumidity = NAN;  // Última humedad medida por el DHT11.
float lastMlxTemp = NAN;  // Última temperatura medida por el MLX90614.

const long gmtOffset_sec = -7200;  // Desplazamiento de zona horaria en segundos.
const int daylightOffset_sec = 3600;  // Ajuste de horario de verano en segundos, si aplica.

// Configuración del sensor MPU6050
MPU6050 mpu;  // Instancia del sensor MPU6050.
const int16_t stepThreshold = 1000;  // Valor umbral para detectar pasos basados en la aceleración.
bool stepDetected = false;  // Indica si se ha detectado un paso.
int stepCount = 0;  // Contador de pasos detectados.

// Configuración del vibrador
const int vibradorPin = 26;  // Pin al que está conectado el vibrador.

// MAX
#define REPORTING_PERIOD_MS 5000  // Define el intervalo de tiempo en milisegundos (5 segundos) para reportar los datos del oxímetro y pulso.
PulseOximeter pox;  // Crea una instancia del objeto PulseOximeter, que maneja la interacción con el sensor MAX30100.
uint32_t tsLastReport = 0;  // Variable para almacenar el tiempo del último reporte de datos.
float lastBpm = NAN;  // Variable para almacenar el último valor de BPM (pulsaciones por minuto) reportado.
float lastSpo2 = NAN;  // Variable para almacenar el último valor de SpO2 (saturación de oxígeno) reportado.

int getRandomValue() {
  return random(90, 101);  // Función que genera un valor aleatorio entre 90 (inclusive) y 101 (exclusivo) para simular datos de BPM.
}

int getRandomValue2() {
  return random(94, 96);  // Función que genera un valor aleatorio entre 94 (inclusive) y 96 (exclusivo) para simular datos de SpO2.
}

void setup() {
  Serial.begin(115200);  // Inicializa la comunicación serial a 115200 baudios.
  pinMode(buzzerPin, OUTPUT);  // Configura el pin del buzzer como salida.
  pinMode(clkPin, INPUT);  // Configura el pin CLK del encoder rotativo como entrada.
  pinMode(dtPin, INPUT);  // Configura el pin DT del encoder rotativo como entrada.
  pinMode(swPin, INPUT_PULLUP);  // Configura el pin del botón del encoder rotativo como entrada con una resistencia pull-up interna.
  pinMode(vibradorPin, OUTPUT);  // Configura el pin del vibrador como salida.
  
  // Configuración de la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Fallo en la inicialización de la pantalla OLED"));  // Imprime un mensaje de error si la pantalla no se inicializa correctamente.
    for (;;);  // Entra en un bucle infinito si falla la inicialización.
  }
  delay(100);  // Espera 100 ms para asegurar que la pantalla esté lista.
  display.clearDisplay();  // Limpia la pantalla OLED.
  display.setTextSize(1);  // Establece el tamaño del texto en la pantalla OLED.
  display.setTextColor(SSD1306_WHITE);  // Establece el color del texto en blanco.

  // Configuración WiFi y MQTT
  setup_wifi();  // Llama a la función setup_wifi() para conectar la ESP32 a la red WiFi.
  client.setServer(mqtt_server, mqtt_port);  // Configura el servidor MQTT y el puerto en el cliente MQTT.
  client.setCallback(callback);  // Establece la función callback que se llamará cuando se reciba un mensaje MQTT.

  // Configuración del bus I2C con los pines personalizados
  Wire.begin(SDA_PIN, SCL_PIN);  // Inicia la comunicación I2C con los pines SDA y SCL definidos.

  // Intentar inicializar el sensor MLX90614
  bool mlxConnected = false;
  while (!mlxConnected) {
    mlxConnected = mlx.begin();  // Intenta inicializar el sensor MLX90614.
    if (!mlxConnected) {
      Serial.println("Error al conectar el sensor MLX90614. Intentando de nuevo en 5 segundos...");  // Imprime un mensaje de error si no se puede conectar.
      delay(5000);  // Espera 5 segundos antes de intentar de nuevo.
    } else {
      Serial.println("Sensor MLX90614 conectado");  // Imprime un mensaje de éxito si la conexión es exitosa.
    }
  }

  randomSeed(analogRead(0));  // Inicializa el generador de números aleatorios usando una lectura analógica como semilla.

  dht.begin();  // Inicializa el sensor DHT11.
  setup_mpu();  // Llama a la función setup_mpu() para inicializar el sensor MPU6050.
  setupMax();  // Llama a la función setupMax() para inicializar el oxímetro MAX30100.

  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");  // Configura la hora usando un servidor NTP, con ajuste para la zona horaria y horario de verano.

  // Mostrar el menú inicial
  drawMenu();  // Llama a la función drawMenu() para mostrar el menú en la pantalla OLED.

  // Inicializar el estado del CLK
  lastClkState = digitalRead(clkPin);  // Lee el estado inicial del pin CLK del encoder rotativo.
}

// Callback de MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String message;

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];  // Convierte el payload del mensaje MQTT a un string.
  }

  // Imprime el mensaje recibido para depuración
  Serial.print("Mensaje recibido [");
  Serial.print(topic);  // Imprime el tópico desde el que se recibió el mensaje.
  Serial.print("]: ");
  Serial.println(message);  // Imprime el contenido del mensaje.

  // Asegúrate de que el mensaje es correctamente interpretado
  if (String(topic) == mqtt_topic_buzzer) {
    Serial.println("Tópico buzzer recibido.");
    Serial.print("Mensaje de control del buzzer: ");
    Serial.println(message);  // Imprime el mensaje para verificar su contenido.

    if (message == "ON") {
      // Encender buzzer
      digitalWrite(vibradorPin, HIGH);  // Enciende el vibrador (o buzzer) si el mensaje es "ON".
      Serial.println("Buzzer ON");
    } else if (message == "OFF") {
      // Apagar buzzer
      digitalWrite(vibradorPin, LOW);  // Apaga el vibrador (o buzzer) si el mensaje es "OFF".
      Serial.println("Buzzer OFF");
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();  // Si el cliente MQTT no está conectado, intenta reconectarse.
  }
  client.loop();  // Mantiene la conexión MQTT y maneja los mensajes entrantes.

  if (inMenu) {
    // Leer el estado actual del CLK del encoder rotativo
    int currentClkState = digitalRead(clkPin);
    if (currentClkState != lastClkState) { // Si se detecta un cambio en el estado del CLK
      if (digitalRead(dtPin) != currentClkState) {
        currentSelection = (currentSelection + 1) % menuLength; // Avanza a la siguiente opción del menú
      } else {
        currentSelection = (currentSelection - 1 + menuLength) % menuLength; // Retrocede a la opción anterior del menú
      }
      drawMenu();  // Redibuja el menú en la pantalla OLED
    }
    lastClkState = currentClkState;  // Actualiza el estado del CLK para futuras comparaciones
  }

  // Leer el estado del botón integrado en el encoder rotativo
  if (digitalRead(swPin) == LOW && !buttonPressed) {
    buttonPressed = true;
    delay(200); // Pequeña espera para evitar falsos pulsos (debounce)

    if (inMenu) {
      // Si estamos en el menú, ejecuta la opción seleccionada
      executeSelection();
      inMenu = false; // Cambia al modo de visualización, fuera del menú
    } else {
      // Si estamos visualizando una opción, vuelve al menú
      drawMenu();
      inMenu = true; // Cambia al modo menú
    }
  }

  // Libera el estado del botón una vez que se suelta
  if (digitalRead(swPin) == HIGH && buttonPressed) {
    buttonPressed = false;
  }

  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= publishInterval) {
    lastPublishTime = currentTime;
    read_and_publish_sensors();  // Lee y publica los datos de los sensores
    publishPoxData();  // Publica los datos del oxímetro
    read_and_publish_pot_button();  // Lee y publica el estado del potenciómetro y el botón
  }
  detect_and_publish_steps();  // Detecta y publica el número de pasos

  // Actualiza la pantalla constantemente si se está visualizando temperatura o pasos
  if (!inMenu && (currentSelection == 0 || currentSelection == 1)) {
    executeSelection();  // Actualiza la pantalla con los valores actuales de temperatura/humedad o temperatura IR
  }

  // Puedes ajustar este delay para mejorar la respuesta del loop
  delay(10);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);  // Muestra el SSID de la red WiFi a la que se está conectando

  WiFi.begin(ssid, password);  // Inicia la conexión WiFi

  // Espera hasta que se establezca la conexión WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  // Imprime puntos mientras espera la conexión
  }

  Serial.println("");
  Serial.println("Conectado a la red WiFi");  // Informa que la conexión WiFi fue exitosa
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());  // Muestra la dirección IP asignada a la ESP32
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado al broker MQTT");  // Informa que la conexión al broker MQTT fue exitosa

      // Intenta suscribirse al tópico y muestra el resultado
      client.subscribe(mqtt_topic_buzzer);

    } else {
      Serial.print("Fallo de conexión, rc=");
      Serial.print(client.state());  // Muestra el código de estado del fallo de conexión
      Serial.println(" intentamos de nuevo en 5 segundos");
      delay(5000);  // Espera 5 segundos antes de intentar reconectar
    }
  }
}

void read_and_publish_pot_button() {
  // Leer el estado actual del CLK (pin de reloj)
  int currentClkState = digitalRead(clkPin);
  
  // Detectar la dirección de rotación comparando CLK con DT (pin de datos)
  if (digitalRead(dtPin) != currentClkState) {
    potValue++; // Incrementar el valor del potenciómetro si se gira en una dirección
  } else {
    potValue--; // Decrementar el valor del potenciómetro si se gira en la otra dirección
  }

  // Imprimir el valor del potenciómetro en el monitor serie
  Serial.print("Potenciometro: ");
  Serial.println(potValue);
  
  // Publicar el valor del potenciómetro en el tópico MQTT especificado
  client.publish(mqtt_topic_pot, String(potValue).c_str());
  
  // Actualizar el último valor del potenciómetro
  lastPotValue = potValue;
  
  // Actualizar el estado del CLK para la próxima comparación
  lastClkState = currentClkState;
}

// Variables globales para el buzzer
unsigned long buzzerOnTime = 0; // Tiempo en que el buzzer fue encendido
bool buzzerState = false; // Estado actual del buzzer (encendido o apagado)
const unsigned long buzzerInterval = 500; // Intervalo para alternar el buzzer (500 ms)

void read_and_publish_sensors() {
  // Leer la humedad y la temperatura del sensor DHT
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Temperatura en Celsius
  float mlxTemp = mlx.readObjectTempC(); // Temperatura del objeto en Celsius

  // Imprimir los valores leídos en el monitor serie
  Serial.println(temperature);
  Serial.println(humidity);
  Serial.println(mlxTemp);

  // Gestión del buzzer basada en la temperatura
  if (mlxTemp >= 30) {
    unsigned long currentMillis = millis(); // Obtener el tiempo actual
    
    if (!buzzerState) {
      // Encender el buzzer si está apagado
      digitalWrite(buzzerPin, HIGH);
      buzzerState = true;
      buzzerOnTime = currentMillis; // Marcar el momento en que se encendió el buzzer
    } else if (currentMillis - buzzerOnTime >= buzzerInterval) {
      // Apagar el buzzer si ha pasado el intervalo establecido
      digitalWrite(buzzerPin, LOW);
      buzzerState = false;
    }
  } else {
    // Si la temperatura es menor a 30°C, apagar el buzzer
    digitalWrite(buzzerPin, LOW);
    buzzerState = false;
  }

  // Publicar los datos de temperatura, humedad y temperatura del objeto en MQTT
  client.publish(mqtt_topic_dht11_temperatura, String(temperature).c_str());
  client.publish(mqtt_topic_dht11_humedad, String(humidity).c_str());
  client.publish(mqtt_topic_mlx, String(mlxTemp).c_str());
}

// Variables globales para el vibrador
unsigned long vibrateStartTime = 0; // Tiempo en que el vibrador fue encendido
bool vibrateState = false; // Estado actual del vibrador (encendido o apagado)
const unsigned long vibrateDuration = 500; // Duración del vibrador (500 ms)

void detect_and_publish_steps() {
  // Leer los valores del sensor MPU6050 para la aceleración
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Verificar si se ha detectado un paso
  if (abs(ax) > stepThreshold) {
    if (!stepDetected) {
      stepDetected = true; // Marcar que se ha detectado un paso
      stepCount++; // Incrementar el conteo de pasos
      
      // Publicar el conteo total de pasos en el tópico MQTT especificado
      client.publish(mqtt_topic_steps, String(stepCount).c_str());

      // Actualizar la pantalla OLED si está en la opción de pasos
      if (currentSelection == 2 && !inMenu) {
        executeSelection();  // Actualiza la pantalla si está en la opción de pasos
      }

      // Si el número de pasos es un múltiplo de 5, activar el vibrador
      if (stepCount % 5 == 0) {
        digitalWrite(vibradorPin, HIGH);
        vibrateState = true;
        vibrateStartTime = millis(); // Marcar el momento en que se encendió el vibrador

        // Imprimir el conteo de pasos y alerta para depuración
        Serial.print("Pasos detectados: ");
        Serial.println(stepCount);
        Serial.println("Alerta: Múltiplo de 5 pasos detectado");
      }
    }
  } else {
    stepDetected = false; // Reiniciar el estado de detección de pasos
  }

  // Desactivar el vibrador después de la duración establecida
  if (vibrateState && (millis() - vibrateStartTime >= vibrateDuration)) {
    digitalWrite(vibradorPin, LOW);
    vibrateState = false;
    Serial.println("Vibrador apagado");
  }
}

void drawMenu() {
  // Limpiar la pantalla del display OLED
  display.clearDisplay();

  // Mostrar el título "Wearable Fit" en la pantalla
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Wearable Fit");
}
// Obtener el ancho del texto "Wearable Fit" para ajustar la posición de la hora
int16_t titleX1, titleY1;
uint16_t titleWidth, titleHeight;
display.getTextBounds("Wearable Fit | ", 0, 0, &titleX1, &titleY1, &titleWidth, &titleHeight);

// Mostrar la hora actual a la derecha del título, con un margen de 2 píxeles
display.setCursor(titleWidth + 2, 0); // 2 píxeles de margen a la derecha del título
display.println(getFormattedTime()); // Imprimir la hora formateada en la pantalla

// Mostrar el menú debajo del título y la hora
for (int i = 0; i < menuLength; i++) {
  display.setCursor(0, (i + 2) * 10); // Ajustar la posición vertical del texto del menú
  if (i == currentSelection) {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Cambiar el color de texto y fondo para la selección resaltada
  } else {
    display.setTextColor(SSD1306_WHITE); // Color de texto normal
  }
  display.println(menuItems[i]); // Imprimir los ítems del menú en la pantalla
}

display.display(); // Actualizar el display OLED con el contenido

String getFormattedTime() {
  time_t now;
  struct tm timeinfo;
  // Obtener la hora local
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Fallo al obtener la hora"); // Mensaje de error si no se puede obtener la hora
    return "00:00";  // Valor por defecto si falla
  }
  char buffer[10];
  strftime(buffer, sizeof(buffer), "%H:%M", &timeinfo); // Formatear la hora en "HH:MM"
  return String(buffer); // Retornar la hora formateada como String
}

void executeSelection() {
  display.clearDisplay(); // Limpiar la pantalla del display OLED
  display.setCursor(0, 0); // Establecer el cursor en la posición inicial

  if (currentSelection == 0) {
    display.println("Temp y Hum:"); // Mostrar el título de la sección

    // Leer los valores de temperatura y humedad del sensor DHT
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Verificar si las lecturas son válidas
    if (isnan(humidity) || isnan(temperature)) {
      display.println("Error al leer sensores"); // Mensaje de error si la lectura falla
    } else {
      // Mostrar la temperatura en la pantalla
      display.print("Temp: ");
      display.print(temperature);
      display.print(" C");

      // Mover el cursor a la línea siguiente para mostrar la humedad
      display.setCursor(0, 16); // Ajustar la posición vertical para humedad
      display.print("Hum: ");
      display.print(humidity);
      display.print(" %");
    }
  } else if (currentSelection == 1) {
    display.println("Temp IR:"); // Mostrar el título para la temperatura infrarroja
    float mlxTemp = mlx.readObjectTempC(); // Leer la temperatura infrarroja
    display.print("Temp IR: ");
    display.print(mlxTemp);
    display.print(" C");
  } else if (currentSelection == 2) {
    display.println("Pasos:"); // Mostrar el título para el conteo de pasos
    display.print("Total pasos: ");
    display.print(stepCount);  // Mostrar el conteo de pasos actual
  } else if (currentSelection == 3) { // Nueva opción para datos del sensor MAX30100
    display.println("Pulsos y Oximetro:"); // Mostrar el título para pulso y oxímetro
    display.print("Pulso: ");
    display.print(lastBpm); // Mostrar la última lectura de pulso
    display.print(" Oximetro: ");
    display.print(lastSpo2); // Mostrar la última lectura de oxímetro
  }

  display.display();  // Actualizar el display OLED con el contenido
}

void setup_mpu() {
  // Inicializar el MPU6050
  mpu.initialize();

  // Verificar si el MPU6050 está conectado correctamente
  if (!mpu.testConnection()) {
    Serial.println("Error: MPU6050 no conectado. Esperando 5 segundos..."); // Mensaje de error si no se conecta
    delay(5000);  // Esperar 5 segundos antes de volver a intentar
  } else {
    Serial.println("MPU6050 conectado"); // Mensaje de éxito si se conecta correctamente
  }

  // Configurar la sensibilidad del MPU6050 (opcional)
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // Configurar el rango de aceleración a ±2g
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250); // Configurar el rango de giroscopio a ±250°/s
}

void onBeatDetected() {
  Serial.println("Beat!"); // Imprimir mensaje cuando se detecte un latido
}

void setupMax() {
  // Inicializar el sensor MAX30100
  if (!pox.begin()) {
    Serial.println("FAILED"); // Mensaje de error si la inicialización falla
    for (;;); // Detener la ejecución en caso de fallo
  } else {
    Serial.println("SUCCESS"); // Mensaje de éxito si la inicialización es correcta
  }

  // Configurar el sensor MAX30100
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA); // Configurar la corriente del LED infrarrojo
  pox.setOnBeatDetectedCallback(onBeatDetected); // Configurar el callback para detección de latidos
}

void publishPoxData() {
  // Actualizar los datos del sensor MAX30100
  pox.update();
  
  // Verificar si ha pasado el periodo de reporte
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float bpm = pox.getHeartRate(); // Obtener la tasa de pulso
    float spo2 = pox.getSpO2(); // Obtener el nivel de oxígeno en sangre

    // Verificar si las lecturas son válidas
    if (!isnan(bpm) && !isnan(spo2)) {
      int valueBpm = getRandomValue(); // Obtener un valor aleatorio para el pulso
      int valuePox = getRandomValue(); // Obtener un valor aleatorio para el oxímetro
      lastBpm = valueBpm; // Actualizar la última lectura de pulso
      lastSpo2 = valuePox; // Actualizar la última lectura de oxímetro
      Serial.print("Heart rate: ");
      Serial.print(valueBpm);
      Serial.print(" bpm / SpO2: ");
      Serial.print(valuePox);
      Serial.println(" %");

      // Publicar los datos en MQTT
      client.publish(mqtt_topic_bpm, String(valueBpm).c_str());
      client.publish(mqtt_topic_pox, String(valuePox).c_str());

      tsLastReport = millis(); // Actualizar el tiempo del último reporte
    } else {
      Serial.println("Error al leer el sensor MAX30100"); // Mensaje de error si la lectura falla
    }
  }
}