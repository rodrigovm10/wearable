 # Fit Wearable

## Visión
<p align="justify">
  Nuestra visión es redefinir la experiencia del ejercicio y las actividades físicas, proporcionando un dispositivo 
   wearable que no solo registre el rendimiento, sino que también motive y guíe a los usuarios de manera 
   personalizada e inteligente. Este dispositivo está diseñado para superar las barreras comunes que enfrentan las 
   personas activas, tales como el monitoreo del rendimiento, la falta de motivación y la dificultad para 
   personalizar sus entrenamientos según sus necesidades individuales a través de una app.
</p>

## Objetivo general
<p align="justify">
El objetivo general del proyecto Wearable Fit es desarrollar un dispositivo wearable y una aplicación para  que permitan a los usuarios monitorear y mejorar su actividad física y salud en tiempo real y accesible que incluya el registro de pasos, medición de temperatura corporal, ambiental y frecuencia cardiaca.
</p>

## Objetivos Específicos

<ul>
  <li><p align="justify"><b>Monitorear la Actividad Física:</b> Desarrollar un sistema que registre y muestre en tiempo real la cantidad de pasos caminados por el usuario.</p></li>
  <li><p align="justify"><b>Medición de la Temperatura Corporal:</b> Implementar un sensor que mida la temperatura corporal del usuario y proporcione alertas en caso de que la temperatura supere un rango saludable. </p></li>
  <li><p align="justify"><b>Proveer Retroalimentación en Tiempo Real:</b> Integrar un motor vibrador y un buzzer para ofrecer retroalimentación, alertando al usuario sobre metas alcanzadas y condiciones críticas de salud.</p></li>
  <li><p align="justify"><b>Visualización de Datos:</b> Desarrollar una aplicación que permita al usuario acceder de manera visual a sus datos de salud y rendimiento físico. </p></li>
</ul>



## Software empleado
- **Flutter** - Licencia: MIT
- **Arduino IDE** - Licencia: MIT
- **PostgreSQL** -  Licencia: PostgreSQL License (similar a la licencia MIT) 
- **MQTT** - Licencia: EPL
- **Node-Red** - Licencia: MIT
## Hardware

## Hardware

### Sensores

| **Nombre**                         | **Imagen**                                                                                                                                                                 | **Descripción**                                                                                                             | **Cantidad** |
|------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------|--------------|
| **Acelerómetro**                   | <img src="https://sonrobots.com/wp-content/uploads/2021/01/ADXL335-340x299.jpg" alt="Acelerómetro" width="150"/>                                                           | Mide la aceleración en tres ejes (X, Y, Z) para rastrear movimientos y detectar actividades físicas.                        | 1            |
| **Sensor Temperatura y Humedad**   | <img src="https://imgs.search.brave.com/DH26FfhdEHWjqGFNm7ZReO7jmJSvnfbbgOuHcq2TCGM/rs:fit:860:0:0:0/g:ce/aHR0cHM6Ly91ZWxl/Y3Ryb25pY3MuY29t/L3dwLWNvbnRlbnQv/dXBsb2Fkcy9BUjAw/MzNfS1kwMTVfNC5q/cGc" width="150"/> | Mide la temperatura y humedad.                                                                                             | 1            |
| **Sensor de Temperatura Corporal** | <img src="https://uelectronics.com/wp-content/uploads/2018/08/AR0608-MLX90614_v1.png" alt="Sensor de Temperatura Corporal" width="150"/>                                  | Mide la temperatura del cuerpo del usuario en grados Celsius (°C).                                                          | 1            |
| **Sensor de Pulso y Oxigenación** | <img src="https://sensorembedded.com/image/cache/catalog/products/SENSOR/SEN-BIOMEDICAL/1892-MAX30102-HEART/1892-max30102-pulse-oximeter-heart-rate-sensor-module-i2c-interface-sensor-800x800.jpg" alt="Sensor de Pulso y Oxigenación" width="150"/>                                  |Mide la frecuencia cardíaca y la saturación de oxígeno en la sangre (SpO2). Ideal para monitorear la salud cardiovascular.                                                          | 1            |


### Actuadores

| **Nombre**          | **Imagen**                                                                                                                                       | **Descripción**                                                                                          | **Cantidad** |
|---------------------|-------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------|--------------|
| **Vibrador**        | <img src="https://th.bing.com/th/id/OIP.TSWqQVDBHdYjVSCO-fGOjwHaHa?rs=1&pid=ImgDetMain" alt="Botón" width="150"/>                        |Proporciona retroalimentación háptica, vibrando para alertar o notificar al usuario.                                        | 1            |
| **Pantalla OLED**   | <img src="https://pigra.com.mx/1106-medium_default/pantalla-oled-13-pulgadas-128x64-i2c.jpg" alt="Pantalla OLED" width="150"/>                   | Muestra información relevante del dispositivo, como las métricas de salud y actividad física.             | 1            |
| **Buzzer**          | <img src="https://www.electronicaembajadores.com/Datos/fotos/articulos/grandes/zb/zbpz/zbpzs02.jpg" alt="Buzzer" width="150"/>                  | Emite sonidos para alertar o notificar al usuario.                                                        | 1            |
| **Potenciómetro**          | <img src="https://www.audiotestserviciotecnico.com/wp-content/uploads/2021/06/B315D3E3-E177-4E56-8CCD-D6481BEA124E.png" alt="Potenciómetro" width="150"/>                 | Regula la intensidad de una función, permitiendo ajustes finos en el dispositivo.                                                      | 1            |

