# EdSoftLED Library Manual

## Overview
EdSoftLED is an Arduino library for controlling addressable LED strips (WS2812 and SK6812) on ESP32 microcontrollers using the RMT (Remote Control) peripheral. It supports RGB and RGBW LED strips with hardware-accelerated timing.

## Supported LED Types
- **WS2812RGB** - WS2812 with RGB byte order
- **WS2812GRB** - WS2812 with GRB byte order (most common)
- **SK6812WRGB** - SK6812 with WRGB byte order (includes white channel)
- **SK6812WGRB** - SK6812 with WGRB byte order (includes white channel)

## Installation
1. Copy `EdSoftLED.h` and `EdSoftLED.cpp` to your Arduino libraries folder
2. Include the library in your sketch: `#include "EdSoftLED.h"`

---

## Constructor

### `EdSoftLED(uint16_t num_leds, uint8_t pin, uint8_t LEDtype)`
Creates an LED strip object.

**Parameters:**
- `num_leds` - Number of LEDs in the strip (can be changed later with `setNumLeds()`)
- `pin` - GPIO pin number connected to the LED strip data line
- `LEDtype` - Type of LED strip (WS2812RGB, WS2812GRB, SK6812WRGB, SK6812WGRB)

**Example:**
```cpp
#include "EdSoftLED.h"

// Create strip with 100 LEDs on GPIO 8, SK6812 RGBW type
EdSoftLED strip(100, 8, SK6812WRGB);
```

---

## Core Functions

### `begin()`
Initializes the LED strip. Call this in your `setup()` function.

**Example:**
```cpp
void setup() {
    strip.begin();
}
```

### `show()`
Updates the physical LED strip with the current pixel data from memory. Colors are automatically adjusted by the brightness setting.

**Important:** You must call `show()` after setting pixel colors for changes to appear on the strip.

**Example:**
```cpp
strip.setPixelColor(0, 255, 0, 0, 0);  // Set first LED to red
strip.show();                           // Update the strip
```

### `setBrightness(uint8_t brightness)`
Sets the global brightness level for the entire strip.

**Parameters:**
- `brightness` - Brightness level from 0 (off) to 255 (full brightness)

**Note:** This affects all LEDs when `show()` is called. The original color values are preserved in memory.

**Example:**
```cpp
strip.setBrightness(128);  // Set to 50% brightness
strip.show();              // Apply brightness
```

---

## Setting Individual Pixels

### `setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t w)`
Sets a single LED to the specified RGBW color.

**Parameters:**
- `i` - LED index (0-based)
- `r` - Red value (0-255)
- `g` - Green value (0-255)
- `b` - Blue value (0-255)
- `w` - White value (0-255, use 0 for RGB-only strips)

**Example:**
```cpp
strip.setPixelColor(0, 255, 0, 0, 0);    // Red
strip.setPixelColor(1, 0, 255, 0, 0);    // Green
strip.setPixelColor(2, 0, 0, 255, 0);    // Blue
strip.setPixelColor(3, 0, 0, 0, 255);    // White (RGBW only)
strip.show();
```

### `setPixelColor(uint16_t i, uint32_t color)`
Sets a single LED using a 32-bit color value.

**Parameters:**
- `i` - LED index (0-based)
- `color` - 32-bit RGBW color (format: 0xWWRRGGBB)

**Example:**
```cpp
uint32_t red = strip.makeRGBWcolor(255, 0, 0, 0);
strip.setPixelColor(0, red);
strip.show();
```

### `setPixelColor(uint16_t i, RGBW color)`
Sets a single LED using an RGBW struct.

**Parameters:**
- `i` - LED index (0-based)
- `color` - RGBW struct containing r, g, b, w values

---

## Filling Multiple Pixels

### `fill(uint32_t color, uint16_t first, uint16_t count)`
Fills a range of LEDs with the same color.

**Parameters:**
- `color` - 32-bit RGBW color value
- `first` - Starting LED index
- `count` - Number of LEDs to fill

