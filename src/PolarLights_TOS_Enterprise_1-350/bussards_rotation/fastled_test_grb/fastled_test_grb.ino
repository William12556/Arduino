/*
 * FastLED Test - GRB Color Order
 * 
 * Test WS2812 rings with GRB color order.
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
  
  FastLED.addLeds<WS2812B, PIN_PORT, GRB>(leds_port, LED_COUNT);
  FastLED.addLeds<WS2812B, PIN_STARBOARD, GRB>(leds_starboard, LED_COUNT);
  FastLED.setBrightness(128);
}

void loop() {
  // Double heartbeat blink pattern: 2 fast blinks, 2 sec pause
  static unsigned long pattern_start = 0;
  unsigned long elapsed = millis() - pattern_start;
  
  if (elapsed < 100) {
    digitalWrite(LED_BUILTIN, HIGH);  // First blink on
  } else if (elapsed < 200) {
    digitalWrite(LED_BUILTIN, LOW);   // First blink off
  } else if (elapsed < 300) {
    digitalWrite(LED_BUILTIN, HIGH);  // Second blink on
  } else if (elapsed < 400) {
    digitalWrite(LED_BUILTIN, LOW);   // Second blink off
  } else if (elapsed >= 2400) {
    pattern_start = millis();         // Reset after 2.4 sec total
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
