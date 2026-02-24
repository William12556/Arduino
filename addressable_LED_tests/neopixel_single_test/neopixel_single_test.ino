/* Created: 2026 February 22
 * ============================================================================
 * neopixel_single_test.ino
 * Single Addressable LED Hardware Validation Sketch (Adafruit NeoPixel)
 * ============================================================================
 *
 * PURPOSE
 * -------
 * Validates wiring, power, and NeoPixel configuration for a single addressable
 * LED. Companion sketch to fastled_single_test.ino for boards where FastLED
 * has known compatibility issues (e.g. Arduino Nano ESP32 / ESP32-S3).
 *
 * SUPPORTED BOARDS
 * ----------------
 * - Adafruit Pro Trinket 5V / 16MHz
 * - Adafruit ItsyBitsy 5V / 32u4 / 16MHz
 * - Arduino Nano (ATmega328P)
 * - Arduino Nano ESP32
 *
 * SUPPORTED LED TYPES
 * -------------------
 * - WS2812B   (NEO_GRB color order)
 * - WS2812E   (NEO_GRB color order, same protocol as WS2812B)
 * - SK6812RGBW (NEO_GRBW color order)
 *
 * CONFIGURATION
 * -------------
 * Set LED_TYPE below to match your LED:
 *
 *   LED Type    | LED_TYPE
 *   ------------|-----------------------------
 *   WS2812B     | NEO_GRB + NEO_KHZ800
 *   WS2812E     | NEO_GRB + NEO_KHZ800
 *   SK6812RGBW  | NEO_GRBW + NEO_KHZ800
 *
 * DATA PIN
 * --------
 * Pin 6 is the default for AVR boards. Pin 6 (D6) for Nano ESP32.
 *
 *   Board                    | DATA_PIN | Notes
 *   -------------------------|----------|----------------------------------
 *   Pro Trinket 5V           |    6     | 5V logic, no shifter needed
 *   ItsyBitsy 5V 32u4        |    6     | 5V logic, no shifter needed
 *   Arduino Nano             |    6     | 5V logic, no shifter needed
 *   Arduino Nano ESP32       |    6     | 3.3V logic - see WARNING
 *
 * NOTE: GPIO RESTRICTIONS ON NANO ESP32
 * ---------------------------------------
 * GPIO6  - connected to QSPI flash, must not be used.
 * GPIO2  - FastLED hangs on ESP32-S3. NeoPixel untested.
 * GPIO8  - confirmed working with FastLED on Nano ESP32.
 * NeoPixel uses ESP32 RMT driver directly and is more tolerant than FastLED.
 * Test with DATA_PIN 6 first. If no result, try DATA_PIN 8.
 *
 * WARNING: ARDUINO NANO ESP32 LOGIC LEVEL
 * ----------------------------------------
 * The Nano ESP32 outputs 3.3V logic. WS2812B/SK6812 require >3.5V for
 * reliable logic HIGH recognition when powered at 5V.
 * A 74AHCT125N level shifter between DATA_PIN and LED DIN is recommended.
 *
 *   Level Shifter Wiring (74AHCT125N):
 *   - VCC  -> 5V supply
 *   - GND  -> Common ground
 *   - 3A   -> Nano ESP32 DATA_PIN
 *   - 3Y   -> LED DIN
 *   - 3OE  -> GND (always enabled)
 *
 * WIRING (ALL BOARDS)
 * --------------------
 * LED DIN -> DATA_PIN (via 74AHCT125N for Nano ESP32)
 * LED 5V  -> External 5V supply
 * LED GND -> Common ground with board and supply
 *
 * POWER
 * -----
 * A single WS2812B/SK6812 at full white draws ~60mA.
 * Use an external 5V supply. Do not power from USB alone during testing.
 * Include a 100-470 ohm series resistor on the data line.
 * Include a 100uF decoupling capacitor across 5V/GND near the LED.
 *
 * OPERATION
 * ---------
 * On power-up:
 *   1. Serial output (115200 baud) reports board identity and pin mapping.
 *   2. Built-in LED blinks 5 times (startup confirmation).
 *   3. Color cycle begins: RED -> GREEN -> BLUE -> WHITE -> OFF, 2s each.
 *      SK6812RGBW adds a WARM WHITE step using the white channel only.
 *   4. Built-in LED double-flashes as a heartbeat indicator.
 *      Pattern: ON 100ms, OFF 250ms, ON 100ms, OFF 1000ms, repeat.
 *
 * ARDUINO IDE BOARD SELECTION
 * ----------------------------
 * Pro Trinket 5V:    Tools -> Board -> Adafruit AVR Boards -> Pro Trinket 5V/16MHz (USB)
 * ItsyBitsy 32u4:    Tools -> Board -> Adafruit AVR Boards -> Adafruit ItsyBitsy 32u4 5V 16MHz
 * Arduino Nano:      Tools -> Board -> Arduino AVR Boards -> Arduino Nano
 * Arduino Nano ESP32: Tools -> Board -> Arduino ESP32 Boards -> Arduino Nano ESP32
 *
 * LIBRARY DEPENDENCY
 * ------------------
 * Adafruit NeoPixel >= 1.10.0
 * Install via: Arduino IDE -> Sketch -> Include Library -> Manage Libraries -> Adafruit NeoPixel
 *
 * ============================================================================
 *
 * VERSION HISTORY
 * ---------------
 * v1.0  2026-02-22  Initial release. Based on fastled_single_test v1.2.
 *                   Uses Adafruit NeoPixel library for ESP32-S3 compatibility.
 *                   Same heartbeat pattern and colour cycle as FastLED variant.
 * ============================================================================
 */

