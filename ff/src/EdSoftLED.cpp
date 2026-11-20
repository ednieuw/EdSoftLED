#include "EdSoftLED.h"
//--------------------------------------------
//  Initialyse
//--------------------------------------------
EdSoftLED::EdSoftLED(uint16_t num_leds, const uint8_t pin, uint8_t LEDtype)
{
    _count_led  = num_leds;
    _pin_number = pin;
    _LEDtype    = LEDtype;
    _pixels     = new RGBW[_count_led]();   // () => zero-initialize. Prevents garbage being shown at startup
//    _Orgpixels  = new RGBW[_count_led];
    _LEDdata    = new rmt_data_t[32 * _count_led + 1] ;
    // NOTE: rmtInit() is NOT called here. It now lives in begin(), so the RMT
    // channel is only claimed for the strip you actually use. This avoids
    // claiming RMT memory blocks twice (once per global strip object) which can
    // exhaust the shared block pool. Call begin() once on the chosen strip.
}

EdSoftLED::EdSoftLED()
{
    _count_led  = 1;
    _pin_number = 8;
    _LEDtype    = SK6812WRGB;
}
//--------------------------------------------
//  Exit
//--------------------------------------------
EdSoftLED::~EdSoftLED()
{
    delete [] _pixels;
//    delete [] _Orgpixels;
    delete [] _LEDdata;
//    pinMode(_pin_number, INPUT);
 //  rmt_driver_uninstall( skconfig.channel );
}

//--------------------------------------------
//  Resize LED strip: clears current LEDs, reallocates for new count
//--------------------------------------------
void EdSoftLED::updateLength(uint16_t n)
{
 memset(_pixels, 0, _count_led * sizeof(RGBW));
 show();
 delete [] _pixels;
 delete [] _LEDdata;
 _count_led = n;
 _pixels  = new RGBW[_count_led]();
 _LEDdata = new rmt_data_t[32 * _count_led + 1];
 _dirty = true;
}

//--------------------------------------------
//  Begin - claim the RMT channel for this strip.
//  Call ONCE, on the strip you actually use (see StartLeds()).
//  RMT_MEM_NUM_BLOCKS_4: deeper hardware FIFO so a delayed refill (caused by a
//  WiFi/BLE interrupt) does not starve the data line mid-frame. This is the fix
//  for flicker on long strips (256, 625 LEDs). More blocks = more tolerance to
//  CPU jitter; it does NOT need more memory per LED - the array is streamed.
//  If "init sender failed" appears, drop to RMT_MEM_NUM_BLOCKS_2.
//--------------------------------------------
void EdSoftLED::begin(void)
{
  if (!rmtInit(_pin_number, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_4, 10000000))
  {
   log_e("init sender failed");
  }
  log_e("real tick set to: 100ns");
}



//--------------------------------------------
//  Set Brightness (0 - 255)of a pixel to be written
//  with show(); to the LED strip
//--------------------------------------------
void EdSoftLED::setBrightness(uint8_t Bright)
{
 if (Bright != Brightness) _dirty = true;                   // Brightness changes the output, so mark dirty
 Brightness = Bright;
 return;
}
//--------------------------------------------
//  Get the current brightness (0 - 255)
//--------------------------------------------
uint8_t EdSoftLED::getBrightness(void)
{
 return (uint8_t)Brightness;
}
//--------------------------------------------
//  Enable/disable perceptual gamma correction (default off).
//  When on, each R/G/B/W channel is mapped through _gamma8[] in show()
//  AFTER brightness scaling, making fades and dimming look smoother.
//--------------------------------------------
void EdSoftLED::setGamma(bool on)
{
 if (on != _useGamma) _dirty = true;
 _useGamma = on;
}
//--------------------------------------------
//  Number of LEDs currently allocated
//--------------------------------------------
uint16_t EdSoftLED::numPixels(void)
{
 return _count_led;
}
//--------------------------------------------
//  Write the _pixels array to the strip
//  that will be multiplied with the Brightness factor
//--------------------------------------------
void EdSoftLED::show()
{
    if (!_dirty) return;                                    // Nothing changed since last show() - skip the transmission

    if (_LEDtype == SK6812WRGB || _LEDtype == SK6812WGRB)
        EdSoftLED::showSK6812();

    if (_LEDtype == WS2812RGB || _LEDtype == WS2812GRB)
        EdSoftLED::showWS2812();

    _dirty = false;                                         // Buffer now matches what is on the strip
}
 
