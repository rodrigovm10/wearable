# Fit Wearable

## Visión
Wearable fitness que va más allá de simplemente registrar datos de actividad física. Este dispositivo no solo mide tu rendimiento, sino que te motiva y te ayuda a alcanzar tus objetivos de manera inteligente y personalizada.

## Software empleado
- **React Native** - Licencia: MIT
- **Arduino IDE** - Licencia: MIT

## Hardware
### Sensores
- **Acelerómetro**
  - **Nombre:** Acelerómetro
  - **Imagen:**  <br><img src="https://sonrobots.com/wp-content/uploads/2021/01/ADXL335-340x299.jpg" alt="Acelerómetro" width="300"/>
  - **Descripción:** Mide la aceleración en tres ejes (X, Y, Z) para rastrear movimientos y detectar actividades físicas.
  - **Cantidad:** 1

- **Sensor de Frecuencia Cardíaca**
  - **Nombre:** Sensor de Ritmo Cardíaco
  - **Imagen:**  <br><img src="https://www.geekfactory.mx/wp-content/uploads/sen0038-sensor-de-pulso-cardiaco1-100x100.jpg" alt="Sensor de Ritmo Cardíaco" width="300"/>
  - **Descripción:** Mide el ritmo cardíaco en pulsaciones por minuto (bpm).
  - **Cantidad:** 1

- **Sensor de Temperatura Corporal**
  - **Nombre:** Sensor de Temperatura Corporal
  - **Imagen:**  <br><img src="https://uelectronics.com/wp-content/uploads/2018/08/AR0608-MLX90614_v1.png" alt="Sensor de Temperatura Corporal" width="300"/>
  - **Descripción:** Mide la temperatura del cuerpo del usuario en grados Celsius (°C).
  - **Cantidad:** 1

### Actuadores
- **Vibrador**
  - **Nombre:** Motor Vibrador
  - **Imagen:**  <br><img src="https://ultra-lab.net/wp-content/uploads/2017/12/VibrationMotor-01-L.jpg" alt="Motor Vibrador" width="300"/>
  - **Descripción:** Proporciona retroalimentación al usuario sobre un tramo recorrido.
  - **Cantidad:** 1

- **Pantalla**
  - **Nombre:** Pantalla OLED 128x64
  - **Imagen:** <br><img src="https://pigra.com.mx/1106-medium_default/pantalla-oled-13-pulgadas-128x64-i2c.jpg" alt="Pantalla OLED" width="300"/>
  - **Descripción:** Muestra información relevante del dispositivo, como las métricas de salud y actividad física.
  - **Cantidad:** 1

- **LED**
  - **Nombre:** LED RGB
  - **Imagen:** <br><img src="https://www.steren.com.mx/media/catalog/product/cache/bb0cad18a6adb5d17b0efd58f4201a2f/image/21726305e/led-de-5-mm-rgb.jpg" alt="LED RGB" width="300"/>
  - **Descripción:** Señales visuales, como indicaciones de estado o alertas.
  - **Cantidad:** 1

- **Buzzer**
  - **Nombre:** Buzzer
  - **Imagen:**<br><img src="https://www.electronicaembajadores.com/Datos/fotos/articulos/grandes/zb/zbpz/zbpzs02.jpg" alt="Buzzer" width="300"/>
  - **Descripción:** Emite sonidos para alertar o notificar al usuario.
  - **Cantidad:** 1

## Historias de usuario épicas
1.	Monitorear la cantidad de pasos caminados por el usuario y mostrarlas, además de poder configurar un objetivo de pasos.
2.	Monitorear el ritmo cardiaco del usuario durante diferentes actividades físicas y alertar si el ritmo cardiaco es muy alto. 
3.	Monitorear la temperatura corporal del usuario y avisar si la temperatura corporal es muy alta.

## Tabla de Requisitos del Proyecto 'Fit Wearable'

| ID  | Como un   | Quiero poder                                         | Para                                           | Con la prioridad de | En el sprint número | Definición de terminado                                                                                                                                                   |
|-----|-----------|------------------------------------------------------|------------------------------------------------|---------------------|---------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1   | Usuario   | Mandar alerta por ritmo cardiaco alto    | Monitorear mi actividad física diaria          | Debería tenerlo (Media) | 1                   | **Prueba:** Tener un ritmo cardiaco alto. **Criterio de aceptación:** El buzzer se manda una alerta.                                                                         |
| 2   | Usuario   | Ver mi ritmo cardiaco                                | Monitorear mi actividad física diaria          | Debe tenerlo (Alta)  | 1                   | **Prueba:** Someter al usuario a caminar, correr y descansar. **Criterio de aceptación:** Mostrar el ritmo cardiaco ante las diferentes situaciones.                        |
| 3   | Usuario   | Ver mi temperatura corporal                          | Monitorear mi actividad física diaria          | Debe tenerlo (Alta)  | 1                   | **Prueba:** Poner el sensor en una distancia para detectar la temperatura. **Criterio de aceptación:** Mostrar la temperatura del usuario actual.                           |
| 4   | Usuario   | Monitorear el ritmo cardíaco durante diferentes actividades físicas | Obtener información sobre mi ritmo cardíaco en tiempo real | Se Debe incluir (Alta) | 2                   | **Prueba:** Someter al usuario a caminar, correr y descansar. **Criterio de aceptación:** Mostrar el ritmo cardiaco en la pantalla del wearable en todo momento.             |
| 5   | Usuario   | Monitorear los pasos caminado                | Monitorear mi actividad física diaria | Se Debería Incluir (Media) | 2                   | **Prueba:** El usuario tiene pasos caminados. **Criterio de aceptación:** Se visualizan los pasos caminados en tiempo real.                         |
| 6   | Usuario   | Ver un registro histórico de mi ritmo cardiaco       | Seguir la evolución del ritmo cardíaco a lo largo del tiempo | Se puede incluir (baja) | 2                   | **Prueba:** El usuario tiene diferentes actividades durante un periodo de una semana. **Criterio de aceptación:** El usuario visualiza su ritmo cardiaco.                  |
| 7   | Usuario   | Ver estadísticas y reportes de actividad física      | Revisar mi progreso y resultados               | Se debería incluir (media) | 3                   | **Prueba:** Tener la aplicación instalada y conectada al wearable. **Criterio de aceptación:** La aplicación muestra gráficos y resumen de actividad física, ritmo cardiaco y calorías quemadas. |


## Fotografía del prototipo

## Arquitectura
![Arquitectura](https://github.com/rodrigovm10/wearable/assets/104101668/c8256386-6f8e-49dc-95f6-954479f0e449)

## Captura Kanban

### Sprint 1
![Sprint 1 - Tablero](https://github.com/rodrigovm10/wearable/assets/104101668/c748fbd1-87da-4874-a1c7-0137aec2078c)

### Sprint 2
![Sprint 2 - Tablero](https://github.com/rodrigovm10/wearable/assets/104101668/f8292947-bfd3-4e6c-bf9d-68c855fe9b24)

### Sprint 3

## Circuito diseñado
### Requisito 1 y 2
![Requisito 1 y 2](https://github.com/rodrigovm10/wearable/assets/104101668/be32d3fd-2640-408b-a074-542a72699945)
### Requisito 3
![Requisito 3](https://github.com/rodrigovm10/wearable/assets/104101668/dc430b0a-ed7c-4b42-a138-0944aacae749)

## Resultados
