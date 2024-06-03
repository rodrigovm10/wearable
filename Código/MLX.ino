#include <Wire.h> // Incluye la biblioteca Wire para la comunicación I2C
#include <Adafruit_MLX90614.h> // Incluye la biblioteca Adafruit_MLX90614 para el sensor de temperatura

Adafruit_MLX90614 mlx = Adafruit_MLX90614(); // Crea una instancia del objeto Adafruit_MLX90614 llamado mlx

void setup() {
  Serial.begin(9600); // Inicia la comunicación serial a 9600 baudios
  Serial.println("Adafruit MLX90614 Test"); // Imprime un mensaje en el monitor serial
  mlx.begin(); // Inicia el sensor MLX90614
}

void loop() {
  Serial.print("Temperatura del objeto: "); // Imprime un mensaje en el monitor serial

  float temp = mlx.readObjectTempC(); // Lee la temperatura del objeto en grados Celsius y la guarda en la variable temp (cambiado a float)

  if (!isnan(temp)) { // Verifica si la lectura de la temperatura es válida (no es NaN)
    Serial.print(temp); // Imprime la temperatura leída
    Serial.println(" °C"); // Imprime la unidad de temperatura (grados Celsius)
  } else {
    Serial.println("Error al leer la temperatura."); // Imprime un mensaje de error si la lectura de la temperatura es inválida
  }

  delay(2000); // Espera 2 segundos antes de volver a leer la temperatura
}
