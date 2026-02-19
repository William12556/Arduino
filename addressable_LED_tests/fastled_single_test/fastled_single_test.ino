/*
 * ============================================================================
 * fastled_single_test.ino
 * Single Addressable LED Hardware Validation Sketch
 * ============================================================================
 *
 * PURPOSE
 * -------
 * Validates wiring, power, and FastLED configuration for a single addressable
 * LED. Designed to isolate hardware issues before deploying multi-LED sketches.
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
 * - WS2812B   (GRB color order)
 * - WS2812E   (GRB color order, same protocol as WS2812B)
 * - SK6812RGBW (RGBW color order - see NOTE below)
 *
 * CONFIGURATION
 * -------------
 * Set LED_TYPE and COLOR_ORDER defines below to match your LED:
 *
 *   LED Type    | #define LED_TYPE | #define COLOR_ORDER
 *   ------------|------------------|--------------------
 *   WS2812B     | WS2812B          | GRB
 *   WS2812E     | WS2812B          | GRB
 *   SK6812RGBW  | SK6812           | RGB
 *
 * NOTE: SK6812RGBW white channel
 *   FastLED does not expose a native .w field on CRGB structs.
 *   White channel is written via raw byte offset. This sketch demonstrates
 *   the technique. See showRGBW() function for details.
 *
 * DATA PIN
 * --------
 * Pin 6 is used across all supported boards.
 *
 *   Board                    | Physical Pin | Notes
 *   -------------------------|--------------|----------------------------
 *   Pro Trinket 5V           | Pin 6        | 5V logic, no shifter needed
 *   ItsyBitsy 5V 32u4        | Pin 6        | 5V logic, no shifter needed
 *   Arduino Nano             | Pin 6        | 5V logic, no shifter needed
 *   Arduino Nano ESP32       | Pin 6 (D6)   | 3.3V logic - see WARNING
 *
 * WARNING: ARDUINO NANO ESP32 LOGIC LEVEL
 * ----------------------------------------
 * The Nano ESP32 outputs 3.3V logic. WS2812B/SK6812 require >3.5V for
 * reliable logic HIGH recognition when powered at 5V.
 * A 74AHCT125N level shifter between Pin 6 and LED DIN is required for
 * reliable operation.
 *
 *   Level Shifter Wiring (74AHCT125N):
 *   - VCC  -> 5V supply
 *   - GND  -> Common ground
 *   - 1A   -> Nano ESP32 Pin 6
 *   - 1Y   -> LED DIN
 *   - 1OE  -> GND (always enabled)
 *
 * WIRING (ALL BOARDS)
 * --------------------
 * LED DIN -> Pin 6 (via 74AHCT125N for Nano ESP32)
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
 *   4. Built-in LED toggles every 500ms as a heartbeat indicator.
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
 * FastLED >= 3.6.0
 * Install via: Arduino IDE -> Sketch -> Include Library -> Manage Libraries -> FastLED
 *
 * ============================================================================
 */

#include <FastLED.h>

// ============================================================================
// CONFIGURATION - Edit these defines to match your hardware
// ============================================================================

// LED type: WS2812B, SK6812 (see table in header above)
#define LED_TYPE        WS2812B

// Color order: GRB for WS2812B/WS2812E, RGB for SK6812RGBW
#define COLOR_ORDER     GRB

// Set to 1 if using SK6812RGBW (enables white channel test)
#define IS_RGBW         0

// Data pin (Pin 6 on all supported boards)
#define DATA_PIN        6

// Number of LEDs under test
#define LED_COUNT       1

// Brightness (0-255). 128 = 50%. Reduce if supply is marginal.
#define BRIGHTNESS      128

// ============================================================================
// LED Array
// ============================================================================

CRGB leds[LED_COUNT];

// ============================================================================
// Setup
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);

  // Report board identity and configuration
  Serial.println(F("\n=== fastled_single_test ==="));

  #if defined(ARDUINO_NANO_ESP32)
    Serial.println(F("Board   : Arduino Nano ESP32"));
    Serial.println(F("WARNING : 3.3V logic - 74AHCT125N level shifter required"));
  #elif defined(ADAFRUIT_ITSYBITSY_32U4_5V)
    Serial.println(F("Board   : Adafruit ItsyBitsy 5V 32u4"));
  #elif defined(__AVR_ATmega328P__)
    Serial.println(F("Board   : Arduino Nano (ATmega328P)"));
  #elif defined(ADAFRUIT_TRINKET_5V)
    Serial.println(F("Board   : Adafruit Pro Trinket 5V"));
  #else
    Serial.println(F("Board   : Unknown"));
  #endif

  Serial.print(F("Data Pin: "));
  Serial.println(DATA_PIN);
  Serial.print(F("LED Type: "));
  Serial.println(F(""
    #if IS_RGBW
    "SK6812RGBW"
    #else
    "WS2812B/WS2812E"
    #endif
  ));
  Serial.print(F("Brightness: "));
  Serial.println(BRIGHTNESS);

  // Startup blink - 5 pulses confirms MCU is running
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  // Initialise FastLED
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, LED_COUNT);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  Serial.println(F("Setup complete. Starting color cycle.\n"));
}

// ============================================================================
// Loop
// ============================================================================

void loop() {
  static unsigned long last_heartbeat = 0;
  unsigned long now = millis();

  // Heartbeat: built-in LED toggles every 500ms
  if (now - last_heartbeat >= 500) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    last_heartbeat = now;
  }

  // Color cycle
  showColor("RED",   CRGB(255, 0, 0));
  showColor("GREEN", CRGB(0, 255, 0));
  showColor("BLUE",  CRGB(0, 0, 255));
  showColor("WHITE", CRGB(255, 255, 255));

  #if IS_RGBW
    // SK6812RGBW: test dedicated white channel
    showRGBW();
  #endif

  showColor("OFF", CRGB(0, 0, 0));
}

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Display a solid color on LED[0] for 2 seconds.
 *
 * @param label  Color name printed to Serial.
 * @param color  CRGB color value.
 */
void showColor(const char* label, CRGB color) {
  Serial.println(label);
  leds[0] = color;
  FastLED.show();
  delay(2000);
}

/**
 * SK6812RGBW white channel test.
 *
 * FastLED does not expose a .w field on CRGB. The white channel byte
 * occupies the byte immediately following the B byte in memory for
 * SK6812 pixels. This function writes directly to that byte.
 *
 * Raw layout for SK6812 in FastLED: [R][G][B][W]
 * The W byte is at offset +3 from the LED array base for LED[0].
 *
 * NOTE: This is an undocumented implementation detail of FastLED.
 * Verify correct behaviour with your FastLED version if white does
 * not illuminate as expected.
 */
void showRGBW() {
  Serial.println(F("WHITE CHANNEL (W only)"));

  // Clear RGB channels
  leds[0] = CRGB(0, 0, 0);

  // Write white channel via raw byte
  uint8_t* raw = (uint8_t*)leds;
  raw[3] = BRIGHTNESS;   // W byte at offset 3 for LED[0]

  FastLED.show();
  delay(2000);

  // Clear white channel
  raw[3] = 0;
  FastLED.show();
}
