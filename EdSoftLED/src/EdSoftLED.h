#ifndef EdSoftLED_H_
#define EdSoftLED_H_

#include <stdint.h>
#include <Arduino.h>

#define SK6812WRGB 10
#define SK6812WGRB 11
#define WS2812RGB  20
#define WS2812GRB  21

struct RGBW {  // Store of the original values           
	uint8_t g; // Green 
	uint8_t r; // Red
	uint8_t b; // Blue
	uint8_t w; // White
};


class EdSoftLED 
{ 
 public: 
	EdSoftLED(uint16_t num_leds, const uint8_t pin , uint8_t LEDtype);
	EdSoftLED(void);
	~EdSoftLED();
	
	void     begin(void);
	void     fill(RGBW RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);	
	void     fill(uint32_t RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);
	void     setBrightness(uint8_t Bright);
    void     setPixelColor(uint16_t i, RGBW RGBWColor);	
    void     setPixelColor(uint16_t i, uint32_t RGBWColor);
    void     setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t w); 
	void     show(void);
	void     showSK6812(void);
	void     showWS2812(void);	
    uint8_t  getWhite(uint32_t c);
    uint8_t  getRed(  uint32_t c);
    uint8_t  getGreen(uint32_t c);
    uint8_t  getBlue( uint32_t c);	
    uint32_t getPixelColor(uint16_t index);
    uint32_t makeRGBWcolor( uint32_t Red, uint32_t Green, uint32_t Blue, uint32_t White);	
  static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) 
  {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
  }
  /*!
    @brief   Convert separate red, green, blue and white values into a
             single "packed" 32-bit WRGB color.
    @param   r  Red brightness, 0 to 255.
    @param   g  Green brightness, 0 to 255.
    @param   b  Blue brightness, 0 to 255.
    @param   w  White brightness, 0 to 255.
    @return  32-bit packed WRGB value, which can then be assigned to a
             variable for later use or passed to the setPixelColor()
             function. Packed WRGB format is predictable, regardless of
             LED strand color order.
  */
  static uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w) 
  {
    return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
  }	

 private:

	uint8_t  _pin_mask;
	uint16_t _count_led;
	uint16_t  Brightness     = 128;
	uint16_t  PreviousBright = 128;
	RGBW     *_pixels;
	RGBW     *_Orgpixels;		
	rmt_data_t *_LEDdata;
    uint8_t  _pin_number ;
	uint8_t  _LEDtype;
};

#endif /* EdSoftLED_H_ */