//--------------------------------------------
// Show SK6812 LED strip
//--------------------------------------------
void EdSoftLED::showSK6812()
{
 uint32_t Kleur = 0;
 uint32_t LedDataBit = 0;                                    // counter for the bit in led_data
 
 //memcpy(_Orgpixels, _pixels,_count_led * sizeof(RGBW) );    // backup the last used colours in the strip

  for (uint32_t i = 0; i< _count_led; i++)                   // Prepare the string with dimmed values
    {
     uint8_t cr = _pixels[i].r * Brightness / 255;          // Brightness-scaled channels
     uint8_t cg = _pixels[i].g * Brightness / 255;
     uint8_t cb = _pixels[i].b * Brightness / 255;
     uint8_t cw = _pixels[i].w * Brightness / 255;
     if (_useGamma) { cr=_gamma8[cr]; cg=_gamma8[cg]; cb=_gamma8[cb]; cw=_gamma8[cw]; }

     Kleur |= ((uint32_t)cr << 24);
    if(_LEDtype == SK6812WGRB)
      {
       Kleur |= ((uint32_t)cg << 16);
       Kleur |= ((uint32_t)cb << 8);
      }
    else
      {
        Kleur |= ((uint32_t)cb << 16);
        Kleur |= ((uint32_t)cg << 8);
      }
     Kleur |= ((uint32_t)cw);
//     Serial.print(i); Serial.print(" : ");Serial.println(Kleur,HEX);
//     Serial.print(i); Serial.print(" : ");Serial.println(Kleur,BIN);

     for (uint8_t bit = 0; bit < 32; bit++)
        {
      if (Kleur & (1UL << (31 - bit)))
      {
       _LEDdata[LedDataBit].level0 = 1;
       _LEDdata[LedDataBit].duration0 = 9;  // T1H ≈0.9µs
       _LEDdata[LedDataBit].level1 = 0;
       _LEDdata[LedDataBit].duration1 = 6;  // T1L ≈0.6µs
       }
     else
       {
       _LEDdata[LedDataBit].level0 = 1;
       _LEDdata[LedDataBit].duration0 = 3;  // T0H ≈0.3µs
       _LEDdata[LedDataBit].level1 = 0;
       _LEDdata[LedDataBit].duration1 = 12; // T0L ≈1.2µs
       }
       LedDataBit++;
      }
      Kleur = 0;
    }
 _LEDdata[LedDataBit] = {0, 0, 0, 0};
 rmtWrite(_pin_number, _LEDdata, _count_led * 32, RMT_WAIT_FOR_EVER);
 delayMicroseconds(300);   // REQUIRED for SK6812
// memcpy(_pixels,_Orgpixels, _count_led * sizeof(RGBW) );
}

//--------------------------------------------
// Show WS2812 LED strip
//--------------------------------------------
void EdSoftLED::showWS2812() {
    uint32_t Kleur = 0;
    uint32_t LedDataBit = 0; // counter for the bit in _LEDdata

 //   memcpy(_Orgpixels, _pixels, _count_led * sizeof(RGBW)); // backup the last used colours in the strip

    for (uint32_t i = 0; i < _count_led; i++)  // Prepare the string with dimmed values
    {
        uint8_t r = (uint8_t)(_pixels[i].r * Brightness / 255);
        uint8_t g = (uint8_t)(_pixels[i].g * Brightness / 255);
        uint8_t b = (uint8_t)(_pixels[i].b * Brightness / 255);
        if (_useGamma) { r=_gamma8[r]; g=_gamma8[g]; b=_gamma8[b]; }

        // WS2812 expects 24 bits sent MSB->LSB in the LED’s native byte order.
        // RGB: [R][G][B] ; GRB: [G][R][B]
        if (_LEDtype == WS2812RGB) {
            Kleur = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
        }
        else { // WS2812GRB
            Kleur = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        }

        for (uint8_t bit = 0; bit < 24; bit++) {
            if (Kleur & (1UL << (23 - bit))) {
                _LEDdata[LedDataBit].level0 = 1;
                _LEDdata[LedDataBit].duration0 = 6; // T1H ≈0.6µs
                _LEDdata[LedDataBit].level1 = 0;
                _LEDdata[LedDataBit].duration1 = 6; // T1L ≈0.6µs
            }
            else {
                _LEDdata[LedDataBit].level0 = 1;
                _LEDdata[LedDataBit].duration0 = 3; // T0H ≈0.3µs
                _LEDdata[LedDataBit].level1 = 0;
                _LEDdata[LedDataBit].duration1 = 9; // T0L ≈0.9µs
            }
            LedDataBit++;
        }
        Kleur = 0;
    }
    _LEDdata[LedDataBit] = {0, 0, 0, 0};
    rmtWrite(_pin_number, _LEDdata, _count_led * 24, RMT_WAIT_FOR_EVER);
 //   memcpy(_pixels, _Orgpixels, _count_led * sizeof(RGBW));
}