**Example:**
```cpp
// Fill first 10 LEDs with red
strip.fill(strip.makeRGBWcolor(255, 0, 0, 0), 0, 10);

// Fill LEDs 20-29 with blue
strip.fill(strip.makeRGBWcolor(0, 0, 255, 0), 20, 10);

strip.show();
```

### `fill(RGBW color, uint16_t first, uint16_t count)`
Fills a range of LEDs using an RGBW struct.

---

## Color Functions

### `makeRGBWcolor(uint32_t red, uint32_t green, uint32_t blue, uint32_t white)`
Creates a 32-bit color value from individual RGBW components.

**Parameters:**
- `red` - Red value (0-255)
- `green` - Green value (0-255)
- `blue` - Blue value (0-255)
- `white` - White value (0-255)

**Returns:** 32-bit color value in format 0xWWRRGGBB

**Example:**
```cpp
uint32_t purple = strip.makeRGBWcolor(255, 0, 255, 0);
uint32_t warmWhite = strip.makeRGBWcolor(255, 200, 100, 128);
```

### `getPixelColor(uint16_t i)`
Retrieves the current color of a specific LED.

**Parameters:**
- `i` - LED index (0-based)

**Returns:** 32-bit color value

**Example:**
```cpp
uint32_t color = strip.getPixelColor(5);
uint8_t r = strip.getRed(color);
uint8_t g = strip.getGreen(color);
```

### Color Extraction Functions
- `getRed(uint32_t c)` - Extract red component (0-255)
- `getGreen(uint32_t c)` - Extract green component (0-255)
- `getBlue(uint32_t c)` - Extract blue component (0-255)
- `getWhite(uint32_t c)` - Extract white component (0-255)

**Example:**
```cpp
uint32_t color = strip.getPixelColor(0);
uint8_t red = strip.getRed(color);
uint8_t green = strip.getGreen(color);
uint8_t blue = strip.getBlue(color);
uint8_t white = strip.getWhite(color);
```

---

## Utility Functions

### `clear()`
Sets all LEDs to off (0, 0, 0, 0). Remember to call `show()` to update the physical strip.

**Example:**
```cpp
strip.clear();
strip.show();  // Turn off all LEDs
```

### `setNumLeds(uint16_t num_leds)`
Dynamically changes the number of LEDs in the strip.

**Parameters:**
- `num_leds` - New number of LEDs

**Behavior:**
- When increasing: New LEDs are initialized to off (0,0,0,0)
- When decreasing: LEDs beyond the new count are automatically turned off, and existing LED colors are preserved
- Memory is reallocated to match the new size

**Example:**
```cpp
// Start with 100 LEDs
EdSoftLED strip(100, 8, SK6812WRGB);

// Later, change to 250 LEDs
strip.setNumLeds(250);
strip.fill(strip.makeRGBWcolor(255, 0, 0, 0), 0, 250);
strip.show();

// Reduce to 50 LEDs (LEDs 50-249 automatically turn off)
strip.setNumLeds(50);
strip.show();
```

---

## Complete Examples

### Example 1: Basic Rainbow
```cpp
#include "EdSoftLED.h"

EdSoftLED strip(60, 8, WS2812GRB);

void setup() {
    strip.begin();
    strip.setBrightness(50);
}

void loop() {
    for(int i = 0; i < 60; i++) {
        int hue = (i * 360 / 60);
        // Convert HSV to RGB (simplified)
        uint8_t r = (hue < 120) ? 255 : 0;
        uint8_t g = (hue >= 120 && hue < 240) ? 255 : 0;
        uint8_t b = (hue >= 240) ? 255 : 0;
        
        strip.setPixelColor(i, r, g, b, 0);
    }
    strip.show();
    delay(50);
}
```

