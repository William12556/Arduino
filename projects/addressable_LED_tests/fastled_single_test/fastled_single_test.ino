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
 * Selected automatically by conditional compilation.
 *
 *   Board                    | DATA_PIN | HEARTBEAT_PIN | Notes
 *   -------------------------|----------|---------------|----------------------
 *   Pro Trinket 5V           |    6     |      13       | 5V logic, no shifter
 *   ItsyBitsy 5V 32u4        |    6     |  LED_BUILTIN  | 5V logic, no shifter
 *   Arduino Nano             |    6     |  LED_BUILTIN  | 5V logic, no shifter
 *   Arduino Nano ESP32       | GPIO17   |     D6        | 3.3V logic, see WARNING
 *
 * NOTE: GPIO RESTRICTIONS ON NANO ESP32
 * ---------------------------------------
 * GPIO6  - connected to QSPI flash, must not be used.
 * GPIO2  - FastLED hangs on ESP32-S3, must not be used.
 * GPIO17 - confirmed working with FastLED on Nano ESP32 (board label D8).
 * GPIO9  - confirmed working as plain GPIO output (board label D6, heartbeat).
 * NOTE: FastLED requires raw GPIO numbers on ESP32-S3 under core 3.3.7 remap mode.
 *        Board label symbols (e.g. D8) resolve to logical pin numbers which FastLED
 *        passes directly to the RMT peripheral. The RMT peripheral expects raw GPIO
 *        numbers, causing silent failure. Use raw GPIO numbers for DATA_PIN.
 *
 * WARNING: ARDUINO NANO ESP32 LOGIC LEVEL
 * ----------------------------------------
 * The Nano ESP32 outputs 3.3V logic. WS2812B/SK6812 require >3.5V for
 * reliable logic HIGH recognition when powered at 5V.
 * A 74AHCT125N level shifter between D8 and LED DIN is required for
 * reliable operation.
 *
 *   Level Shifter Wiring (74AHCT125N):
 *   - VCC  -> 5V supply
 *   - GND  -> Common ground
 *   - 1A   -> D8  (WS2812E data)
 *   - 1Y   -> WS2812E DIN
 *   - 1OE  -> GND (always enabled)
 *   - 2A   -> D6  (heartbeat regular LED)
 *   - 2Y   -> Regular LED anode (heartbeat output)
 *   - 2OE  -> GND (always enabled)
 *
 *     ┌──────────┐
 * 1OE │ 1     14 │ VCC
 * 1A  │ 2     13 │ 4OE
 * 1Y  │ 3     12 │ 4A
 * 2OE │ 4     11 │ 4Y
 * 2A  │ 5     10 │ 3OE
 * 2Y  │ 6      9 │ 3A
 * GND │ 7      8 │ 3Y
 *     └──────────┘
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
 * FastLED >= 3.6.0
 * Install via: Arduino IDE -> Sketch -> Include Library -> Manage Libraries -> FastLED
 *
 * ============================================================================
 *
 * VERSION HISTORY
 * ---------------
 * v1.0  2026-02-22  Initial release. Multi-board, multi-LED-type single LED
 *                   validation sketch. Double-flash heartbeat.
 *                   Diagnostic Serial output in showColor().
 * v1.1  2026-02-22  DATA_PIN set to 6. Confirmed working on Pro Trinket 5V.
 *                   Header updated with Nano ESP32 GPIO restrictions.
 *                   Comment corrected to match DATA_PIN value.
 * v1.2  2026-02-22  Added board/pin selection table in configuration block.
 * v1.3  2026-02-25  Conditional compilation added for DATA_PIN and HEARTBEAT_PIN.
 *                   Nano ESP32: DATA_PIN D8 (ch1), HEARTBEAT_PIN D5 (ch2).
 * v1.4  2026-02-26  HEARTBEAT_PIN changed from D5 to D6. D5 conflicts with D8 under
 *                   core 3.3.7 remap (both resolve to GPIO8).
 *                   DATA_PIN changed from D8 to raw GPIO17. FastLED requires raw GPIO
 *                   numbers on ESP32-S3 under core 3.3.7 remap mode; board label
 *                   symbols cause silent RMT failure.
 *                   Pro Trinket: DATA_PIN 6, HEARTBEAT_PIN 13.
 *                   BRIGHTNESS reduced to 32. Level shifter wiring updated.
 *                   Aligned with neopixel_single_test v1.4.
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

