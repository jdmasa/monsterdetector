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
- **D5** (GPIO14) - Alejar monstruo (distancia)
- **D6** (GPIO12) - Disminuir nivel
- **D7** (GPIO13) - Aumentar nivel
- **FLASH** (GPIO0) - Acercar monstruo (distancia)

## Sistema de Dos Variables

El detector maneja dos variables independientes:

### Distancia del Monstruo (1-6)
Controla la proximidad visual del monstruo y los mensajes de alerta:

**Distancia 1: CLEAR** - Todo despejado, sin monstruos

**Distancia 2: SIGNAL** - Algo se detecta, pero aún no confirmado

**Distancia 3-4: DETECTED** - ¡Monstruo confirmado!
- Aparece una huella en el radar
- Distancia 3: Lejos (90 píxeles del centro)
- Distancia 4: Media (60 píxeles del centro)

**Distancia 5: ALERT** - ¡Muy cerca!
- La huella aparece a 40 píxeles del centro

**Distancia 6: DANGER!** - ¡El monstruo está encima!
- La pantalla se invierte (colores invertidos)
- La huella aparece muy cerca del centro (28 píxeles)

### Nivel (0-99)
Contador independiente que puede usarse como:
- Sistema de puntuación
- Contador de monstruos encontrados
- Nivel de dificultad del juego
- Cualquier otro propósito creativo

## Controles

**Control de Distancia del Monstruo:**
- **Botón FLASH**: Acercar el monstruo (aumenta distancia 1→6)
- **Botón D5**: Alejar el monstruo (disminuye distancia 6→1)

**Control de Nivel:**
- **Botón D7**: Aumentar nivel (0→99)
- **Botón D6**: Disminuir nivel (99→0)

## Características

### Animación de Radar
- Pulso de radar cada segundo
- Onda que se expande desde el centro inferior
- Efecto visual realista tipo sonar

### Indicadores Visuales
- Cono de radar con 4 anillos de distancia
- Huella del monstruo que se mueve verticalmente según la distancia
- Texto de estado en pantalla (CLEAR, SOMETHING, DETECTED, ALERT, DANGER!)
- Indicador de nivel actual (0-99) con formato "LV:XX"

### Monitor Serial
Información de depuración disponible a 115200 baudios:
- Mensajes "PING!" en cada pulso de radar
- Estado de la distancia cuando cambia
- Cambios en el nivel

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

### Modo Básico: Detector de Proximidad
1. **Inicio**: El detector arranca en Distancia 1 (todo despejado) y Nivel 0
2. **Acercamiento del monstruo**: Presiona FLASH repetidamente para que el monstruo se acerque (distancia 1→6)
3. **Alejamiento del monstruo**: Presiona D5 para que el monstruo se aleje (distancia 6→1)
4. **Máxima alerta**: Al llegar a Distancia 6, la pantalla se invierte indicando peligro extremo

### Modo Juego: Sistema de Puntuación
1. **Buscar monstruos**: Camina por la casa buscando "monstruos" escondidos
2. **Cuando encuentres uno**: Presiona FLASH para acercarte (aumentar distancia)
3. **Capturar el monstruo**: Al llegar a Distancia 6, presiona D7 para aumentar tu puntuación
4. **Siguiente monstruo**: Presiona D5 varias veces para resetear la distancia a 1
5. **Meta**: ¡Intenta capturar 99 monstruos!

### Ideas Creativas
- **Contador de sustos**: Usa el nivel para contar cuántas veces asustaste a alguien
- **Dificultad progresiva**: El nivel indica qué tan difícil es encontrar el siguiente monstruo
- **Sistema de vidas**: Cada vez que el monstruo llega a Distancia 6, pierdes una vida (nivel baja)
- **Temporizador**: Usa el nivel como cuenta regresiva (empieza en 99 y baja a 0)

## Librerías Utilizadas

- **Adafruit SSD1306** v2.5.16
- **Adafruit GFX Library** v1.12.4
- **Wire** (I2C)
- **SPI**

## Estructura del Código

```
src/main.cpp
├── Variables globales
│   ├── monsterDistance (1-6) - Proximidad visual del monstruo
│   ├── currentLevel (0-99) - Contador de nivel/puntuación
│   ├── lastRadarPulse - Control de temporización del pulso
│   └── radarWaveRadius - Radio actual de la onda de radar
├── setup() - Inicialización de hardware y pantalla
├── loop() - Lectura de botones y actualización
│   ├── Botón FLASH → Aumenta monsterDistance
│   ├── Botón D5 → Disminuye monsterDistance
│   ├── Botón D7 → Aumenta currentLevel
│   └── Botón D6 → Disminuye currentLevel
├── drawRadar() - Dibuja el cono, huella y estado
│   ├── Dibuja anillos concéntricos
│   ├── Posiciona huella según monsterDistance
│   └── Muestra texto de estado y nivel
└── updateMonsterDetector() - Animación del pulso
    ├── Controla pulso cada 1 segundo
    └── Dibuja onda expansiva
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