### Example 2: Dynamic Strip Resizing
```cpp
#include "EdSoftLED.h"

EdSoftLED strip(256, 8, SK6812WRGB);

void setup() {
    strip.begin();
    strip.setBrightness(100);
}

void loop() {
    // 256 LEDs - Red
    strip.setNumLeds(256);
    strip.fill(strip.makeRGBWcolor(255, 0, 0, 0), 0, 256);
    strip.show();
    delay(2000);
    
    // 100 LEDs - Green (automatically clears 101-256)
    strip.setNumLeds(100);
    strip.fill(strip.makeRGBWcolor(0, 255, 0, 0), 0, 100);
    strip.show();
    delay(2000);
    
    // 50 LEDs - Blue (automatically clears 51-100)
    strip.setNumLeds(50);
    strip.fill(strip.makeRGBWcolor(0, 0, 255, 0), 0, 50);
    strip.show();
    delay(2000);
}
```

### Example 3: Theater Chase Effect
```cpp
#include "EdSoftLED.h"

EdSoftLED strip(60, 8, WS2812GRB);

void setup() {
    strip.begin();
    strip.setBrightness(128);
}

void loop() {
    theaterChase(strip.makeRGBWcolor(255, 0, 0, 0), 50);
    theaterChase(strip.makeRGBWcolor(0, 255, 0, 0), 50);
    theaterChase(strip.makeRGBWcolor(0, 0, 255, 0), 50);
}

void theaterChase(uint32_t color, int wait) {
    for(int a = 0; a < 10; a++) {
        for(int b = 0; b < 3; b++) {
            strip.clear();
            for(int c = b; c < 60; c += 3) {
                strip.setPixelColor(c, color);
            }
            strip.show();
            delay(wait);
        }
    }
}
```

### Example 4: RGBW White Balance
```cpp
#include "EdSoftLED.h"

EdSoftLED strip(30, 8, SK6812WRGB);

void setup() {
    strip.begin();
    strip.setBrightness(200);
}

void loop() {
    // Pure white LED
    strip.fill(strip.makeRGBWcolor(0, 0, 0, 255), 0, 10);
    
    // Warm white (RGB mixing + white LED)
    strip.fill(strip.makeRGBWcolor(255, 200, 100, 100), 10, 10);
    
    // Cool white (RGB mixing + white LED)
    strip.fill(strip.makeRGBWcolor(200, 220, 255, 100), 20, 10);
    
    strip.show();
    delay(5000);
}
```

---

## Technical Notes

### Memory Usage
- Each LED requires 4 bytes for color data (RGBW struct)
- Each LED requires 128 bytes for RMT transmission data (32 bits × 4 bytes per rmt_data_t)
- Example: 100 LEDs = ~12.8 KB RAM

### Timing
- Uses ESP32's RMT peripheral with 100ns tick resolution
- **WS2812**: T0H=0.3µs, T0L=0.9µs, T1H=0.6µs, T1L=0.6µs
- **SK6812**: T0H=0.4µs, T0L=0.8µs, T1H=0.8µs, T1L=0.4µs

### Performance
- `show()` blocks until transmission completes
- Transmission time ≈ 30µs per LED for WS2812, 32µs per LED for SK6812
- Example: 100 LEDs ≈ 3ms transmission time

### Best Practices
1. Call `begin()` once in `setup()`
2. Always call `show()` after updating pixel colors
3. Use `clear()` to turn off all LEDs efficiently
4. When reducing LED count with `setNumLeds()`, extra LEDs are automatically cleared
5. Set brightness before calling `show()` for best results
6. Avoid calling `setNumLeds()` in tight loops (it reallocates memory)

---

## Troubleshooting

**LEDs don't light up:**
- Check power supply (5V, sufficient current: ~60mA per LED at full white)
- Verify GPIO pin connection
- Ensure correct LED type is specified
- Try increasing brightness: `setBrightness(255)`

**Wrong colors:**
- Try different LED type constants (WS2812GRB vs WS2812RGB)
- Check if your strip is RGB or RGBW

**Flickering:**
- Ensure stable power supply
- Add capacitor (1000µF) across power supply
- Keep data wire short or use level shifter for long runs

**Memory issues:**
- Reduce number of LEDs
- Check available heap memory
- Avoid frequent `setNumLeds()` calls

---

## License & Credits
EdSoftLED Library - LED strip control for ESP32
Uses ESP32 RMT peripheral for precise timing control.