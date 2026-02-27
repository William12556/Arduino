/*
 * FastLED Test - GRB Color Order
 * 
 * Tests WS2812B LED rings with GRB color order configuration.
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
 * - On-board LED toggles every 500ms (operational status indicator)
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
  
  // Initialize FastLED for both rings with GRB color order
  FastLED.addLeds<WS2812B, PIN_PORT, GRB>(leds_port, LED_COUNT);
  FastLED.addLeds<WS2812B, PIN_STARBOARD, GRB>(leds_starboard, LED_COUNT);
  
  // Set brightness to 50% to reduce power consumption and eye strain
  FastLED.setBrightness(128);
}

void loop() {
  // Heartbeat indicator: simple periodic flash
  // Flash every 500ms to indicate sketch is running
  static unsigned long last_heartbeat = 0;
  unsigned long current_time = millis();
  
  if (current_time - last_heartbeat >= 500) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // Toggle LED
    last_heartbeat = current_time;
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
