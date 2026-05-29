# EdSoftLED V1.9.0
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
void     updateLength(uint16_t n);
```

## Function reference

### fill
`fill(RGBWColor, FirstLed, NoofLEDs)` sets a range of LEDs to the same colour. `FirstLed` is zero-based. Accepts either a packed `uint32_t` (0xWWRRGGBB) or an `RGBW` struct. Does not call `show()`.

```cpp
LED.fill(0x000000FF, 0, 4);          // set LEDs 0–3 to blue
```

### setBrightness
`setBrightness(Bright)` sets the global brightness scale (0–255) applied during `show()`. The stored pixel values are not changed, so you can adjust brightness at any time.

```cpp
LED.setBrightness(128);              // 50% brightness
```

### setPixelColor
`setPixelColor(i, color)` sets a single LED at index `i`. Accepts a packed `uint32_t` (0xWWRRGGBB), an `RGBW` struct, or four separate `r, g, b, w` bytes. Does not call `show()`.

```cpp
LED.setPixelColor(0, 0x00FF0000);           // red
LED.setPixelColor(0, {0, 0, 0, 255});       // white via W channel
LED.setPixelColor(0, 255, 0, 0, 0);        // red via r,g,b,w
```

### show
`show()` sends the current pixel buffer to the LED strip, applying the brightness scaling. Call after any `setPixelColor` or `fill` calls.

```cpp
LED.show();
```

### getPixelColor
`getPixelColor(i)` returns the stored colour of LED `i` as a packed `uint32_t` (0xWWRRGGBB).

```cpp
uint32_t c = LED.getPixelColor(3);
```

### getWhite / getRed / getGreen / getBlue
Extract individual channels from a packed `uint32_t` colour value.

```cpp
uint8_t r = LED.getRed(0x00FF0000);  // returns 255
```

### makeRGBWcolor
`makeRGBWcolor(Red, Green, Blue, White)` packs four channel values into a single `uint32_t` (0xWWRRGGBB).

```cpp
uint32_t c = LED.makeRGBWcolor(255, 0, 0, 0);  // red
```

### updateLength
`updateLength(n)` clears and turns off all currently allocated LEDs, then reallocates the strip for `n` LEDs. Useful when you have a large LED matrix that lights up randomly during upload — allocate for the full matrix size at startup, call `updateLength` with the smaller count to blank the matrix before continuing your sketch.

```cpp
EdSoftLED LED(255, 5, SK6812WRGB);  // allocate for full 16x16 matrix
LED.updateLength(20);               // blanks all 255 LEDs, continues with 20
```

## Usage
1. Download zip file
2. In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library
3. Include the library in your project using "#include <EdSoftLED.h>" directive

Initialize with LED type  SK6812WRGB, SK6812WGRB, WS2812RGB or WS2812GRB to match the colour sequence of the LEDs used.

If the colours do not match change the 0xWWRRGGBB sequence in your program.

## Example
```
#include <EdSoftLED.h>
EdSoftLED LED(4, 5, SK6812WRGB);          // EdSoftLED LED(NUM_LEDS, LED_PIN, LED_type); // WS2812RGB
// EdSoftLED LED(4, 5, WS2812RGB);        // if WS2812 LEDs are used
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


