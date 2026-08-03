# EdSoftLED V1.11.0
Arduino library for SK6812 based 4-channel RGBW and WS2812 based 3-channel RGB LEDs, driven by the ESP32 RMT peripheral.

Use Espressif ESP32 core 3.3.0 or higher.

## What's new in 1.11.0
Corrected typos.

## What's new in 1.10.0
- `begin()` now claims the RMT channel (moved out of the constructor) so only the strip you actually use grabs RMT resources — call it once on the active strip.
- Deeper RMT FIFO (`RMT_MEM_NUM_BLOCKS_4`) to prevent flicker on long strips when WiFi/BLE interrupts compete for the CPU.
- New: `clear()`, `numPixels()`, `getBrightness()`, a whole-strip `fill(uint32_t)` overload, and optional perceptual `setGamma()`.
- A dirty-flag in `show()` skips redundant transmissions when nothing has changed.
- Pixel buffer is zero-initialised at construction (no random LEDs at startup).

## API
```
void     begin(void);
void     updateLength(uint16_t n);
void     fill(RGBW RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);
void     fill(uint32_t RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);
void     fill(uint32_t RGBWColor);                 // whole strip
void     clear(void);                              // blank whole buffer, no show()
void     setBrightness(uint8_t Bright);
uint8_t  getBrightness(void);
void     setGamma(bool on);                        // perceptual gamma, default off
void     setPixelColor(uint16_t i, RGBW RGBWColor);
void     setPixelColor(uint16_t i, uint32_t RGBWColor);
void     setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void     show(void);
uint16_t numPixels(void);
uint8_t  getWhite(uint32_t c);
uint8_t  getRed(  uint32_t c);
uint8_t  getGreen(uint32_t c);
uint8_t  getBlue( uint32_t c);
uint32_t getPixelColor(uint16_t index);
uint32_t makeRGBWcolor(uint32_t Red, uint32_t Green, uint32_t Blue, uint32_t White);
static uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
static uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
```

## Function reference

### begin
`begin()` claims the RMT channel and configures it for this strip. Call it **once**, on the strip you actually use, after construction. The constructor no longer initialises the RMT hardware, so creating several `EdSoftLED` objects (e.g. one for SK6812 and one for WS2812) does not claim RMT resources until you choose one and call `begin()` on it.

```cpp
strip.begin();
```

If you see `init sender failed` on the serial monitor, the RMT memory-block pool is exhausted; lower `RMT_MEM_NUM_BLOCKS_4` to `RMT_MEM_NUM_BLOCKS_2` in `begin()`.

### fill
`fill(RGBWColor, FirstLed, NoofLEDs)` sets a range of LEDs to the same colour. `FirstLed` is zero-based. Accepts either a packed `uint32_t` (0xWWRRGGBB) or an `RGBW` struct. The single-argument `fill(uint32_t)` fills the whole strip. Does not call `show()`.

```cpp
strip.fill(0x000000FF, 0, 4);        // set LEDs 0-3 to blue
strip.fill(0x00FF0000);              // whole strip red
```

### clear
`clear()` blanks the whole pixel buffer (all channels 0). Does not call `show()`, so follow it with `show()` to turn the LEDs off.

```cpp
strip.clear();
strip.show();
```

### setBrightness / getBrightness
`setBrightness(Bright)` sets the global brightness scale (0-255) applied during `show()`. The stored pixel values are not changed, so you can adjust brightness at any time. `getBrightness()` returns the current value.

```cpp
strip.setBrightness(128);            // 50% brightness
uint8_t b = strip.getBrightness();
```

### setGamma
`setGamma(true)` enables perceptual gamma correction; `setGamma(false)` disables it (the default). When enabled, each channel is mapped through a gamma table during `show()` **after** brightness scaling, so fades and low-light dimming look smoother and less steppy. Colours are unchanged at full brightness; the difference is most visible at low levels.

```cpp
strip.setGamma(true);                // smoother dimming
```

### setPixelColor
`setPixelColor(i, color)` sets a single LED at index `i`. Accepts a packed `uint32_t` (0xWWRRGGBB), an `RGBW` struct, or four separate `r, g, b, w` bytes. Does not call `show()`.

