/*
  EdSoftLED_Demo.ino  -  Demonstration of the EdSoftLED library
  Author: Ed Nieuwenhuys  -  ednieuw.nl

  Shows every part of the API:
    - constructor with LED type
    - updateLength()  (allocate big, blank, shrink - kills random LEDs at boot)
    - fill() with packed uint32_t and with an RGBW struct
    - all three setPixelColor() overloads
    - the static Color() packing helpers
    - getPixelColor() + getRed/getGreen/getBlue/getWhite extractors
    - setBrightness()

  Wiring: data line on LED_DATA_PIN. Use a 4-channel SK6812 (RGBW) strip,
  or change LED_TYPE to a WS2812 variant for a 3-channel RGB strip.

  Compile note (Arduino Nano ESP32 / ESP32-S3):
    EdSoftLED talks to the RMT peripheral by GPIO, so either pin-numbering
    mode works. D5 resolves to GPIO 8 on this board.
*/

#include <EdSoftLED.h>

// ---- Configuration ---------------------------------------------------------
#define LED_DATA_PIN   8          // GPIO 8 (= D5 on Nano ESP32)
#define LED_TYPE       SK6812WGRB // SK6812WRGB / SK6812WGRB / WS2812RGB / WS2812GRB

const uint16_t MAX_LEDS = 256;    // Allocate for the full matrix you might ever drive
const uint16_t NUM_LEDS = 12;     // Actual number of LEDs we use in this demo

// Allocate for the WORST case up front. updateLength() then blanks all of them
// and shrinks to NUM_LEDS. This is the trick that stops a big matrix lighting
// up with random colours during upload/boot.
EdSoftLED strip(MAX_LEDS, LED_DATA_PIN, LED_TYPE);

// ---- A few packed colours (0xWWRRGGBB) -------------------------------------
// Built with the static helper so byte order is guaranteed regardless of strip.
const uint32_t RED   = EdSoftLED::Color(255,   0,   0);      // r,g,b
const uint32_t GREEN = EdSoftLED::Color(  0, 255,   0);
const uint32_t BLUE  = EdSoftLED::Color(  0,   0, 255);
const uint32_t WHITE = EdSoftLED::Color(  0,   0,   0, 255); // r,g,b,w  -> uses the W channel
const uint32_t OFF   = 0x00000000;

//-----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(300);
  Serial.println("\nEdSoftLED demo start");

  strip.begin();

  // Blank everything that was allocated, then continue with just NUM_LEDS.
  // After this call all MAX_LEDS are off and the strip is sized to NUM_LEDS.
  strip.updateLength(NUM_LEDS);

  strip.setBrightness(40);        // 0-255 global scale, applied during show()
  strip.show();                   // all off
  Serial.println("Setup done");
}

//-----------------------------------------------------------------------------
void loop()
{
  // 1) fill() with a packed uint32_t - whole strip one colour ----------------
  strip.fill(RED, 0, NUM_LEDS);   // (colour, firstLed, count). Does not show().
  strip.show();
  Serial.println("fill RED");
  delay(800);

  // 2) fill() with an RGBW struct --------------------------------------------
  RGBW teal = { 180, 0, 120, 0 }; // {g, r, b, w}  - note struct field order!
  strip.fill(teal, 0, NUM_LEDS);
  strip.show();
  Serial.println("fill teal (RGBW struct)");
  delay(800);

  // 3) setPixelColor() - three overloads -------------------------------------
  strip.fill(OFF, 0, NUM_LEDS);
  strip.setPixelColor(0, BLUE);                 // packed uint32_t
  strip.setPixelColor(1, (RGBW){0, 255, 0, 0}); // RGBW struct: green via r
  strip.setPixelColor(2, 0, 0, 0, 255);         // r,g,b,w bytes: white via W
  strip.show();
  Serial.println("setPixelColor x3 (packed / struct / bytes)");
  delay(1200);

  // 4) A moving white dot using the W channel --------------------------------
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    strip.fill(OFF, 0, NUM_LEDS);
    strip.setPixelColor(i, WHITE);
    strip.show();
    delay(80);
  }

  // 5) Read a pixel back and split it into channels --------------------------
  strip.fill(OFF, 0, NUM_LEDS);
  strip.setPixelColor(5, EdSoftLED::Color(200, 100, 50));  // orange-ish
  strip.show();

  uint32_t c = strip.getPixelColor(5);
  Serial.printf("Pixel 5 packed = 0x%08lX\n", (unsigned long)c);
  Serial.printf("  R=%u  G=%u  B=%u  W=%u\n",
                strip.getRed(c), strip.getGreen(c),
                strip.getBlue(c), strip.getWhite(c));   // R=200 G=100 B=50 W=0
  delay(1500);

  // 6) Brightness ramp on a full white strip ---------------------------------
  strip.fill(WHITE, 0, NUM_LEDS);
  for (int b = 0; b <= 120; b += 10) { strip.setBrightness(b); strip.show(); delay(60); }
  for (int b = 120; b >= 0; b -= 10) { strip.setBrightness(b); strip.show(); delay(60); }
  strip.setBrightness(40);

  strip.fill(OFF, 0, NUM_LEDS);
  strip.show();
  delay(600);
}
