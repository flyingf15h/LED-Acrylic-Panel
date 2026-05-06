#include <FastLED.h> 

#define LED1_PIN 5 // left panel 
#define LED2_PIN 6 // right panel
#define SWITCH_PIN1 7 
#define POT_PIN A0 
#define NUM_LEDS 4 
#define BRIGHTNESS 200 

CRGB leds1[NUM_LEDS];  
CRGB leds2[NUM_LEDS];

uint8_t savedLeft = 192; 
uint8_t savedRight = 192;
uint8_t shimmerOffset = 0; 
uint8_t waveOffset = 0;

// include pink, 0-224 for FastLED red to purple and 224-225 for pink 
CRGB extendHue(uint8_t hue) {
  if(hue < 213) {
    uint8_t fastHue = map(hue, 0, 212, 0, 213); 
    return CHSV(fastHue, 150, 255);
  } else {
    uint8_t t = map(hue, 213, 255, 0, 255); 
    CRGB purple = CHSV(192, 110,255);
    CRGB pink = CRGB(255, 130, 200);
    return blend(purple, pink, t);
  }
}

// Switch controls
int readSwitch() {
  bool pin1 = digitalRead(SWITCH_PIN1);
  // left -> 0 / pin1=LOW, rgb 
  // middle -> 1 / pin1=HIGH, left panel  
  // right -> 2 / pin2=LOW, right panel
  bool pin2 = digitalRead(8);
  if(!pin1 && pin2) return 0;
  if(pin1 && pin2) return 1; 
  if(pin1 && !pin2) return 2; 
  return 0;
}

void setup() {
  FastLED.addLeds<WS2812B, LED1_PIN, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED2_PIN, GRB>(leds2, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(SWITCH_PIN1, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
}

void loop() {
  int mode = readSwitch();
  int potVal = analogRead(POT_PIN);

  // rainbow
  if(mode == 0) {
    int waveDelay = map(potVal, 0, 1023, 50, 1);

    for(int i = 0; i < NUM_LEDS; i++) {
      uint8_t hue1 = waveOffset + (i * 32);
      uint8_t hue2 = waveOffset + ((NUM_LEDS + i) * 32);
      leds1[NUM_LEDS - 1 - i] = CHSV(hue1, 140, 255);
      leds2[i] = CHSV(hue2, 140, 255);
    }

    FastLED.show();
    delay(waveDelay); 
    waveOffset++;
  } else if(mode == 1) {
    uint8_t extHue = map(potVal, 0, 1023, 0, 255); 
    savedLeft = extHue; 

    for(int i = 0; i < NUM_LEDS; i++) {
      uint8_t wave = sin8(shimmerOffset + (i*64));
      uint8_t hueShift = (int8_t)map(wave, 0, 255, -12, 12);
      // leds1[i] = CHSV(baseHue + hueShift, 130, 255);
      // leds2[i] = CHSV(savedRight + hueShift, 130, 255);
      CRGB left = extendHue(savedLeft + hueShift);
      CRGB right = extendHue(savedRight + hueShift);
      leds1[i] = left; 
      leds2[i] = right;
    }
    FastLED.show();
    shimmerOffset++;
    delay(20);
  } else if(mode == 2) {
    uint8_t extHue = map(potVal, 0, 1023, 0, 255); 
    savedRight = extHue; 

    for(int i = 0; i < NUM_LEDS; i++) {
      uint8_t wave = sin8(shimmerOffset + (i*64));
      uint8_t hueShift = (int8_t)map(wave, 0, 255, -12, 12);
      // leds1[i] = CHSV(baseHue + hueShift, 130, 255);
      // leds2[i] = CHSV(savedRight + hueShift, 130, 255);
      CRGB left = extendHue(savedLeft + hueShift);
      CRGB right = extendHue(savedRight + hueShift);
      leds1[i] = left; 
      leds2[i] = right;
    }
    FastLED.show();
    shimmerOffset++;
    delay(20);
  }
}
