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

