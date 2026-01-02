#include <Arduino.h>
/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

// Button pins (D5=GPIO14, D6=GPIO12, D7=GPIO13 on NodeMCU/D1 Mini)
#define BUTTON_D5 14  // D5
#define BUTTON_D6 12  // D6
#define BUTTON_D7 13  // D7
#define BUTTON_FLASH 0  // GPIO0 - FLASH button

// Button state tracking
bool lastButtonStateD5 = HIGH;
bool lastButtonStateD6 = HIGH;
bool lastButtonStateD7 = HIGH;
bool lastButtonStateFlash = HIGH;

// Monster detector variables
// Distance levels: 1-2=clear, 3-4=something detected, 5=immediate danger, 6=right on top!
int monsterDistance = 1;   // 1 to 6
int monsterDirection = 0;  // -100 (left) to 100 (right), 0 is center
unsigned long lastRadarPulse = 0;
int radarWaveRadius = 0;   // Current radius of the radar wave animation

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16


static const unsigned char PROGMEM cat16x16_bmp[] = {
  0b00001100, 0b00001100,
  0b00011110, 0b00011110,
  0b00111111, 0b00111111,
  0b01111111, 0b11111111,
  0b01101111, 0b11110110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b00111111, 0b11111100,
  0b00011101, 0b11101110,
  0b00001100, 0b00001100,
  0b00011110, 0b00011110,
  0b00110011, 0b10011001,
  0b00100001, 0b10010001,
  0b00000001, 0b11000000,
  0b00000011, 0b11110000,
  0b00000000, 0b00000000
};
static const unsigned char epd_bitmap_huella [] PROGMEM = {
	0x0f, 0xf0, 0x38, 0x1c, 0x61, 0x06, 0x43, 0x82, 0xc3, 0x93, 0x9f, 0xb1, 0x9f, 0xb9, 0x9d, 0xb1, 
	0x89, 0xb3, 0x87, 0xef, 0x8f, 0xff, 0xcf, 0xf7, 0x47, 0xe2, 0x60, 0x06, 0x38, 0x1c, 0x0f, 0xf0
};
  
// Monster detector functions
void drawRadar();
void updateMonsterDetector();

void setup() {
  Serial.begin(115200);
  Wire.begin(5, 4);

  // Configure button pins with internal pull-up resistors
  pinMode(BUTTON_D5, INPUT_PULLUP);
  pinMode(BUTTON_D6, INPUT_PULLUP);
  pinMode(BUTTON_D7, INPUT_PULLUP);
  pinMode(BUTTON_FLASH, INPUT_PULLUP);

  // Wait for display and serial
  delay(500);

  Serial.println("Button monitor ready. Buttons on D5, D6, D7, and FLASH");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Rotate the display (0=0°, 1=90°, 2=180°, 3=270°)
  display.setRotation(1);

  // Clear the buffer and show welcome screen
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("MONSTER"));
  display.setCursor(0, 40);
  display.println(F("DETECTOR"));
  display.display();
  delay(2000);

  Serial.println("Monster Detector initialized!");
}

void loop() {
  // Read current button states (LOW = pressed when using INPUT_PULLUP)
  bool currentStateD5 = digitalRead(BUTTON_D5);
  bool currentStateD6 = digitalRead(BUTTON_D6);
  bool currentStateD7 = digitalRead(BUTTON_D7);
  bool currentStateFlash = digitalRead(BUTTON_FLASH);

  // FLASH button: Monster getting CLOSER (increase level)
  if (currentStateFlash == LOW && lastButtonStateFlash == HIGH) {
    monsterDistance = min(6, monsterDistance + 1);
    Serial.print("Monster closer! Level: ");
    Serial.println(monsterDistance);
  }
  lastButtonStateFlash = currentStateFlash;

  // D5 button: Monster getting FARTHER (decrease level)
  if (currentStateD5 == LOW && lastButtonStateD5 == HIGH) {
    monsterDistance = max(1, monsterDistance - 1);
    Serial.print("Monster farther! Level: ");
    Serial.println(monsterDistance);
  }
  lastButtonStateD5 = currentStateD5;

  // D6 button: Monster to the RIGHT (decrease direction)
  if (currentStateD6 == LOW && lastButtonStateD6 == HIGH) {
    monsterDirection = min(100, monsterDirection + 20);
    Serial.print("Monster right! Direction: ");
    Serial.println(monsterDirection);
  }
  lastButtonStateD6 = currentStateD6;

  // D7 button: Monster to the LEFT (increase direction)
  if (currentStateD7 == LOW && lastButtonStateD7 == HIGH) {
    monsterDirection = max(-100, monsterDirection - 20);
    Serial.print("Monster left! Direction: ");
    Serial.println(monsterDirection);
  }
  lastButtonStateD7 = currentStateD7;

  // Update radar display
  updateMonsterDetector();

  // Small delay for debouncing
  delay(50);
}

