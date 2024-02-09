/// @file    LEDStripImageAnimation.ino
/// @brief   Displays colors from image array with Arduino Uno on WS2812B LED strip
///          Change settings with LCD keypad shield
/// @example LEDStripImageAnimation.ino

#include <FastLED.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include "fire.h"

#define LED_TYPE        WS2812B
#define COLOR_ORDER     GRB
#define DATA_PIN        2
#define NUM_LEDS        300
#define NUM_FRAMES      40
#define IMAGE_WIDTH     150
#define MAX_BRIGHTNESS  255
#define MAX_FPS         120
#define AMOUNT          10
#define MODE_BRIGHTNESS 0
#define MODE_SPEED      1
#define MODE_SIZE       2
#define BTN_NONE        0
#define BTN_UP          1
#define BTN_DOWN        2
#define BTN_LEFT        3
#define BTN_RIGHT       4
#define BTN_SELECT      5
#define DEBOUNCE_DELAY  50

unsigned long lastDebounceTime = 0,
              lastTick = 0;
int lastButtonValue = 0,
    lastCommand = 0,
    tick = 0,
    mode = 0,          // 0 ... 2
    brightness = 200,  // 0 ... 255 (MAX_BRIGHTNESS)
    fps = 20,          // 0 ... 120 (MAX_FPS)
    wide = 0,          // 0 ... 1
    frame = 0;         // 0 ... 39 (NUM_FRAMES - 1)

CRGB leds[NUM_LEDS];
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // FastLED LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // LiquidCrystal LCD configuration
  lcd.begin(16, 2);

  lastTick = millis();
}
  
void loop(){
  displayMode();
  processInputs();
  if(tick){
    showColorValues();
  }  
  setTick();
}

void showColorValues(){
  if(wide){
    for(int i = 0; i < NUM_LEDS; i = i + 2){
        leds[i] = pgm_read_dword(&(fire[(frame * IMAGE_WIDTH) + (i / 2)]));
        leds[i + 1] = leds[i];
    }
  } else {
    for(int i = 0; i < IMAGE_WIDTH; i++){
        leds[i] = pgm_read_dword(&(fire[(frame * IMAGE_WIDTH) + i]));
        leds[i + IMAGE_WIDTH] = leds[i];
    }
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
  if(frame >= NUM_FRAMES - 1){
    frame = 0;
  } else {
    frame++;
  }  
}

void displayMode(){
  switch(mode){
    case MODE_BRIGHTNESS: {
      lcd.setCursor(0,0);
      lcd.print("Brightness:     ");
      lcd.setCursor(0,1);
      lcd.print(String(brightness) + "        ");
      break;
    }
    case MODE_SPEED: {
      lcd.setCursor(0,0);
      lcd.print("Speed (FPS):    ");
      lcd.setCursor(0,1);
      lcd.print(String(fps) + "        ");
      break;
    }
    case MODE_SIZE: {
      lcd.setCursor(0,0);
      lcd.print("Size:           ");
      lcd.setCursor(0,1);
      if(wide){
        lcd.print("Wide            ");
      } else {
        lcd.print("Successive      ");
      }
      break;
    }
  }
}

void setTick(){
  unsigned long currentTime = millis();
  tick = 0;
  if(currentTime - lastTick >= (1000 / fps)){
    tick = 1;
    lastTick = currentTime;
  }
}

int getButtonValue(){
  int analogValue = analogRead(0);
  if (analogValue > 1000) return BTN_NONE;
  if (analogValue < 50)   return BTN_RIGHT;
  if (analogValue < 250)  return BTN_UP;
  if (analogValue < 450)  return BTN_DOWN;
  if (analogValue < 600)  return BTN_LEFT;
  if (analogValue < 920)  return BTN_SELECT;
  return BTN_NONE;
}

void processInputs(){
  unsigned long currentTime = millis();
  int buttonValue = getButtonValue(),
      command = 0;
  
  // Debouncing
  if(buttonValue != lastButtonValue) lastDebounceTime = currentTime;
  if((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) command = buttonValue;
  
  // Edge detection
  if(command != lastCommand){
    switch(command){
      case BTN_UP: {
        if (mode == MODE_BRIGHTNESS && brightness <= MAX_BRIGHTNESS - AMOUNT) brightness = brightness + AMOUNT;
        if (mode == MODE_SPEED && fps <= MAX_FPS - AMOUNT) fps = fps + AMOUNT;
        if (mode == MODE_SIZE) wide = !wide;
        break;
      }
      case BTN_DOWN: {
        if (mode == MODE_BRIGHTNESS && brightness > 0) brightness = brightness - AMOUNT;
        if (mode == MODE_SPEED && fps > 0) fps = fps - AMOUNT;
        if (mode == MODE_SIZE) wide = !wide;
        break;
      }
      case BTN_RIGHT: {
        if (mode >= 2) mode = 0;
        else mode++;
        break;
      }
      case BTN_LEFT: {
        if (mode <= 0) mode = 2;
        else mode--;
        break;
      }
    }
    lastCommand = command;
  }  
  lastButtonValue = buttonValue;
}
