/*
 * fastled_diagnostic.ino
 *
 * Created: 2026 February 26
 *
 * Purpose:
 *   GPIO diagnostic for DATA_PIN and HEARTBEAT_PIN on the Arduino Nano ESP32.
 *   No LED libraries are used. Tests pure digitalWrite() drive capability on
 *   both pins via the 74AHCT125N level shifter.
 *
 * Hardware:
 *   Arduino Nano ESP32 (ESP32-S3, 3.3V logic)
 *   74AHCT125N level shifter (5V VCC)
 *     Channel 1: D8 (1A) -> 1Y -> WS2812E DIN
 *     Channel 2: D5 (2A) -> 2Y -> Heartbeat LED
 *     1OE, 2OE -> GND
 *
 * Pin Assignments:
 *   Board                    | DATA_PIN | HEARTBEAT_PIN | Notes
 *   -------------------------|----------|---------------|---------------------------
 *   Arduino Nano ESP32       | D8       | D5            | Via 74AHCT125N level shifter
 *   Adafruit Pro Trinket 5V  | 6        | 13            | Direct connection
 *   Other AVR                | 6        | LED_BUILTIN   | Direct connection
 *
 * Cycle:
 *   Both pins driven HIGH for CYCLE_HIGH_MS, then LOW for CYCLE_LOW_MS.
 *   Repeats indefinitely. Serial reports each state change.
 *
 * Board Selection:
 *   Arduino Nano ESP32: Tools -> Board -> esp32 by Espressif Systems -> Arduino Nano ESP32
 *   Pro Trinket 5V:     Tools -> Board -> Adafruit AVR Boards -> Pro Trinket 5V/16MHz (USB)
 *
 * Version History:
 *   v1.0  2026-02-26  Initial release.
 *   v1.1  2026-02-26  DATA_PIN changed from D8 to raw GPIO17. FastLED/NeoPixel
 *                     require raw GPIO numbers on ESP32-S3 under core 3.3.7 remap
 *                     mode; board label symbols cause silent RMT failure.
 *                     HEARTBEAT_PIN comment updated to reflect GPIO9 = D6.
 *
 * Copyright (c) 2025 William Watson. This work is licensed under the MIT License.
 */

// ---------------------------------------------------------------------------
// CONFIGURATION
// ---------------------------------------------------------------------------

#define CYCLE_HIGH_MS  5000   // Duration pins are driven HIGH (ms)
#define CYCLE_LOW_MS   5000   // Duration pins are driven LOW (ms)

// Pin assignments - conditional per board
#if defined(ARDUINO_NANO_ESP32)
  #define DATA_PIN       17   // GPIO17 = board label D8, level shifter channel 1
  #define HEARTBEAT_PIN  D6   // Board label D6 = GPIO9, level shifter channel 2
#elif defined(ADAFRUIT_TRINKET_5V)
  #define DATA_PIN       6    // Direct connection, no level shifter
  #define HEARTBEAT_PIN  13   // LED_BUILTIN on Pro Trinket
#else
  #define DATA_PIN       6    // Default for AVR boards
  #define HEARTBEAT_PIN  LED_BUILTIN
#endif

// ---------------------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println(F("=== fastled_diagnostic ==="));

  // Report board identity
  #if defined(ARDUINO_NANO_ESP32)
    Serial.println(F("Board         : Arduino Nano ESP32"));
    Serial.println(F("WARNING       : 3.3V logic - 74AHCT125N level shifter required"));
  #elif defined(ADAFRUIT_TRINKET_5V)
    Serial.println(F("Board         : Adafruit Pro Trinket 5V"));
  #else
    Serial.println(F("Board         : Unknown AVR"));
  #endif

  // Report resolved GPIO numbers
  Serial.print(F("DATA_PIN      : ")); Serial.println(DATA_PIN);
  Serial.print(F("HEARTBEAT_PIN : ")); Serial.println(HEARTBEAT_PIN);
  Serial.print(F("Cycle HIGH    : ")); Serial.print(CYCLE_HIGH_MS); Serial.println(F(" ms"));
  Serial.print(F("Cycle LOW     : ")); Serial.print(CYCLE_LOW_MS);  Serial.println(F(" ms"));
  Serial.println();

  pinMode(DATA_PIN, OUTPUT);
  pinMode(HEARTBEAT_PIN, OUTPUT);

  digitalWrite(DATA_PIN, LOW);
  digitalWrite(HEARTBEAT_PIN, LOW);

  Serial.println(F("Setup complete. Starting GPIO cycle."));
  Serial.println(F("Measure voltage on D8 and D5 header pins and level shifter"));
  Serial.println(F("inputs (1A, 2A) and outputs (1Y, 2Y) during each phase."));
  Serial.println();
}

// ---------------------------------------------------------------------------
// LOOP
// ---------------------------------------------------------------------------

void loop() {
  // Drive both pins HIGH
  digitalWrite(DATA_PIN, HIGH);
  digitalWrite(HEARTBEAT_PIN, HIGH);
  Serial.print(F("HIGH - measure now ("));
  Serial.print(CYCLE_HIGH_MS);
  Serial.println(F(" ms)"));
  delay(CYCLE_HIGH_MS);

  // Drive both pins LOW
  digitalWrite(DATA_PIN, LOW);
  digitalWrite(HEARTBEAT_PIN, LOW);
  Serial.print(F("LOW  - measure now ("));
  Serial.print(CYCLE_LOW_MS);
  Serial.println(F(" ms)"));
  delay(CYCLE_LOW_MS);
}