// Draw the radar cone display
void drawRadar() {
  // Display is 64x128 in rotated position (rotation=1)
  // Origin (radar center) at bottom center
  int centerX = display.width() / 2;  // 64
  int centerY = display.height();     // 128 (bottom)

  // Draw concentric arcs for distance rings (4 rings)
  for(int i = 1; i <= 4; i++) {
    int radius = i * 30;
    // Draw arc from -60 degrees to +60 degrees (120 degree cone)
    for(int angle = -60; angle <= 60; angle += 2) {
      float rad = angle * PI / 180.0;
      int x = centerX + radius * sin(rad);
      int y = centerY - radius * cos(rad);
      if(x >= 0 && x < display.width() && y >= 0 && y < display.height()) {
        display.drawPixel(x, y, SSD1306_WHITE);
      }
    }
  }

  // Draw cone edges
  int maxRadius = 120;
  display.drawLine(centerX, centerY,
                   centerX - maxRadius * sin(60 * PI / 180),
                   centerY - maxRadius * cos(60 * PI / 180),
                   SSD1306_WHITE);
  display.drawLine(centerX, centerY,
                   centerX + maxRadius * sin(60 * PI / 180),
                   centerY - maxRadius * cos(60 * PI / 180),
                   SSD1306_WHITE);

  // Draw center line
  display.drawLine(centerX, centerY, centerX, 0, SSD1306_WHITE);

  // Draw status message based on distance level
  display.setTextSize(1);  // Larger font
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if(monsterDistance == 1 || monsterDistance == 2) {
    display.println(F("CLEAR"));
  } else if(monsterDistance == 3 || monsterDistance == 4) {
    display.println(F("DETECTED"));

    // Draw monster (cat) for levels 3-4
    int blipRadius = (monsterDistance == 3) ? 90 : 60;
    float blipAngle = map(monsterDirection, -100, 100, -60, 60) * PI / 180.0;
    int blipX = centerX + blipRadius * sin(blipAngle);
    int blipY = centerY - blipRadius * cos(blipAngle);
    // Center the bitmap (16x16) on the calculated position
    display.drawBitmap(blipX - 8, blipY - 8, epd_bitmap_huella, 16, 16, SSD1306_WHITE);
  } else if(monsterDistance == 5) {
    display.invertDisplay(false); // Invert display for alert
    display.println(F("ALERT"));

    // Draw monster (cat) closer
    int blipRadius = 40;
    float blipAngle = map(monsterDirection, -100, 100, -60, 60) * PI / 180.0;
    int blipX = centerX + blipRadius * sin(blipAngle);
    int blipY = centerY - blipRadius * cos(blipAngle);
    // Center the bitmap (16x16) on the calculated position
    display.drawBitmap(blipX - 8, blipY - 8, epd_bitmap_huella, 16, 16, SSD1306_WHITE);
  } else if(monsterDistance == 6) {
    display.invertDisplay(true); // Invert display for alert
    display.println(F("DANGER!"));

    // Draw monster (cat) right in the center (monster is right on top!)
    display.drawBitmap(centerX - 8, centerY - 28, epd_bitmap_huella, 16, 16, SSD1306_WHITE);
  }

  // Show level number
  display.setTextSize(2);
  display.setCursor(0, 18);
  display.print(F("LVL:"));
  display.print(monsterDistance);
}

// Update the monster detector display
void updateMonsterDetector() {
  // Handle radar pulse wave animation (every second)
  unsigned long currentTime = millis();

  // Check if we should start a new pulse
  if(currentTime - lastRadarPulse >= 1000) {
    lastRadarPulse = currentTime;
    radarWaveRadius = 0;  // Start new wave from center
    Serial.println("PING!");
  }

  // Animate the wave if it's active
  if(radarWaveRadius < 120) {
    radarWaveRadius += 8;  // Wave speed
  }

  // Draw radar display
  display.clearDisplay();
  drawRadar();

  // Draw the expanding wave arc
  if(radarWaveRadius < 120) {
    int centerX = display.width() / 2;
    int centerY = display.height();

    // Draw wave arc from -60 to +60 degrees
    for(int angle = -60; angle <= 60; angle += 1) {
      float rad = angle * PI / 180.0;
      int x = centerX + radarWaveRadius * sin(rad);
      int y = centerY - radarWaveRadius * cos(rad);
      if(x >= 0 && x < display.width() && y >= 0 && y < display.height()) {
        display.drawPixel(x, y, SSD1306_WHITE);
      }
      // Draw a second line for thickness
      int x2 = centerX + (radarWaveRadius + 1) * sin(rad);
      int y2 = centerY - (radarWaveRadius + 1) * cos(rad);
      if(x2 >= 0 && x2 < display.width() && y2 >= 0 && y2 < display.height()) {
        display.drawPixel(x2, y2, SSD1306_WHITE);
      }
    }
  }

  display.display();
}