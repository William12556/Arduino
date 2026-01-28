/*
 * FastLED Test - RGB Color Order
 * 
 * Tests WS2812B LED rings with RGB color order configuration.
 * Cycles through: Red -> Green -> Blue -> White -> Off
 * 
 * HARDWARE: Arduino Nano ESP32
 * 
 * WIRING DIAGRAM:
 * 
 *   Arduino Nano ESP32      74AHCT125N Level Shifter          WS2812B Rings
 *   ┌──────────────┐        ┌──────────────────┐        ┌──────────────────┐
 *   │              │        │                  │        │  Port  Starboard │
 *   │  5V      ────┼────────┤ VCC (16)         ├────────┤ VCC          VCC │
 *   │              │        │                  │        │                  │
 *   │  GND     ────┼────────┤ GND (8)          ├────────┤ GND          GND │
 *   │              │        │ 1OE (1) ─────────┼── GND  │                  │
 *   │              │        │ 2OE (4) ─────────┼── GND  │                  │
 *   │              │        │                  │        │                  │
 *   │  D2 (GPIO2)──┼────────┤ 1A (2)           │        │                  │
 *   │              │        │                  │        │                  │
 *   │  D3 (GPIO3)──┼────────┤ 2A (5)           │        │                  │
 *   │              │        │                  │        │                  │
 *   └──────────────┘        │  1Y (3) ─────────┼────────┤ DIN              │
 *                           │                  │        │                  │
 *                           │  2Y (6) ─────────┼────────┤              DIN │
 *                           │                  │        │                  │
 *                           └──────────────────┘        └──────────────────┘
 * 
 * CONNECTIONS:
 *   OE pins (1OE, 2OE) tied together to GND to enable outputs
 * 
 * 74AHCT125N PINOUT (DIP-14):
 *   1OE(1)  1A(2)  1Y(3)  2OE(4)  2A(5)  2Y(6)  GND(8)
 *   VCC(16)                                            (pins 7,9-15 unused)
 * 
 * NOTE: Level shifter required for reliable 5V WS2812B operation with 3.3V ESP32 logic.
 *       Tie both OE pins to GND to enable outputs.
 * 
 * UPLOAD INSTRUCTIONS:
 * 1. Connect Arduino Nano ESP32 via USB
 * 2. Tools -> Board -> Arduino ESP32 Boards -> Arduino Nano ESP32
 * 3. Tools -> Port -> Select appropriate COM/tty port
 * 4. Click Upload button
 * 
 * EXPECTED BEHAVIOR:
 * - 5 fast blinks on startup (startup confirmation)
 * - Double heartbeat pattern on built-in LED (operational status)
 * - Both rings cycle through colors in sync: R -> G -> B -> W -> Off
 * - Each color displays for 1 second
 */

#include <FastLED.h>

// Pin definitions - use D-prefix for clarity on Nano ESP32
#define PIN_PORT        D2      // GPIO2 - Port ring data line
#define PIN_STARBOARD   D3      // GPIO3 - Starboard ring data line
#define LED_COUNT       8       // 8 LEDs per ring

// LED array buffers
CRGB leds_port[LED_COUNT];
CRGB leds_starboard[LED_COUNT];

void setup() {
  // Configure built-in LED for heartbeat indicator
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Startup confirmation: 5 fast blinks
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  
  // Initialize FastLED for both rings with RGB color order
  FastLED.addLeds<WS2812B, PIN_PORT, RGB>(leds_port, LED_COUNT);
  FastLED.addLeds<WS2812B, PIN_STARBOARD, RGB>(leds_starboard, LED_COUNT);
  
  // Set brightness to 50% to reduce power consumption and eye strain
  FastLED.setBrightness(128);
}

void loop() {
  // Heartbeat indicator: double blink pattern
  // Pattern: blink-pause-blink-long_pause (repeats every 2.4 seconds)
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
    pattern_start = millis();         // Reset pattern after 2.4 seconds
  }
  
  // RED - Test red channel
  fill_solid(leds_port, LED_COUNT, CRGB::Red);
  fill_solid(leds_starboard, LED_COUNT, CRGB::Red);
  FastLED.show();
  delay(1000);
  
  // GREEN - Test green channel
  fill_solid(leds_port, LED_COUNT, CRGB::Green);
  fill_solid(leds_starboard, LED_COUNT, CRGB::Green);
  FastLED.show();
  delay(1000);
  
  // BLUE - Test blue channel
  fill_solid(leds_port, LED_COUNT, CRGB::Blue);
  fill_solid(leds_starboard, LED_COUNT, CRGB::Blue);
  FastLED.show();
  delay(1000);
  
  // WHITE - Test all channels simultaneously
  fill_solid(leds_port, LED_COUNT, CRGB::White);
  fill_solid(leds_starboard, LED_COUNT, CRGB::White);
  FastLED.show();
  delay(1000);
  
  // OFF - Clear all LEDs
  fill_solid(leds_port, LED_COUNT, CRGB::Black);
  fill_solid(leds_starboard, LED_COUNT, CRGB::Black);
  FastLED.show();
  delay(1000);
}
