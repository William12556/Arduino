/*
 * FastLED Test - 12-bit NeoPixel Ring with SOS Blink
 * 
 * Hardware: Adafruit 12-bit NeoPixel Ring (WS2812B)
 * 
 * Compatible Boards:
 * - Arduino Nano ESP32
 * - Adafruit Pro Trinket 5V
 * 
 * Features:
 * - SOS morse code pattern on built-in LED
 * - Random pixel flash with random colors
 * - Hardware validation for WS2812B wiring
 * 
 * ============================================================================
 * WIRING - ARDUINO NANO ESP32
 * ============================================================================
 * NeoPixel DIN -> GPIO 6
 * NeoPixel 5V  -> External 5V supply (NOT USB 5V - insufficient current)
 * NeoPixel GND -> Common ground with ESP32 and power supply
 * 
 * **CRITICAL: ESP32 outputs 3.3V logic**
 * WS2812B requires >3.5V for reliable HIGH recognition at 5V power.
 * Use 74AHCT125N level shifter between GPIO 6 and NeoPixel DIN.
 * 
 * Level Shifter Wiring:
 * - 74AHCT125N VCC -> 5V supply
 * - 74AHCT125N GND -> Common ground
 * - 74AHCT125N 1A  -> ESP32 GPIO 6
 * - 74AHCT125N 1Y  -> NeoPixel DIN
 * - 74AHCT125N 1OE -> GND (always enabled)
 * 
 * ============================================================================
 * WIRING - ADAFRUIT PRO TRINKET 5V
 * ============================================================================
 * NeoPixel DIN -> Pin 6
 * NeoPixel 5V  -> External 5V supply (NOT USB 5V - insufficient current)
 * NeoPixel GND -> Common ground with Pro Trinket and power supply
 * 
 * **ADVANTAGE: Pro Trinket outputs native 5V logic**
 * No level shifter required - direct connection to NeoPixel DIN is reliable.
 * 
 * ============================================================================
 * POWER REQUIREMENTS
 * ============================================================================
 * 12 WS2812B LEDs at full white: ~720mA (60mA per LED)
 * Always use external 5V power supply (2A minimum recommended)
 * Never power from USB - insufficient current capacity
 * 
 * ============================================================================
 */

#include <FastLED.h>

// Hardware configuration - Pin 6 compatible with both boards
#define DATA_PIN       6
#define LED_COUNT      12

// LED array
CRGB leds[LED_COUNT];

// SOS timing constants (milliseconds)
#define DOT_DURATION   200
#define DASH_DURATION  600
#define SYMBOL_GAP     200
#define LETTER_GAP     600
#define WORD_GAP       2000

// State tracking
unsigned long sos_timer = 0;
int sos_state = 0;

void setup() {
  // Initialize built-in LED (works on both boards)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Initialize FastLED
  // GRB color order for WS2812B
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(128);
  
  // Clear all LEDs
  fill_solid(leds, LED_COUNT, CRGB::Black);
  FastLED.show();
  
  // Seed random number generator from analog noise
  // A0 available on both boards
  randomSeed(analogRead(A0));
  
  sos_timer = millis();
}

void loop() {
  // Handle SOS blinking in background (non-blocking)
  update_sos_blink();
  
  // Random pixel flash pattern
  random_pixel_flash();
}

/**
 * Update SOS morse code pattern on built-in LED
 * Pattern: ... --- ... (3 dots, 3 dashes, 3 dots)
 * 
 * Non-blocking state machine allows LED animations to run simultaneously
 * with status indicator on built-in LED.
 * 
 * Timing:
 * - Dot: 200ms on
 * - Dash: 600ms on
 * - Symbol gap: 200ms between dots/dashes
 * - Letter gap: 600ms between S and O
 * - Word gap: 2000ms before repeat
 */
void update_sos_blink() {
  unsigned long current = millis();
  unsigned long elapsed = current - sos_timer;
  
  // SOS state machine
  switch (sos_state) {
    // First S (3 dots)
    case 0: // Dot 1 ON
      if (elapsed >= DOT_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 1: // Dot 1 OFF
      if (elapsed >= SYMBOL_GAP) {
        sos_timer = current;
        sos_state++;
      }
      break;
      
    case 2: // Dot 2 ON
      if (elapsed >= DOT_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 3: // Dot 2 OFF
      if (elapsed >= SYMBOL_GAP) {
        sos_timer = current;
        sos_state++;
      }
      break;
      
    case 4: // Dot 3 ON
      if (elapsed >= DOT_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 5: // Dot 3 OFF (letter gap)
      if (elapsed >= LETTER_GAP) {
        sos_timer = current;
        sos_state++;
      }
      break;
      
    // O (3 dashes)
    case 6: // Dash 1 ON
      if (elapsed >= DASH_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 7: // Dash 1 OFF
      if (elapsed >= SYMBOL_GAP) {
        sos_timer = current;
        sos_state++;
      }
      break;
      
    case 8: // Dash 2 ON
      if (elapsed >= DASH_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 9: // Dash 2 OFF
      if (elapsed >= SYMBOL_GAP) {
        sos_timer = current;
        sos_state++;
      }
      break;
      
    case 10: // Dash 3 ON
      if (elapsed >= DASH_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 11: // Dash 3 OFF (letter gap)
      if (elapsed >= LETTER_GAP) {
        sos_timer = current;
        sos_state++;
      }
      break;
      
    // Second S (3 dots)
    case 12: // Dot 1 ON
      if (elapsed >= DOT_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 13: // Dot 1 OFF
      if (elapsed >= SYMBOL_GAP) {
        sos_timer = current;
        sos_state++;
      }
      break;
      
    case 14: // Dot 2 ON
      if (elapsed >= DOT_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 15: // Dot 2 OFF
      if (elapsed >= SYMBOL_GAP) {
        sos_timer = current;
        sos_state++;
      }
      break;
      
    case 16: // Dot 3 ON
      if (elapsed >= DOT_DURATION) {
        digitalWrite(LED_BUILTIN, LOW);
        sos_timer = current;
        sos_state++;
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }
      break;
      
    case 17: // Dot 3 OFF (word gap)
      if (elapsed >= WORD_GAP) {
        sos_timer = current;
        sos_state = 0; // Reset to start
      }
      break;
  }
}

/**
 * Random pixel flash with random color
 * 
 * Mimics NeoPixel_Random.ino behavior:
 * - Selects random pixel (0-11)
 * - Generates random RGB color (0-128 range for moderate brightness)
 * - Flashes pixel on for 100ms
 * - Turns pixel off
 * - Repeats continuously
 * 
 * Validates:
 * - Data line communication to WS2812B ring
 * - Color order (GRB) correctness
 * - Individual pixel addressing
 * - Power supply adequacy
 */
void random_pixel_flash() {
  // Pick random pixel (0-11 for 12-bit ring)
  int pixel = random(0, LED_COUNT);
  
  // Generate random RGB values (0-128 range for moderate brightness)
  // Prevents power supply overload during testing
  uint8_t r = random(0, 128);
  uint8_t g = random(0, 128);
  uint8_t b = random(0, 128);
  
  // Turn random pixel on with random color
  leds[pixel] = CRGB(r, g, b);
  FastLED.show();
  delay(100);
  
  // Turn pixel off
  leds[pixel] = CRGB::Black;
  FastLED.show();
}
