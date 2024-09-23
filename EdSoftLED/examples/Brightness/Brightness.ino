//------------------------------------------------------------------------------
//  Example to show Brightness and makeRGBWcolor
//------------------------------------------------------------------------------

#include "EdSoftLED.h"
const byte LED_PIN   = D5;   // = GPIO 8;                     // Pin number LEDs are connected to
const byte NUM_LEDS  = 24;                                    // How many leds in  strip?

EdSoftLED LED(NUM_LEDS, LED_PIN, WS2812RGB);                  // EdSoft_SK6812LED(NUM_LEDS, LED_PIN);  

//------------------------------------------------------------------------------
//  Setup
//------------------------------------------------------------------------------

void setup() 
{
 LED.setBrightness(10);                                        // Set brightness of LEDs
}
//------------------------------------------------------------------------------
//  Loop
//------------------------------------------------------------------------------

void loop() 
{
 static byte luminosity = 10;
 WhiteOverRainbow(50, 100, 5 );                           // wait, whiteSpeed, whiteLength
 LED.setBrightness(luminosity+=5);
}

//------------------------------------------------------------------------------
//  LED WhiteOverRainbow
//------------------------------------------------------------------------------

void WhiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength ) 
{
  if(whiteLength >= NUM_LEDS) whiteLength = NUM_LEDS - 1;
  int head = whiteLength - 1;
  int tail = 0;
  int loops = 1;
  int loopNum = 0;
  static unsigned long lastTime = 0;
  while(true)
  {
    for(int j=0; j<256; j++) 
     {
      for(int i=0; i<NUM_LEDS; i++) 
       {
        if((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) )
             LED.setPixelColor(i, 0XFFFFFF );
        else  
        LED.setPixelColor(i, Wheel(((i * 256 / NUM_LEDS) + j) & 255));
       }
      if(millis() - lastTime > whiteSpeed) 
       {
        head++;        tail++;
        if(head == NUM_LEDS) loopNum++;
        lastTime = millis();
      }
      if(loopNum == loops) return;
      head %= NUM_LEDS;
      tail %= NUM_LEDS;
       LED.show();
      delay(wait);
    }
  }  // end 
}
//------------------------------------------------------------------------------
//  LED Wheel
//  Input a value 0 to 255 to get a color value.
//  The colours are a transition r - g - b - back to r.
//------------------------------------------------------------------------------
uint32_t Wheel(byte WheelPos) 
{
 WheelPos = 255 - WheelPos;
 if(WheelPos < 85)   { return LED.makeRGBWcolor( 255 - WheelPos * 3, 0, WheelPos * 3, 0);  }
 if(WheelPos < 170)  { WheelPos -= 85;  return LED.makeRGBWcolor( 0,  WheelPos * 3, 255 - WheelPos * 3, 0); }
 WheelPos -= 170;      
 return LED.makeRGBWcolor(WheelPos * 3, 255 - WheelPos * 3, 0, 0);
}
