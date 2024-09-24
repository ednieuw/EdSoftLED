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
	
	void     fill(RGBW RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);	
	void     fill(uint32_t RGBWColor, uint16_t FirstLed, uint16_t NoofLEDs);
	void     setBrightness(uint8_t Bright);
    void     setPixelColor(uint16_t i, RGBW RGBWColor);	
    void     setPixelColor(uint16_t i, uint32_t RGBWColor);
	void     show(void);
	void     showSK6812(void);
	void     showWS2812(void);	
    uint8_t  getWhite(uint32_t c);
    uint8_t  getRed(  uint32_t c);
    uint8_t  getGreen(uint32_t c);
    uint8_t  getBlue( uint32_t c);	
    uint32_t getPixelColor(uint16_t index);
    uint32_t makeRGBWcolor( uint32_t Red, uint32_t Green, uint32_t Blue, uint32_t White);	

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
