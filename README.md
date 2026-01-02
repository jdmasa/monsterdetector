# Detector de Monstruos

Un divertido proyecto de radar detector de monstruos para jugar con niños, utilizando ESP8266 y una pantalla OLED SSD1306.

## Descripción

Este proyecto convierte tu ESP8266 en un detector de monstruos estilo radar, similar a los que se ven en las películas de Alien. La pantalla muestra un cono de radar con anillos de distancia y una animación de pulso que se expande desde el centro cada segundo.

## Hardware Necesario

- **ESP8266** (NodeMCU, D1 Mini, o similar)
- **Pantalla OLED SSD1306** 128x64 (I2C)
- **4 Botones** conectados a:
  - D5 (GPIO14)
  - D6 (GPIO12)
  - D7 (GPIO13)
  - FLASH (GPIO0)

## Conexiones

### Pantalla OLED (I2C)
- **SDA** → GPIO5 (D1)
- **SCL** → GPIO4 (D2)
- **VCC** → 3.3V
- **GND** → GND

### Botones
Los botones ya están configurados con resistencias pull-up internas:
- **D5** (GPIO14) - Alejar monstruo
- **D6** (GPIO12) - Monstruo a la derecha
- **D7** (GPIO13) - Monstruo a la izquierda
- **FLASH** (GPIO0) - Acercar monstruo

## Niveles de Alerta

El detector tiene 6 niveles de proximidad del monstruo:

### Nivel 1-2: CLEAR (Despejado)
Sin monstruos detectados. El radar muestra solo los anillos de distancia.

### Nivel 3-4: DETECTED (Detectado)
¡Monstruo detectado! Aparece una huella en el radar:
- **Nivel 3**: Monstruo lejos
- **Nivel 4**: Monstruo a distancia media

### Nivel 5: ALERT (Alerta)
¡El monstruo está muy cerca! La huella aparece más cerca del centro.

### Nivel 6: DANGER! (Peligro)
¡El monstruo está encima de ti!
- La pantalla se invierte (colores invertidos)
- La huella aparece en el centro del radar

## Controles

- **Botón FLASH**: Acercar el monstruo (aumenta nivel 1→6)
- **Botón D5**: Alejar el monstruo (disminuye nivel 6→1)
- **Botón D6**: El monstruo está a la DERECHA
- **Botón D7**: El monstruo está a la IZQUIERDA

## Características

### Animación de Radar
- Pulso de radar cada segundo
- Onda que se expande desde el centro inferior
- Efecto visual realista tipo sonar

### Indicadores Visuales
- Cono de radar con 4 anillos de distancia
- Huella del monstruo que se mueve según dirección
- Texto de estado en pantalla grande
- Indicador de nivel actual

### Monitor Serial
Información de depuración disponible a 115200 baudios:
- Mensajes "PING!" en cada pulso de radar
- Estado del nivel cuando cambia
- Dirección del monstruo

## Compilación y Carga

### Usando PlatformIO

```bash
# Compilar
platformio run

# Compilar y cargar
platformio run --target upload

# Monitor serial
platformio device monitor
```

### Configuración

El proyecto usa la configuración de rotación `display.setRotation(1)` para orientación vertical. Puedes ajustar esto en el código si tu pantalla está montada de forma diferente.

## Cómo Jugar

1. **Inicio**: El detector arranca en Nivel 1 (todo despejado)
2. **Simular acercamiento**: Presiona el botón FLASH repetidamente para que el "monstruo" se acerque
3. **Cambiar dirección**: Usa D6 y D7 para indicar si el monstruo está a la izquierda o derecha
4. **Alejarse**: Presiona D5 para simular que te alejas del monstruo
5. **Máxima alerta**: ¡Al llegar al Nivel 6, la pantalla se invierte indicando peligro extremo!

## Librerías Utilizadas

- **Adafruit SSD1306** v2.5.16
- **Adafruit GFX Library** v1.12.4
- **Wire** (I2C)
- **SPI**

## Estructura del Código

```
src/main.cpp
├── Variables globales (niveles, dirección, estado del radar)
├── setup() - Inicialización de hardware y pantalla
├── loop() - Lectura de botones y actualización
├── drawRadar() - Dibuja el cono del radar y la huella
└── updateMonsterDetector() - Animación del pulso y renderizado
```

## Troubleshooting

### La pantalla no enciende
- Verifica las conexiones I2C (SDA/SCL)
- Comprueba la dirección I2C (0x3C o 0x3D)
- Verifica el voltaje (3.3V, no 5V)

### Los botones no responden
- Verifica que los botones estén conectados correctamente
- Los pines deben ir a GND cuando se presionan
- Ya tienen pull-up interno configurado

### La pantalla está invertida
- Cambia `display.setRotation(1)` a 0, 2, o 3 según tu montaje

## Licencia

Este proyecto está basado en ejemplos de Adafruit y es de código abierto para uso educativo y recreativo.

## Créditos

- Bitmap de huella: Icono personalizado
- Bibliotecas: Adafruit Industries
- Concepto: Detector estilo radar de Alien

---

¡Diviértete cazando monstruos! 👾
