//------------------------------------------------------------------------------
// Digital time in a WS2812 16x16 LED matrix
// Start with the LEDs top left. So Data entry is top left = LED number 0
// For the even rows Turn the LED-strip 180 degrees
//  data in ----> 00 01 02 03 04 05 06 07 08 09 10 11  -->|
//           |<-- 23 22 21 20 19 18 17 16 15 14 13 12  <--|
//           |--> 24 25 26 .....
//------------------------------------------------------------------------------


#include <EdSoftLED.h>
#include <TimeLib.h>                               // Arduino standard library Time or  https://github.com/PaulStoffregen/Time 
#include <Wire.h>                                  // Arduino standard library
#include "RTClib.h"                                // Arduino standard library or https://github.com/adafruit/RTClib 
 
//------------------------------------------------------------------------------
// LED matrix orientation configuration
// Set these to match your physical panel. Use the XY test in setup() to find them:
//   light XY(0,0)=red, XY(1,0)=green, XY(0,1)=blue, then adjust until
//   red=top-left, green=one right, blue=one below.
//------------------------------------------------------------------------------
#define MATRIX_TRANSPOSE   0     // 1 = panel mounted rotated 90deg (swap x/y axes)
#define MATRIX_FLIP_X      0     // 1 = mirror horizontally
#define MATRIX_FLIP_Y      1     // 1 = flip vertically (upside down)
#define MATRIX_SERPENTINE  1     // 1 = rows alternate direction (zig-zag wiring); 0 = all rows same direction
#define MATRIX_ODD_ROWS    1     // which rows are reversed when serpentine: 1 = odd rows, 0 = even rows



const byte LED_PIN       = D5;                     // = GPIO pin 8.  Pin where the LED strip is attached to
const int NUM_LEDS       = 256;                    // How many leds in  strip?
const byte MATRIX_WIDTH  = 16;                     // Width in pixels of the LED strips
const byte MATRIX_HEIGHT = 16;                     // Height in pixels of the LED strips
const byte BRIGHTNESS    = 32;                     // BRIGHTNESS 0 - 255
const uint32_t white     = 0x00FFFFFF;    // 0xFF000000;
EdSoftLED LED(NUM_LEDS, LED_PIN, WS2812RGB);       // EdSoft_SK6812LED(NUM_LEDS, LED_PIN);  

RTC_DS3231 RTCklok;
DateTime Inow;
tmElements_t I;                                                                        // Initialyse a time struct I for internal times used to display the time in the clock
uint32_t lastminute;
uint32_t msTick;                                                                       // Static to remmember the msTick in this function
 

// Definition of the digits 0 - 9, 3 wide, 5 high. 
const byte PROGMEM Getal[10][3][5]  = { 
                     { {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1} },  //0
                     { {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 1} },  //1
                     { {1, 0, 1, 1, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 0, 1} },  //2
                     { {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 1, 1} },  //3
                     { {1, 1, 1, 0, 0}, {0, 0, 1, 0, 0}, {1, 1, 1, 1, 1} },  //4
                     { {1, 1, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 1, 1} },  //5
//                   { {1, 1, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 1, 1} },  //5 An other 5
                     { {1, 1, 1, 1, 1}, {0, 0, 1, 0, 1}, {0, 0, 1, 1, 1} },  //6
                     { {1, 1, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1} },  //7
                     { {1, 1, 1, 1, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 1, 1} },  //8
                     { {1, 1, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 1, 1} }   //9
                     }; 
#define XY_CALIBRATE  0          // set to 1 to show the R/G/B corner test instead of the clock

#if XY_CALIBRATE
void XYTest(void)
{
 LedsOff();
 LED.setPixelColor(XY(0,0), 0x00FF0000);   // origin -> RED
 LED.setPixelColor(XY(1,0), 0x0000FF00);   // +x     -> GREEN
 LED.setPixelColor(XY(0,1), 0x000000FF);   // +y     -> BLUE
 LED.show();
}
#endif



//------------------------------------------------------------------------------
// ARDUINO Setup
//------------------------------------------------------------------------------
void setup() 
{
 Serial.begin(9600);                                                                          // Setup the serial port to 9600 baud       
 LED.begin();
 RTCklok.begin();                                           // If no RTC module is installed use the ATMEGAchip clock
 LED.setBrightness(25);    
 LedsOff();                                                                                   // Turn all LEDs off
 LED.show();                                                                                  // and do not forget LED.show(); to send the data in the LEDstrip
 msTick = millis();
 LedsOff();
LED.setPixelColor(XY(0,0), 0x00FF0000);  // origin -> RED
LED.setPixelColor(XY(1,0), 0x0000FF00);  // +x     -> GREEN
LED.setPixelColor(XY(0,1), 0x000000FF);  // +y     -> BLUE
LED.show();
//while(1) delay(1000);   // freeze so you can look
#if XY_CALIBRATE
 XYTest();
 while (1) delay(1000);          // freeze on the test pattern
#endif
}

// //------------------------------------------------------------------------------
// // Map an (x,y) coordinate to the serpentine LED index.
// // Origin (0,0) = top-left, x to the right, y downward.
// // Adjust the two "flip" lines to match your panel's wiring.
// //------------------------------------------------------------------------------
// uint16_t XY(byte x, byte y)
// {
//  byte t = x; x = y; y = t;            // transpose: swap axes (panel is rotated 90deg)
//  // If still rotated the wrong way, add ONE of these after the swap:
//  // x = (MATRIX_WIDTH  - 1) - x;      // ... try this, or
//  // y = (MATRIX_HEIGHT - 1) - y;      // ... this
//  if (y % 2) return (y * MATRIX_WIDTH) + (MATRIX_WIDTH - 1 - x);
//  else       return (y * MATRIX_WIDTH) + x;
// }