```cpp
strip.setPixelColor(0, 0x00FF0000);          // red
strip.setPixelColor(0, {0, 0, 0, 255});      // white via W channel (RGBW struct order is g,r,b,w)
strip.setPixelColor(0, 255, 0, 0, 0);        // red via r,g,b,w bytes
```

### show
`show()` sends the current pixel buffer to the LED strip, applying brightness (and gamma, if enabled). If nothing has changed since the last `show()`, the transmission is skipped automatically. Call after any `setPixelColor`, `fill`, `clear`, or `setBrightness`.

```cpp
strip.show();
```

### numPixels
`numPixels()` returns the number of LEDs currently allocated.

```cpp
for (uint16_t i = 0; i < strip.numPixels(); i++) strip.setPixelColor(i, 0x00FF0000);
```

### getPixelColor
`getPixelColor(i)` returns the stored colour of LED `i` as a packed `uint32_t` (0xWWRRGGBB).

```cpp
uint32_t c = strip.getPixelColor(3);
```

### getWhite / getRed / getGreen / getBlue
Internal wire-order helpers used by `setPixelColor(uint32_t)` and `fill(uint32_t)` to unpack a packed colour into the internal `RGBW` buffer. They are **not** general packed-value accessors: because the internal byte order is tied to the strip's wire order, `getRed`/`getGreen` do not return the R/G bytes of a `makeRGBWcolor()`/`Color()` value. Treat them as library internals rather than a way to read a channel out of an arbitrary packed colour.

If you need to split a packed `0xWWRRGGBB` value into channels in your own code, shift it directly:

```cpp
uint8_t w = (c >> 24) & 0xFF;
uint8_t r = (c >> 16) & 0xFF;
uint8_t g = (c >>  8) & 0xFF;
uint8_t b =  c        & 0xFF;
```

### makeRGBWcolor / Color
`makeRGBWcolor(Red, Green, Blue, White)` packs four channel values into a single `uint32_t` (0xWWRRGGBB). The static `Color(r,g,b)` and `Color(r,g,b,w)` helpers do the same and can be used without an object instance.

```cpp
uint32_t c = strip.makeRGBWcolor(255, 0, 0, 0);   // red
uint32_t d = EdSoftLED::Color(0, 0, 0, 255);      // white via W channel
```

### updateLength
`updateLength(n)` clears and turns off all currently allocated LEDs, then reallocates the strip for `n` LEDs. Useful when you have a large LED matrix that lights up randomly during upload — allocate for the full matrix size at startup, call `updateLength` with the smaller count to blank the matrix before continuing your sketch.

```cpp
EdSoftLED strip(255, 8, SK6812WRGB);  // allocate for full 16x16 matrix
strip.begin();
strip.updateLength(20);               // blanks all 255 LEDs, continues with 20
```

## Usage
1. Download the zip file.
2. In Arduino IDE: Sketch -> Include Library -> Add .ZIP Library.
3. Include the library with `#include <EdSoftLED.h>`.

Initialise with the LED type `SK6812WRGB`, `SK6812WGRB`, `WS2812RGB` or `WS2812GRB` to match the colour sequence of the LEDs used. If the colours do not match, change the type or adjust the 0xWWRRGGBB sequence in your program.

On the ESP32 the data line is driven via the RMT peripheral by GPIO number, so the sketch works in either Arduino-pin or GPIO-number compile mode.

## Example
```cpp
#include <EdSoftLED.h>
EdSoftLED strip(4, 8, SK6812WRGB);        // EdSoftLED strip(NUM_LEDS, LED_PIN, LED_type);
// EdSoftLED strip(4, 8, WS2812RGB);      // if WS2812 LEDs are used
uint32_t color1 = 0x000000FF;             // 0xWWRRGGBB (blue)

void setup()
{
  strip.begin();                          // claim the RMT channel (required)
  strip.setBrightness(64);
  strip.fill(color1, 1, 3);               // fill LEDs 1,2,3 with color1
  strip.show();
}

void loop()
{
  strip.setPixelColor(0, {255, 255, 255, 255}); // first LED white (W channel)
  strip.show();
  delay(500);

  strip.setPixelColor(0, 0x00FF0000);     // first LED red (RGB channels)
  strip.show();
  delay(500);
}
```
