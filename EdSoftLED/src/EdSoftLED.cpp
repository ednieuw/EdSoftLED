#include "EdSoftLED.h"
//--------------------------------------------
//  Initialyse 
//--------------------------------------------
EdSoftLED::EdSoftLED(uint16_t num_leds, const uint8_t pin, uint8_t LEDtype)
{
	_count_led  = num_leds;
	_pin_number = pin;
	_LEDtype    = LEDtype;
	_pixels     = new RGBW[_count_led];   
	_Orgpixels  = new RGBW[_count_led];
	
  if (!rmtInit(_pin_number, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, 10000000)) 
  {
 //   Serial.print("init sender failed\n");
  }
//  Serial.print("real tick set to: 100ns");

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
	delete [] _Orgpixels;
   //rmt_driver_uninstall( skconfig.channel );	
}

//--------------------------------------------
//  Set Brightness (0 - 255)of a pixel to be written 
//  with show(); to the LED strip
//--------------------------------------------
void EdSoftLED::setBrightness(uint8_t Bright)
{
 Brightness = Bright;
// show();
 return;
}
//--------------------------------------------
//  Write the _pixels array to the strip
//  that will be multiplied with the Brightness factor
//--------------------------------------------

void EdSoftLED::show()
{
if(_LEDtype == SK6812WRGB)	EdSoftLED::showSK6812();
if(_LEDtype == WS2812RGB)	EdSoftLED::showWS2812();	

/*
 uint32_t Kleur = 0;
 uint32_t LedDataBit = 0;                                    // counter for the bit in led_data
 
 memcpy(_Orgpixels, _pixels,_count_led * sizeof(RGBW) );    // backup the last used colours in the strip

 for (uint32_t i = 0; i< _count_led; i++)                   // Prepare the string with dimmed values
	{
 //    Kleur |= ((uint32_t)(_pixels[i].r * Brightness / 255)<<24);
     Kleur |= ((uint32_t)(_pixels[i].r * Brightness / 255)<<16);	 
     Kleur |= ((uint32_t)(_pixels[i].g * Brightness / 255)<<8);
     Kleur |= ((uint32_t)(_pixels[i].b * Brightness / 255));

//     Kleur |= ((uint32_t)(_pixels[i].r * Brightness / 255)<<24);
//     Kleur |= ((uint32_t)(_pixels[i].g * Brightness / 255)<<16);	 
//     Kleur |= ((uint32_t)(_pixels[i].b * Brightness / 255)<<8);
//     Kleur |= ((uint32_t)(_pixels[i].w * Brightness / 255));
//	 Serial.print(i); Serial.print(" : ");Serial.println(Kleur,HEX);
//	 Serial.print(i); Serial.print(" : ");Serial.println(Kleur,BIN);	 
	 
//     for (uint8_t bit = 0; bit < 32; bit++) 
//        {
//         if (Kleur & (1 << (31 - bit)))
     for (uint8_t bit = 0; bit < 24; bit++) 
        {
         if (Kleur & (1 << (23 - bit)))	   	  {
           led_data[LedDataBit].level0 = 1;
           led_data[LedDataBit].duration0 = 6;
           led_data[LedDataBit].level1 = 0;
           led_data[LedDataBit].duration1 = 6;
          } 
		else 
		  {
           led_data[LedDataBit].level0 = 1;
           led_data[LedDataBit].duration0 = 3;
           led_data[LedDataBit].level1 = 0;
           led_data[LedDataBit].duration1 = 9;
          }
	   LedDataBit++;	  
      }
	  Kleur = 0;
	}

 rmtWrite(_pin_number, led_data, _count_led * 24, RMT_WAIT_FOR_EVER);
 //  rmtWrite(_pin_number, led_data, _count_led * 32, RMT_WAIT_FOR_EVER);
 memcpy(_pixels,_Orgpixels, _count_led * sizeof(RGBW) );
 */
}


