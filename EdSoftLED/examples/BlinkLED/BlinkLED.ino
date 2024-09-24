#include "EdSoftLED.h"

EdSoftLED LED(4, D5, WS2812RGB);            // D5 = GPIO-pin 8
// EdSoft_SK6812LED(NUM_LEDS, LED_PIN);

uint32_t color1 = 0X00000044;             // 0xWWRRGGBB . 44 is aproximately a brighness of 25%

void setup() {
  Serial.begin(115200);
  Serial.println("started");
  LED.fill(color1, 1, 4 );                // Fill LEDs 2,3 and 4 with color1 LED.fill(RGBWColor, FirstLed, NoofLEDs );
}

void loop() {
  LED.setPixelColor(0, {0, 0, 0, 44});   // Set first LED to white or blank with WS2812 (using RGBW channel)
  LED.show();                             // Send the values to the LEDs
  delay(500);
  LED.setPixelColor(0, 0X00444400);       // Set first LED to yellow (using only 0xWWRRGGBB )
  LED.show();
  delay(500);
}