//--------------------------------------------
// Get Pixel Color
//--------------------------------------------
uint32_t EdSoftLED::getPixelColor(uint16_t i)
{
 uint32_t Retvalue = 0;
 if (i < _count_led)
    Retvalue = makeRGBWcolor(_pixels[i].r,_pixels[i].g,_pixels[i].b,_pixels[i].w);
return(Retvalue);
}
//--------------------------------------------
//  Write an RGBW struct in strip array _pixels
//--------------------------------------------
void EdSoftLED::setPixelColor(uint16_t i, RGBW RGBWColor)
{
    if (i < _count_led)
    {
     _pixels[i].r = RGBWColor.r;
      _pixels[i].g = RGBWColor.g;
     _pixels[i].b = RGBWColor.b;
     _pixels[i].w = RGBWColor.w;
     _dirty = true;
    }
 return;
}
//--------------------------------------------
//  Write an uint32_t in strip array _pixels
//--------------------------------------------
void EdSoftLED::setPixelColor(uint16_t i, uint32_t RGBWColor)
{
    if (i < _count_led)
    {
    _pixels[i].r = getRed(RGBWColor)  ;
    _pixels[i].g = getGreen(RGBWColor);
    _pixels[i].b = getBlue(RGBWColor) ;
    _pixels[i].w = getWhite(RGBWColor);
    _dirty = true;
    }
 return;
}
//--------------------------------------------
//  Write an uint32_t in strip array _pixels
//--------------------------------------------
void EdSoftLED::setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    if (i < _count_led)
    {
    _pixels[i].r = r;
    _pixels[i].g = g;
    _pixels[i].b = b;
    _pixels[i].w = w;
    _dirty = true;
    }
 return;
}

//------------------------------------------------------------------------------
// Function to make RGBW color
//------------------------------------------------------------------------------
uint32_t EdSoftLED::makeRGBWcolor( uint32_t Red, uint32_t Green, uint32_t Blue, uint32_t White)
{
 return ( (White<<24) + (Red << 16) + (Green << 8) + Blue );
}

//--------------------------------------------
// Fill part of the LED Strip with an RGBW color
//--------------------------------------------
void EdSoftLED::fill(RGBW RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs)
{
 uint16_t LastLED = FirstLed + NoofLEDs;
 if (LastLED > _count_led) LastLED = _count_led;
 for(uint16_t i = FirstLed; i < LastLED; i++)
   {
     _pixels[i].r = RGBWColor.r;
      _pixels[i].g = RGBWColor.g;
     _pixels[i].b = RGBWColor.b;
     _pixels[i].w = RGBWColor.w;
//    Serial.print(i); Serial.print("-"); Serial.println(_pixels[i].r);
  }
 _dirty = true;
}
//--------------------------------------------
// Fill part of the LED Strip with an RGBW color
//--------------------------------------------
void EdSoftLED::fill(uint32_t RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs)
{
 uint16_t LastLED = FirstLed + NoofLEDs;
 if (LastLED > _count_led) LastLED = _count_led;
 for(uint16_t i = FirstLed; i < LastLED; i++)
   {
    _pixels[i].r = getRed(RGBWColor)  ;
    _pixels[i].g = getGreen(RGBWColor);
    _pixels[i].b = getBlue(RGBWColor) ;
    _pixels[i].w = getWhite(RGBWColor);
//    Serial.print(i); Serial.print ("|"); Serial.println(_pixels[i].r);
  }
 _dirty = true;
}
//--------------------------------------------
// Fill the WHOLE strip with a packed RGBW colour (convenience overload)
//--------------------------------------------
void EdSoftLED::fill(uint32_t RGBWColor)
{
 fill(RGBWColor, 0, _count_led);
}
//--------------------------------------------
// Blank the whole buffer (all LEDs off). Does not call show().
//--------------------------------------------
void EdSoftLED::clear(void)
{
 memset(_pixels, 0, _count_led * sizeof(RGBW));
 _dirty = true;
}
//------------------------------------------------------------------------------
//  Functions to extract RGBW colors
//------------------------------------------------------------------------------
 uint8_t EdSoftLED::getWhite(uint32_t c) { return (c >> 24);}
 uint8_t EdSoftLED::getRed(  uint32_t c) { return (c >>  8);}
 uint8_t EdSoftLED::getGreen(uint32_t c) { return (c >> 16);}
 uint8_t EdSoftLED::getBlue( uint32_t c) { return (c);      }

//------------------------------------------------------------------------------
//  Gamma correction table (gamma = 2.6). Maps a linear 0-255 value to a
//  perceptually-corrected 0-255 value. Used in show() only when setGamma(true).
//  Standard Adafruit-style table.
//------------------------------------------------------------------------------
const uint8_t EdSoftLED::_gamma8[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,
    2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,
    5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17,
   17, 18, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26,
   26, 27, 28, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 35, 36, 37,
   38, 39, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
   67, 68, 69, 70, 72, 73, 74, 75, 76, 77, 78, 80, 81, 82, 83, 84,
   86, 87, 88, 89, 91, 92, 93, 95, 96, 97, 99,100,101,103,104,106,
  107,109,110,111,113,114,116,117,119,121,122,124,125,127,128,130,
  132,133,135,137,138,140,142,143,145,147,149,150,152,154,156,158,
  159,161,163,165,167,169,171,173,175,177,179,181,183,185,187,189,
  191,193,195,197,199,201,204,206,208,210,212,215,217,219,221,224,
  226,228,231,233,235,238,240,243,245,248,250,253,255,255,255,255 };