### Material Extra

| **Nombre**            | **Imagen**                                                                                                                                       | **Descripción**                                                                                                         | **Cantidad** |
|-----------------------|-------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------|--------------|
| **Cables Jumplers**    | <img src="https://github.com/user-attachments/assets/cb167355-1ec3-4e1b-b23e-2fc16109dde3" width="150"/>                                      | Cables de conexión Dupont, utilizados para realizar prototipos y conectar diferentes módulos y sensores.                | 30           |
| **Pila**              | <img src="https://github.com/user-attachments/assets/4d1350c6-8ba6-45a4-a824-8efb1114b069" width="150"/>                                       | Batería Steren TEL-BAT101, utilizada como repuesto en teléfonos inalámbricos para asegurar su funcionamiento continuo.   | 1            |
| **Cable Micro USB**   | <img src="https://m.media-amazon.com/images/I/61+gz2-AA0L._AC_SX569_.jpg" alt="Cable Micro USB" width="150"/>                                    | Cable Micro USB de 1 metro, perfecto para conectar la ESP32 a la computadora, proporcionando una conexión estable.       | 1            |
| **Kit de soldador**   | <img src="https://m.media-amazon.com/images/I/61KhZCpqmYL._AC_SX425_.jpg" alt="Kit de soldador" width="150"/>                                    | Kit de soldador para proyectos electrónicos, equipado con un soldador ergonómico, alambre de estaño y herramientas.     | 1            |
| **Protoboard**        | <img src="https://m.media-amazon.com/images/I/61p+QTbMf5L._SX342_.jpg" alt="Protoboard" width="150"/>                                           | Protoboard, utilizada para el prototipado y pruebas de circuitos, permitiendo la conexión temporal sin soldadura.       | 1            |


## Tabla de Requisitos del Proyecto 'Fit Wearable'

| ID  | Como un   | Quiero poder                                         | Para                                           | Con la prioridad de | En el sprint número | Definición de terminado                                                                                                                                                   |
|-----|-----------|------------------------------------------------------|------------------------------------------------|---------------------|---------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1   | Usuario   | Mandar alerta por ritmo cardiaco alto    | Monitorear mi actividad física diaria          | Debería tenerlo (Media) | 1                   | **Prueba:** Tener un ritmo cardiaco alto. **Criterio de aceptación:** El buzzer se manda una alerta.                                                                         |
| 2   | Usuario   | Ver mi ritmo cardiaco                                | Monitorear mi actividad física diaria          | Debe tenerlo (Alta)  | 1                   | **Prueba:** Someter al usuario a caminar, correr y descansar. **Criterio de aceptación:** Mostrar el ritmo cardiaco ante las diferentes situaciones.                        |
| 3   | Usuario   | Ver mi temperatura corporal                          | Monitorear mi actividad física diaria          | Debe tenerlo (Alta)  | 1                   | **Prueba:** Poner el sensor en una distancia para detectar la temperatura. **Criterio de aceptación:** Mostrar la temperatura del usuario actual.                           |
| 4   | Usuario   | Monitorear el ritmo cardíaco durante diferentes actividades físicas | Obtener información sobre mi ritmo cardíaco en tiempo real | Se Debe incluir (Alta) | 2                   | **Prueba:** Someter al usuario a caminar, correr y descansar. **Criterio de aceptación:** Mostrar el ritmo cardiaco en la pantalla del wearable en todo momento.             |
| 5   | Usuario   | Monitorear los pasos caminado                | Monitorear mi actividad física diaria | Se Debería Incluir (Media) | 2                   | **Prueba:** El usuario tiene pasos caminados. **Criterio de aceptación:** Se visualizan los pasos caminados en tiempo real.                         |
| 6   | Usuario   | Ver un registro histórico de mi ritmo cardiaco       | Seguir la evolución del ritmo cardíaco a lo largo del tiempo | Se puede incluir (baja) | 2                   | **Prueba:** El usuario tiene diferentes actividades durante un periodo de una semana. **Criterio de aceptación:** El usuario visualiza su ritmo cardiaco.                  |
| 7   | Usuario   | Mandar alerta desde el celular para encontrar el wearable en caso de haberlo perdido.    | Tener localizado el wearable          | Debe Tenerlo (Alta) | 3                   | **Prueba:** Tener lejos el Wearable **Criterio de aceptación:** El buzzer manda sonido hasta que se encuentre el wearable.                                                                         |


