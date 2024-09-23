#include "EdSoftLED.h"

EdSoftLED LEDsk = EdSoftLED(14, D5, SK6812WRGB);                  // EdSoftLEDLED(NUM_LEDS, LED_PIN);
EdSoftLED LEDws = EdSoftLED(14, D5, WS2812RGB);
EdSoftLED LED   = EdSoftLED();  

uint32_t color1 = 0X0A0A0A00;             // 0xWWRRGGBB  

void setup() {
  Serial.begin(115200);
  Serial.println("Kleur" );

  LED = LEDws;                             // LED = LEDws; Select a WS2812 strip  
 // LED = LEDsk;                             // LED = LEDsk; Select a SK6812 strip
  LED.setBrightness(60);
  LED.fill(color1, 0, 14 );                // Fill LEDs 2,3 and 4 with color1 LED.fill(RGBWColor, FirstLed, NoofLEDs );
}

void loop() {
  
  LED.setPixelColor(0, {0, 0, 0, 0});   // Set second LED to white (using only W channel)
  LED.setPixelColor(3,  0X00000000);       // Set second LED to white (using only RGB channels)
  LED.setPixelColor(6,  0X00000000);       // Set second LED to white (using only RGB channels)  
  LED.setPixelColor(8,  0X00000000);       // Set second LED to white (using only RGB channels)     
  LED.setPixelColor(13, {0, 0, 0, 0});   // Set second LED to white (using only W channel)
  LED.show();                             // Send the values to the LEDs
  delay(500);
  
  LED.setPixelColor(0,  0XAA000000);       // Set second LED to white (using only RGB channels)
  LED.setPixelColor(3,  0X00AA0000);       // Set second LED to red (using only RGB channels)
  LED.setPixelColor(6,  0X00AAAA00);       // Set second LED to yellow (using only RGB channels) 
  LED.setPixelColor(8,  0X0000AA00);       // Set second LED to green (using only RGB channels)    
  LED.setPixelColor(13, 0X000000AA);       // Set second LED to blue (using only RGB channels)
  LED.show();
  delay(500);
}