#include <Adafruit_NeoPixel.h>

// ============================================================================
// CONFIGURATION - Edit these defines to match your hardware
// ============================================================================

// LED type and color order.
// WS2812B / WS2812E:  NEO_GRB + NEO_KHZ800
// SK6812RGBW:         NEO_GRBW + NEO_KHZ800
#define LED_TYPE        (NEO_GRB + NEO_KHZ800)

// Set to 1 if using SK6812RGBW (enables white channel test)
#define IS_RGBW         0

// Data pin - see board/pin table in header above
#define DATA_PIN        8   // <-- Change this when switching boards

// Number of LEDs under test
#define LED_COUNT       1

// Brightness (0-255). 128 = 50%. Reduce if supply is marginal.
#define BRIGHTNESS      128

// ============================================================================
// NeoPixel instance
// ============================================================================

Adafruit_NeoPixel strip(LED_COUNT, DATA_PIN, LED_TYPE);

// ============================================================================
// Setup
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);

  // Report board identity and configuration
  Serial.println(F("\n=== neopixel_single_test ==="));

  #if defined(ARDUINO_NANO_ESP32)
    Serial.println(F("Board   : Arduino Nano ESP32"));
    Serial.println(F("WARNING : 3.3V logic - 74AHCT125N level shifter recommended"));
  #elif defined(ADAFRUIT_ITSYBITSY_32U4_5V)
    Serial.println(F("Board   : Adafruit ItsyBitsy 5V 32u4"));
  #elif defined(__AVR_ATmega328P__)
    Serial.println(F("Board   : Arduino Nano (ATmega328P)"));
  #elif defined(ADAFRUIT_TRINKET_5V)
    Serial.println(F("Board   : Adafruit Pro Trinket 5V"));
  #else
    Serial.println(F("Board   : Unknown"));
  #endif

  Serial.print(F("Data Pin  : "));
  Serial.println(DATA_PIN);
  Serial.print(F("LED Type  : "));
  #if IS_RGBW
    Serial.println(F("SK6812RGBW"));
  #else
    Serial.println(F("WS2812B/WS2812E"));
  #endif
  Serial.print(F("Brightness: "));
  Serial.println(BRIGHTNESS);

  // Startup blink - 5 pulses confirms MCU is running
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  // Initialise NeoPixel
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();

  Serial.println(F("Setup complete. Starting color cycle.\n"));
}

// ============================================================================
// Heartbeat state machine
// Pattern: ON 100ms | OFF 250ms | ON 100ms | OFF 1000ms | repeat
// ============================================================================

// Heartbeat timing table: {pin state, duration ms}
static const struct { uint8_t state; unsigned int duration; } HB[] = {
  { HIGH, 100  },   // Flash 1 on
  { LOW,  250  },   // Gap
  { HIGH, 100  },   // Flash 2 on
  { LOW,  1000 }    // Pause
};
#define HB_STEPS (sizeof(HB) / sizeof(HB[0]))

/**
 * Non-blocking heartbeat update.
 * Call as frequently as possible from loop() and blocking wait loops.
 * Advances through the double-flash pattern using millis() timing.
 */
void updateHeartbeat() {
  static uint8_t       step  = 0;
  static unsigned long timer = 0;
  unsigned long now = millis();

  if (now - timer >= HB[step].duration) {
    step  = (step + 1) % HB_STEPS;
    timer = now;
    digitalWrite(LED_BUILTIN, HB[step].state);
  }
}

// ============================================================================
// Loop
// ============================================================================

void loop() {
  // Color cycle
  showColor("RED",   255, 0, 0);
  showColor("GREEN", 0, 255, 0);
  showColor("BLUE",  0, 0, 255);
  showColor("WHITE", 255, 255, 255);

  #if IS_RGBW
    showRGBW();
  #endif

  showColor("OFF", 0, 0, 0);
}

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Display a solid colour on LED[0] for 2 seconds.
 * Non-blocking wait allows heartbeat to run during hold period.
 *
 * @param label  Colour name printed to Serial.
 * @param r      Red channel (0-255).
 * @param g      Green channel (0-255).
 * @param b      Blue channel (0-255).
 */
void showColor(const char* label, uint8_t r, uint8_t g, uint8_t b) {
  Serial.print(label);
  Serial.print(F(" R=")); Serial.print(r);
  Serial.print(F(" G=")); Serial.print(g);
  Serial.print(F(" B=")); Serial.println(b);

  strip.setPixelColor(0, strip.Color(r, g, b));
  Serial.println(F("Calling strip.show()..."));
  strip.show();
  Serial.println(F("strip.show() returned."));

  unsigned long start = millis();
  while (millis() - start < 2000) {
    updateHeartbeat();
  }
}

/**
 * SK6812RGBW white channel test.
 * Uses the fourth (white) channel of the SK6812RGBW LED directly.
 * RGB channels are set to zero, white channel set to BRIGHTNESS.
 */
void showRGBW() {
  Serial.println(F("WHITE CHANNEL (W only)"));

  // strip.Color() with four arguments sets RGBW
  strip.setPixelColor(0, strip.Color(0, 0, 0, BRIGHTNESS));
  strip.show();

  unsigned long start = millis();
  while (millis() - start < 2000) {
    updateHeartbeat();
  }

  strip.clear();
  strip.show();
}