//------------------------------------------------------------------------------
// Map logical (x,y) -> physical LED index, honouring the orientation #defines.
// Logical origin (0,0) = top-left, x to the right, y downward.
//------------------------------------------------------------------------------
uint16_t XY(byte x, byte y)
{
#if MATRIX_TRANSPOSE
 { byte t = x; x = y; y = t; }
#endif
#if MATRIX_FLIP_X
 x = (MATRIX_WIDTH  - 1) - x;
#endif
#if MATRIX_FLIP_Y
 y = (MATRIX_HEIGHT - 1) - y;
#endif

#if MATRIX_SERPENTINE
 bool reverse = (y % 2) ? MATRIX_ODD_ROWS : !MATRIX_ODD_ROWS;
 if (reverse) return (y * MATRIX_WIDTH) + (MATRIX_WIDTH - 1 - x);
 else         return (y * MATRIX_WIDTH) + x;
#else
 return (y * MATRIX_WIDTH) + x;
#endif
}
//------------------------------------------------------------------------------
// ARDUINO Loop
//------------------------------------------------------------------------------
void loop() 
{
 EverySecondCheck();                                                                          // Keep the loop simple
}
//------------------------------------------------------------------------------
// CLOCK Update routine done every second
//------------------------------------------------------------------------------
void EverySecondCheck(void)
{
 if (millis() - msTick > 999)                                                                 // Enter the loop every second
    {
     GetTijd(0);                                                                              // Update I.Second, I.Minute, I.Hour, I.Day, I.Month, I.Year
     msTick = millis();
     if (I.Minute != lastminute)   EveryMinuteUpdate();                                       // Enter the every minute routine after one minute
    }
}
//------------------------------------------------------------------------------
// CLOCK Update routine done every minute
//------------------------------------------------------------------------------
void EveryMinuteUpdate(void)
 {
  lastminute = I.Minute;  
  LedsOff();                                                                                  // Start by clearing the display to a known state   
  TimePlaceDigit(I.Hour,I.Minute);
  LED.show();
  Print_RTC_tijd();
 }
 
//------------------------------------------------------------------------------
//  LED Place digits 0 - 9 in Matrix display
// First row and column = 0, PosX,PosY is left top position of 3x5 digit
// Calculate position LED #define MATRIX_WIDTH 12 #define MATRIX_HEIGHT 12
//------------------------------------------------------------------------------
void Zet_Pixel(byte Cijfer, byte Pos_X, byte Pos_Y)
{
 for (int i = 0; i < 3; i++)
   for (int j = 0; j < 5; j++)
     if (pgm_read_byte_near(&Getal[Cijfer][i][j]))
        LED.setPixelColor(XY(Pos_X + i, Pos_Y + j), white);
}

// void Zet_Pixel(byte Cijfer,byte Pos_X, byte Pos_Y) 
// { 
//  uint32_t LEDnum;
//  for(int i=0;i<3;i++)
//   {  
//    for(int j=0;j<5;j++)
//    {
//     int c = pgm_read_byte_near ( &Getal[Cijfer][i][j]); 
//     if ( c )                                                                                  // if Digit == 1 then turn that light on
//      {                                                                                        // Serial.print(strip.getPixelColor(LEDnum) & 0X00FFFFFF,HEX); Serial.print(" ");
//       if((Pos_Y+j)%2) LEDnum = ((MATRIX_WIDTH -1) - (Pos_X + i) + (Pos_Y + j) * (MATRIX_HEIGHT));
//       else            LEDnum =                      (Pos_X + i) + (Pos_Y + j) * (MATRIX_HEIGHT); 
//       LED.setPixelColor(LEDnum, white);
//      }
//    }
//  }
// }
//------------------------------------------------------------------------------
//  LED Time in four digits in display
//------------------------------------------------------------------------------
void TimePlaceDigit(byte uur, byte minuut)
{   
 Zet_Pixel(    uur / 10, 2, 1);  Zet_Pixel(    uur % 10, 7, 1);
 Zet_Pixel( minuut / 10, 2, 7);  Zet_Pixel( minuut % 10, 7, 7);
}
//------------------------------------------------------------------------------
//  LED Clear display settings of the LED's
//------------------------------------------------------------------------------
void LedsOff(void) 
{ 
 LED.fill(0, 0, NUM_LEDS );
}

//------------------------------------------------------------------------------
// DS3231 Get time from DS3231
//------------------------------------------------------------------------------
void GetTijd(byte printit)
{
 Inow    = RTCklok.now();
 I.Hour   = _min(Inow.hour()  , 24);
 I.Minute = _min(Inow.minute(), 59);
 I.Second = _min(Inow.second(), 59);
 I.Day    = Inow.day();
 I.Month  = Inow.month();
 I.Year   = Inow.year()-2000;
 I.Wday   = Inow.dayOfTheWeek();
// if (I.Hour > 24) { I.Hour = random(12)+1; I.Minute = random(60)+1; I.Second = 30;}          // Set a time if time module is absent or defect
 if (printit)  Print_RTC_tijd(); 
}
//------------------------------------------------------------------------------
// DS3231 utility function prints time to serial
//------------------------------------------------------------------------------
void Print_RTC_tijd(void)
{
 char sptext[40];
 sprintf(sptext,"%02d:%02d:%02d %02d-%02d-%04d",                                              
     Inow.hour(),Inow.minute(),Inow.second(),Inow.day(),Inow.month(),Inow.year());
 Serial.println(sptext);
}
