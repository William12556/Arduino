/*
 * FastLED Test - RGB Color Order
 * 
 * Test WS2812 rings with RGB color order.
 * Cycles: Red -> Green -> Blue -> White -> Off
 */

#include <FastLED.h>

#define PIN_PORT        2
#define PIN_STARBOARD   3
#define LED_COUNT       8

CRGB leds_port[LED_COUNT];
CRGB leds_starboard[LED_COUNT];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  // 5 fast blinks on startup
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  
  FastLED.addLeds<WS2812, PIN_PORT, RGB>(leds_port, LED_COUNT);
  FastLED.addLeds<WS2812, PIN_STARBOARD, RGB>(leds_starboard, LED_COUNT);
  FastLED.setBrightness(128);
}

void loop() {
  // Heartbeat blink
  static unsigned long last_beat = 0;
  static bool beat_state = false;
  if (millis() - last_beat >= 500) {
    last_beat = millis();
    beat_state = !beat_state;
    digitalWrite(LED_BUILTIN, beat_state);
  }
  
  // RED
  fill_solid(leds_port, LED_COUNT, CRGB::Red);
  fill_solid(leds_starboard, LED_COUNT, CRGB::Red);
  FastLED.show();
  delay(1000);
  
  // GREEN
  fill_solid(leds_port, LED_COUNT, CRGB::Green);
  fill_solid(leds_starboard, LED_COUNT, CRGB::Green);
  FastLED.show();
  delay(1000);
  
  // BLUE
  fill_solid(leds_port, LED_COUNT, CRGB::Blue);
  fill_solid(leds_starboard, LED_COUNT, CRGB::Blue);
  FastLED.show();
  delay(1000);
  
  // WHITE
  fill_solid(leds_port, LED_COUNT, CRGB::White);
  fill_solid(leds_starboard, LED_COUNT, CRGB::White);
  FastLED.show();
  delay(1000);
  
  // OFF
  fill_solid(leds_port, LED_COUNT, CRGB::Black);
  fill_solid(leds_starboard, LED_COUNT, CRGB::Black);
  FastLED.show();
  delay(1000);
}
