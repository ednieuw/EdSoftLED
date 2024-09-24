# EdSoftLED
Library for WS2812 and SK6812 LEDstrips with board ESP32 V3 on a Arduino Nano ESP32

Compiling with the Adafruit Neopixel with an Arduino Nano ESP32 works fine if the board ESP32 version 2.0.13 from Arduino is used.
But to make the software longer time resistant ESP32 version 3.0 from Espressif can be used.
Most libraries, for my programs, work in September 2024 with the new Arduino Nano ESP32 but not the Neopixel library.
Neopixel uses machine language to send a string of data into the LED strip while V3.0 has an Remote Control Transceiver (RMT) peripheral.

Espressif has an example in their documentation for a few RGB WS2812 LEDs but I use the SK6812 RGBW LED strips.<br>
https://docs.espressif.com/projects/arduino-esp32/en/latest/api/rmt.html

The EdSoftLED library is tested upto 256 LEDs. V1.0.0 allocates memory for 512 RGBW LEDs. 

I have to find out how to reallocate a structure and make to allocation of the number of LED flexible.

For now it works.

IThe SK6812 timing to send data into the LED strip can also be used without problem for the WS2812 LED strip. The only difference is 32 bits (4 * 8) must be send for very SK6812 RGBW LED and 24 bits for every WS2812 RGB LED.
In the library two different timings are used. to be sure it works properly 

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

# EdSoftLED V1.0.1
Arduino library for SK6812 based 4-channel RGBW and WS2812 based 3-channel RGB LEDs.

```
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
```

## Usage
1. Download zip file
2. In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library
3. Include the library in your project using "#include <EdSoftLED.h>" directive

Initialize with LED type  SK6812WRGB or WS2812RGB.
If the colours do not match change the 0xWWRRGGBB sequence in your program.

## Example
```
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
  
 LED.setPixelColor(0, 0X00FF0000);        // Set first LED to red (using only RGB channels)
 LED.show();
 delay(500);
}
```