void EdSoftLED::showSK6812()
{
 uint32_t Kleur = 0;
 uint32_t LedDataBit = 0;                                    // counter for the bit in led_data
 
 memcpy(_Orgpixels, _pixels,_count_led * sizeof(RGBW) );    // backup the last used colours in the strip

 for (uint32_t i = 0; i< _count_led; i++)                   // Prepare the string with dimmed values
	{
     Kleur |= ((uint32_t)(_pixels[i].r * Brightness / 255)<<24);
     Kleur |= ((uint32_t)(_pixels[i].g * Brightness / 255)<<16);	 
     Kleur |= ((uint32_t)(_pixels[i].b * Brightness / 255)<<8);
     Kleur |= ((uint32_t)(_pixels[i].w * Brightness / 255));
//	 Serial.print(i); Serial.print(" : ");Serial.println(Kleur,HEX);
//	 Serial.print(i); Serial.print(" : ");Serial.println(Kleur,BIN);	 
	 
     for (uint8_t bit = 0; bit < 32; bit++) 
        {
         if (Kleur & (1 << (31 - bit)))
  	  {
           led_data[LedDataBit].level0 = 1;
           led_data[LedDataBit].duration0 = 8;
           led_data[LedDataBit].level1 = 0;
           led_data[LedDataBit].duration1 = 4;
          } 
		else 
		  {
           led_data[LedDataBit].level0 = 1;
           led_data[LedDataBit].duration0 = 4;
           led_data[LedDataBit].level1 = 0;
           led_data[LedDataBit].duration1 = 8;
          }
	   LedDataBit++;	  
      }
	  Kleur = 0;
	}
 rmtWrite(_pin_number, led_data, _count_led * 32, RMT_WAIT_FOR_EVER);
 memcpy(_pixels,_Orgpixels, _count_led * sizeof(RGBW) );
}


void EdSoftLED::showWS2812()
{
 uint32_t Kleur = 0;
 uint32_t LedDataBit = 0;                                    // counter for the bit in led_data
 
 memcpy(_Orgpixels, _pixels,_count_led * sizeof(RGBW) );    // backup the last used colours in the strip

 for (uint32_t i = 0; i< _count_led; i++)                   // Prepare the string with dimmed values
	{
     Kleur |= ((uint32_t)(_pixels[i].r * Brightness / 255)<<16);	 
     Kleur |= ((uint32_t)(_pixels[i].g * Brightness / 255)<<8);
     Kleur |= ((uint32_t)(_pixels[i].b * Brightness / 255));

     for (uint8_t bit = 0; bit < 24; bit++) 
        {
         if (Kleur & (1 << (23 - bit)))	   	  {
           led_data[LedDataBit].level0 = 1;
           led_data[LedDataBit].duration0 = 6;
           led_data[LedDataBit].level1 = 0;
           led_data[LedDataBit].duration1 = 6;
          } 
		else 
		  {
           led_data[LedDataBit].level0 = 1;
           led_data[LedDataBit].duration0 = 3;
           led_data[LedDataBit].level1 = 0;
           led_data[LedDataBit].duration1 = 9;
          }
	   LedDataBit++;	  
      }
	  Kleur = 0;
	}

 rmtWrite(_pin_number, led_data, _count_led * 24, RMT_WAIT_FOR_EVER);
 memcpy(_pixels,_Orgpixels, _count_led * sizeof(RGBW) );
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
//	Serial.print(i); Serial.print("-"); Serial.println(_pixels[i].r);
  }
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
//	Serial.print(i); Serial.print ("|"); Serial.println(_pixels[i].r);
  }
}
//------------------------------------------------------------------------------
//  Functions to extract RGBW colors
//------------------------------------------------------------------------------ 
 uint8_t EdSoftLED::getWhite(uint32_t c) { return (c >> 24);}
 uint8_t EdSoftLED::getRed(  uint32_t c) { return (c >>  8);}
 uint8_t EdSoftLED::getGreen(uint32_t c) { return (c >> 16);}
 uint8_t EdSoftLED::getBlue( uint32_t c) { return (c);      }
 