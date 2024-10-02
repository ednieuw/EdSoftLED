# EdSoftLED
Library for WS2812 and SK6812 LEDstrips with core ESP32 V3.x.x.

It is tested with an Arduino Nano ESP32 but will probably also work with other ESP32 boards.

Compiling with the Adafruit Neopixel library with an Arduino Nano ESP32 works fine when the board Arduino Nano ESP32 core version 2.0.13 from Arduino is used. 

This library can be used for ESP32 core version 3.0 or higher from Espressif.
https://docs.espressif.com/projects/arduino-esp32/en/latest/api/rmt.html

The library is kept compatible with the Neopixel library but not all functionality from the Neopixel library is available.

You can add the code below to keep the software compatible between core V2 using de Neopixel library and core V3 using EdsoftLED
Both libraties use the same coding in the rest of the software except the functions not implemented in this library.

```
  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
      // Code for version 3.x
  #else
      // Code for version 2.x
  #endif
```
For example:
 ```
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
#include "EdSoftLED.h"
#else
#include <Adafruit_NeoPixel.h>
#endif
```

# EdSoftLED V1.0.2
Arduino library for SK6812 based 4-channel RGBW and WS2812 based 3-channel RGB LEDs.

```
void     begin(void);
void     fill(RGBW RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);	
void     fill(uint32_t RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);
void     setBrightness(uint8_t Bright);
void     setPixelColor(uint16_t i, RGBW RGBWColor);	
void     setPixelColor(uint16_t i, uint32_t RGBWColor);
void     setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t w); 
void     show(void);
void     showSK6812(void);
void     showWS2812(void);	
uint8_t  getWhite(uint32_t c);
uint8_t  getRed(  uint32_t c);
uint8_t  getGreen(uint32_t c);
uint8_t  getBlue( uint32_t c);	
uint32_t getPixelColor(uint16_t index);
uint32_t makeRGBWcolor( uint32_t Red, uint32_t Green, uint32_t Blue, uint32_t White);	
static uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
                   { return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;  }
static uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
                   { return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;  }		
```

## Usage
1. Download zip file
2. In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library
3. Include the library in your project using "#include <EdSoftLED.h>" directive

Select board: ESP32 -> Arduino Nano ESP32 (almost at the bottom of a long list)
In the examples the LED strip is connected to pin D5 (= GPIO8)
If the LEDs do not turn on change LED_PIN D5 in 8.
Initialize with LED type  SK6812WRGB or WS2812RGB.
If the colours do not match change the 0xWWRRGGBB sequence in your program.

## Example
```
#include <EdSoftLED.h>
EdSoftLED LED(4, D5, SK6812WRGB);         // EdSoftLED LED(NUM_LEDS, LED_PIN, LED_type); // WS2812RGB
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
  
 LED.setPixelColor(0, 0X00FF0000);        // Set first LED to red (using only RGB channels)
 LED.show();
 delay(500);
}
```

# Timings
The timing for the bit encoded pulses are different for the two LED types. While expermenting I discovered the WS2812 LED strips work fine with the SK6812 timing settings. To be sure this library uses the suggested timings for the selected LED type.

WS2812 timing
```
//
// Note: This example uses a board with 32 WS2812b LEDs chained one
//      after another, each RGB LED has its 24 bit value
//      for color configuration (8b for each color)
//
//      Bits encoded as pulses as follows:
//
//      "0":
//         +-------+              +--
//         |       |              |
//         |       |              |
//         |       |              |
//      ---|       |--------------|
//         +       +              +
//         | 0.4us |   0.85 0us   |
//
//      "1":
//         +-------------+       +--
//         |             |       |
//         |             |       |
//         |             |       |
//         |             |       |
//      ---+             +-------+
//         |    0.8us    | 0.4us |
//
```

SK6812 timing
```
// Note: This example uses a board with 32 SK6812 LEDs chained one
//      after another, each RGB LED has its 24 bit value
//      for color configuration (8b for each color)
//
//      Bits encoded as pulses as follows:
//
//      "0":
//         +-------+              +--
//         |       |              |
//         |       |              |
//         |       |              |
//      ---|       |--------------|
//         +       +              +
//         | 0.29us |   0.85 0us   |
//
//      "1":
//         +-------------+       +--
//         |             |       |
//         |             |       |
//         |             |       |
//         |             |       |
//      ---+             +-------+
//         |    0.8us    | 0.4us |
```
