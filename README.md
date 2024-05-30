# Wearable Fit

## Visión
Wearable fitness que va más allá de simplemente registrar datos de actividad física. Este dispositivo no solo mide tu rendimiento, sino que te motiva y te ayuda a alcanzar tus objetivos de manera inteligente y personalizada.

## Software empleado
- **React** - Licencia: MIT
- **Node.js** - Licencia: MIT
- **Express** - Licencia: MIT

## Hardware
- **Acelerómetro**
  - **Nombre:** Acelerómetro MPU6050
  - **Imagen:** ![Acelerómetro MPU6050](ruta/a/imagen.png)
  - **Descripción:** Mide la aceleración en tres ejes (X, Y, Z) para rastrear movimientos y detectar actividades físicas.
  - **Cantidad:** 1

- **Sensor de Frecuencia Cardíaca**
  - **Nombre:** Sensor de Ritmo Cardíaco MAX30100
  - **Imagen:** ![Sensor MAX30100](ruta/a/imagen.png)
  - **Descripción:** Mide el ritmo cardíaco en pulsaciones por minuto (bpm).
  - **Cantidad:** 1

- **Sensor de Temperatura Corporal**
  - **Nombre:** Sensor de Temperatura Corporal MLX90614
  - **Imagen:** ![Sensor MLX90614](ruta/a/imagen.png)
  - **Descripción:** Mide la temperatura del cuerpo del usuario en grados Celsius (°C).
  - **Cantidad:** 1

## Actuadores
- **Vibrador**
  - **Nombre:** Motor Vibrador
  - **Imagen:** ![Motor Vibrador](ruta/a/imagen.png)
  - **Descripción:** Proporciona retroalimentación al usuario sobre un tramo recorrido.
  - **Cantidad:** 1

- **Pantalla**
  - **Nombre:** Pantalla OLED 128x64
  - **Imagen:** ![Pantalla OLED](ruta/a/imagen.png)
  - **Descripción:** Muestra información relevante del dispositivo, como las métricas de salud y actividad física.
  - **Cantidad:** 1

- **LED**
  - **Nombre:** LED RGB
  - **Imagen:** ![LED RGB](ruta/a/imagen.png)
  - **Descripción:** Señales visuales, como indicaciones de estado o alertas.
  - **Cantidad:** 1

- **Buzzer**
  - **Nombre:** Buzzer
  - **Imagen:** ![Buzzer](data:image/jpeg;base64,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)
  - **Descripción:** Emite sonidos para alertar o notificar al usuario.
  - **Cantidad:** 1

## Historias de usuario épicas

## Fotografía del prototipo

## Arquirectura 

## Captura Kanban

### Sprint 1

## Circuito diseñado

## Resultados