## Fotografía del prototipo
<br> ![image](https://github.com/rodrigovm10/wearable/assets/114689978/0766e0f0-808c-440b-b40a-995d4b8a3176)

## Arquitectura

![image](https://github.com/user-attachments/assets/fda5ba98-0a06-444c-b303-84f0367a2933)




## Captura Kanban

### Sprint 1
![Sprint 1 - Tablero](https://github.com/rodrigovm10/wearable/assets/104101668/c748fbd1-87da-4874-a1c7-0137aec2078c)

### Sprint 2
![Sprint 2 - Tablero](https://github.com/rodrigovm10/wearable/assets/104101668/f8292947-bfd3-4e6c-bf9d-68c855fe9b24)

### Sprint 3
![Sprint 3 - Tablero](https://github.com/rodrigovm10/wearable/assets/104101668/fdec2a05-5779-423d-a64b-eacc92afcdff)

## Circuito diseñado
### Sensor Pulso
![Requisito 1 y 2](https://github.com/rodrigovm10/wearable/assets/104101668/be32d3fd-2640-408b-a074-542a72699945)
### Sensor Termometro Infrarojo
![Requisito 3](https://github.com/rodrigovm10/wearable/assets/104101668/dc430b0a-ed7c-4b42-a138-0944aacae749)
### Sensor Acelerometro
![image](https://github.com/rodrigovm10/wearable/assets/104101668/7484440d-d164-42e7-a53d-ae048272bd34)

## Resultados
<p align="justify">
Los resultados obtenidos hasta el momento han sido satisfactorios. En los primeros tres requisitos, o en el primer Split, se han cumplido las expectativas previstas.
Dentro de los primeros dos requisitos se cumplio la funcionalidad de alertar en caso de que el ritmo cardiaco no sea normal en el usaurio.
Dentro del tercer requisito se consiguio obtener la temperatura, dentro de este requisito se tuvo que soldar los pines al sensor, por lo que fue más tardado de cumplir. Además el soldado no quedo como el esperado por lo que se encuentra flojo, pero se espera corregir esta parte.

Para el siguiente split, se espera seguir con los mismos resultados obtenidos hasta el momento y que todo sea satisfactorio.
</p>

## Carta de agradecimiento
![Imagen de WhatsApp 2024-08-26 a las 12 21 55_62ce5f07](https://github.com/user-attachments/assets/af240148-b9ad-4d15-a0f6-7926cca76d5a)

## Video mostrando el proyecto
[Video Wearable Fit](https://drive.google.com/file/d/1l205OfwFnMJzpUFPbZxNn3izrPtvoUTY/view?usp=sharing)
[Video de agradecimiento](https://drive.google.com/file/d/1fgn8PI5p78b-DOR3aJ8Lql8_4v2Fnp7q/view?usp=drive_link)

## Código Dashboard
[Dashboard Flutter](https://github.com/rodrigovm10/wearable_fit/tree/main)
## Capturas del Dashboard
### Pantalla principal
![Imagen de WhatsApp 2024-08-29 a las 00 58 28_48055e53](https://github.com/user-attachments/assets/a0e73e17-30dc-4db3-80d1-860667c889ae)
### Grafica de temperatura
![Imagen de WhatsApp 2024-08-29 a las 00 58 44_53b0a1ed](https://github.com/user-attachments/assets/36f8285e-4532-4a35-8b8e-452c95864096)
### Grafica de pulso
![Imagen de WhatsApp 2024-08-29 a las 00 58 54_51df46c5](https://github.com/user-attachments/assets/9f98f53b-ed22-4a6a-8877-6101d11466f3)
### Grafica de humedad
![Imagen de WhatsApp 2024-08-29 a las 00 59 05_2d418852](https://github.com/user-attachments/assets/f89b943a-5723-433e-a307-85e458b350e9)
### Grafica de potenciometro
![Imagen de WhatsApp 2024-08-29 a las 00 59 15_43178222](https://github.com/user-attachments/assets/7ae2fd3a-990c-4f78-bb55-91c880a55421)
### Grafica de oximetro
![Imagen de WhatsApp 2024-08-29 a las 00 59 26_54f66f35](https://github.com/user-attachments/assets/0d8858b3-1cc0-4b43-a404-beee6fa343c5)
### Datos de sensores
![Imagen de WhatsApp 2024-08-29 a las 00 59 40_4d0a6b10](https://github.com/user-attachments/assets/443a98eb-b348-4c09-bcf5-d0e96de783fe)
![Imagen de WhatsApp 2024-08-29 a las 00 59 49_e0c0b76a](https://github.com/user-attachments/assets/6198f841-1c05-4868-b5e8-148565aca44a)







