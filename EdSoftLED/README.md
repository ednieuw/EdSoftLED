# EdSoftLED V1.0.0
Arduino library for SK6812 based 4-channel RGBW and WS2812 based 3-channel RGB LEDs.


void     fill(RGBW RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);	
void     fill(uint32_t RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);
void     setBrightness(uint16_t Bright);
void     setPixelColor(uint16_t i, RGBW RGBWColor);	
void     setPixelColor(uint16_t i, uint32_t RGBWColor);
void     show(void);
uint8_t  getwhite(uint32_t c);
uint8_t  getred(  uint32_t c);
uint8_t  getgreen(uint32_t c);
uint8_t  getblue( uint32_t c);	
uint32_t getPixelColor(uint16_t index);
uint32_t makeRGBWcolor( uint32_t Red, uint32_t Green, uint32_t Blue, uint32_t White);	


## Usage
1. Download zip file
2. In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library
3. Include the library in your project using "#include <EdSoftLED.h>" directive


## Example

#include <EdSoftLED.h>
EdSoftLED LED(4, 5, SK6812WRGB);          // EdSoftLED LED(NUM_LEDS, LED_PIN, LED_type); // WS2812RGB
uint32_t color1 = 0X000000FF;             // 0xWWRRGGBB  

void setup() 
{                                         // LED.fill(RGBWColor, FirstLed, NoofLEDs );
  LED.fill(color1, 1, 5 );                // Fill LEDs 2,3 and 4 with color1 
}

void loop() 
{  
 LED.setPixelColor(0, {0, 0, 0, 255});    // Set first LED to white (using only W channel)
 LED.show();                              // Send the values to the LEDs
 delay(500);
  
 LED.setPixelColor(0, 0X00FF0000);        // Set first LED to green white (using only RGB channels)
 LED.show();
 delay(500);
}




