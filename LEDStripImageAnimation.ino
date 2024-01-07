/// @file    LEDStripImageAnimation.ino
/// @brief   Displays colors from image array with Arduino Uno on WS2812B LED strip
/// @example LEDStripImageAnimation.ino

#include <FastLED.h>
#include <avr/pgmspace.h>
#include "pool.h"

#define DATA_PIN      2
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB
#define NUM_LEDS      300
#define NUM_FRAMES    40
#define IMAGE_WIDTH   150
#define BRIGHTNESS    20     //96
#define FPS           20     //120

CRGB leds[NUM_LEDS];

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}
  
void loop(){
  for(int frame = 0; frame < NUM_FRAMES; frame++){
    for(int i = 0; i < IMAGE_WIDTH; i++){
        leds[i] = pgm_read_dword(&(pool[(frame * IMAGE_WIDTH) + i]));
        leds[i + IMAGE_WIDTH] = pgm_read_dword(&(pool[(frame * IMAGE_WIDTH) + i]));
    }
    FastLED.show();
    FastLED.delay(1000 / FPS);
  }
}