// WS2812E data pin.
// NOTE: FastLED addLeds<> requires a compile-time integer literal.
//       Board label symbols (e.g. D8) resolve to integer constants at
//       compile time and satisfy this requirement.
// Nano ESP32: GPIO17 (board label D8) via 74AHCT125N level shifter channel 1 (1A->1Y->DIN).
//             Raw GPIO number required - board label symbol D8 causes silent RMT failure.
// Pro Trinket 5V: pin 6 direct (5V logic, no shifter required).
#if defined(ARDUINO_NANO_ESP32)
  #define DATA_PIN      9   // GPIO9 = board label D6, level shifter channel 1
  #define DATA_PIN_2    17  // GPIO17 = board label D8, level shifter channel 2
#elif defined(ADAFRUIT_TRINKET_5V)
  #define DATA_PIN      6   // Direct connection, no level shifter
  #define DATA_PIN_2    7   // Second channel
#else
  #define DATA_PIN      6   // Default for AVR boards
  #define DATA_PIN_2    7
#endif

// Heartbeat output pin. Mirrors LED_BUILTIN. Double-flash pattern.
// Nano ESP32: D6 (GPIO9) via level shifter channel 2 (2A->2Y->external LED).
//             Board label symbol D6 works for plain GPIO (no RMT involved).
// Pro Trinket 5V: pin 13 (LED_BUILTIN, onboard LED).
#if defined(ARDUINO_NANO_ESP32)
  #define HEARTBEAT_PIN D6  // Board label D6, level shifter channel 2
#elif defined(ADAFRUIT_TRINKET_5V)
  #define HEARTBEAT_PIN 13  // LED_BUILTIN on Pro Trinket
#else
  #define HEARTBEAT_PIN LED_BUILTIN
#endif

// Number of LEDs under test
#define LED_COUNT       8

// Brightness (0-255). 32 = ~12%. Reduce if supply is marginal.
#define BRIGHTNESS      32

// ============================================================================
// LED Array
// ============================================================================

CRGB leds[LED_COUNT];
CRGB leds2[LED_COUNT];

// ============================================================================
// Setup
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(HEARTBEAT_PIN, OUTPUT);

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

  Serial.print(F("Data Pin 1: "));
  Serial.println(DATA_PIN);
  Serial.print(F("Data Pin 2: "));
  Serial.println(DATA_PIN_2);
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
  FastLED.addLeds<LED_TYPE, DATA_PIN_2, COLOR_ORDER>(leds2, LED_COUNT);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

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
  static uint8_t  step       = 0;
  static unsigned long timer = 0;
  unsigned long now = millis();

  if (now - timer >= HB[step].duration) {
    step  = (step + 1) % HB_STEPS;
    timer = now;
    digitalWrite(LED_BUILTIN, HB[step].state);
    digitalWrite(HEARTBEAT_PIN, HB[step].state);
  }
}

// ============================================================================
// Loop
// ============================================================================

void loop() {
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
  Serial.print(label);
  Serial.print(F(" R=")); Serial.print(color.r);
  Serial.print(F(" G=")); Serial.print(color.g);
  Serial.print(F(" B=")); Serial.println(color.b);
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = color;
    leds2[i] = color;
  }
  Serial.println(F("Calling FastLED.show()..."));
  FastLED.show();
  Serial.println(F("FastLED.show() returned."));
  // Non-blocking 2 second wait - heartbeat runs during hold
  unsigned long start = millis();
  while (millis() - start < 2000) {
    updateHeartbeat();
  }
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
  // Non-blocking 2 second wait - heartbeat runs during hold
  unsigned long start = millis();
  while (millis() - start < 2000) {
    updateHeartbeat();
  }

  // Clear white channel
  raw[3] = 0;
  FastLED.show();
}
