Created: 2026 February 26

# KB: ESP32-S3 FastLED RMT Silent Failure Under Core 3.3.7 Pin Remap

## Table of Contents

- [Summary](<#summary>)
- [Environment](<#environment>)
- [Symptoms](<#symptoms>)
- [Root Cause](<#root-cause>)
  - [Pin Remap Mode](<#pin-remap-mode>)
  - [RMT Peripheral Requirement](<#rmt-peripheral-requirement>)
  - [Pin Conflict](<#pin-conflict>)
- [Diagnosis](<#diagnosis>)
- [Resolution](<#resolution>)
- [Verified Pin Assignments](<#verified-pin-assignments>)
- [Affected Sketches](<#affected-sketches>)
- [Version History](<#version-history>)

---

## Summary

On the Arduino Nano ESP32 running Espressif ESP32 core 3.3.7, FastLED and Adafruit NeoPixel both fail silently when a board label symbol (e.g. `D8`) is used as the WS2812 data pin. The `show()` call returns without error but no signal is produced on the pin. The fix is to use the raw GPIO number (`17`) instead of the board label symbol (`D8`).

A secondary issue was identified: board label `D5` was non-functional in testing; root cause not isolated. `D6` (GPIO9) was adopted as the working heartbeat pin.

[Return to Table of Contents](<#table-of-contents>)

---

## Environment

| Item | Detail |
|---|---|
| Board | Arduino Nano ESP32 (ESP32-S3) |
| Core | Espressif esp32 3.3.7 (manually installed) |
| Arduino IDE | 2.x |
| Libraries | FastLED, Adafruit NeoPixel |
| LED | WS2812E (single) |
| Level Shifter | 74AHCT125N |
| Host | macOS (Apple Silicon M4) |
| Pin Numbering | Tools → Pin Numbering → By Arduino Pin (default) |

[Return to Table of Contents](<#table-of-contents>)

---

## Symptoms

- `FastLED.show()` and `strip.show()` return without error.
- Serial output confirms the sketch is running and cycling through colours.
- Data pin (D8 / 1A on level shifter) measures constant 3.3V — no RMT signal.
- Level shifter output (1Y) measures constant 5V — confirming the static input.
- WS2812E does not respond.
- Heartbeat pin (D5) measures 0V constantly — pin not driving at all.
- `Serial.println(DATA_PIN)` reports `8` (logical pin number, not GPIO number).

[Return to Table of Contents](<#table-of-contents>)

---

## Root Cause

Three distinct issues combined to produce the failure.

### Pin Remap Mode

The Arduino Nano ESP32 variant defines two sets of pin constants in `pins_arduino.h`, selected at compile time by the `BOARD_HAS_PIN_REMAP` preprocessor flag:

```
// Arduino style (remap active - default)
D5 = 5   (logical)   → physical GPIO8
D8 = 8   (logical)   → physical GPIO17

// ESP32 GPIO style (remap inactive)
D5 = 8   (GPIO)
D8 = 17  (GPIO)
```

The `boards.txt` entry for `nano_nora` defines:

```
nano_nora.build.defines=-DBOARD_HAS_PIN_REMAP ...
```

Remap mode is active by default. Board label symbols resolve to logical pin numbers (0–24), not raw GPIO numbers.

### RMT Peripheral Requirement

FastLED and Adafruit NeoPixel both use the ESP32-S3 RMT peripheral to generate WS2812 timing. The RMT driver is initialised with the pin number passed to `addLeds<>()` or the `Adafruit_NeoPixel` constructor. The RMT peripheral expects a **raw GPIO number**.

When `D8` is used, it resolves to logical pin `8`. The RMT driver interprets this as GPIO8, which is not the physical pin connected to the level shifter. The RMT peripheral initialises without error but drives the wrong GPIO, producing no visible output on the D8 header pin. No error is reported.

### Pin Conflict

During testing, `HEARTBEAT_PIN` defined as `D5` consistently measured 0V — the pin was not driving. The root cause was not isolated. A pin conflict with D8 was considered but not confirmed by direct testing. A misread of the physical header pin position during voltage measurement is an alternative explanation. `D6` (GPIO9) was adopted as a working replacement. D5 should be avoided until further testing is conducted.

[Return to Table of Contents](<#table-of-contents>)

---

## Diagnosis

Diagnosis proceeded in stages.

**Stage 1 — Confirm GPIO drive capability**

A dedicated diagnostic sketch (`fastled_diagnostic.ino`) was created using only `digitalWrite()` — no LED libraries — cycling both pins HIGH and LOW every 5 seconds. This isolated library behaviour from raw GPIO function.

Result: `DATA_PIN` (D8) cycled correctly. `HEARTBEAT_PIN` (D5) remained at 0V.

**Stage 2 — Resolve the D5 failure**

Voltage measurements on the Nano ESP32 header pins and the level shifter confirmed D5 was not driving. The official Arduino Nano ESP32 full pinout PDF confirmed:

- Header pin D5 → GPIO8
- Header pin D8 → GPIO17

D5 was replaced with D6 (GPIO9). Both pins then cycled correctly in the diagnostic sketch.

**Stage 3 — Identify the RMT failure**

With both pins confirmed driveable, `fastled_single_test.ino` was uploaded. D8 measured constant 3.3V — FastLED was not producing an RMT signal. `Serial.println(DATA_PIN)` printed `8`, confirming FastLED received logical pin 8, not GPIO17.

**Stage 4 — Apply the fix**

`DATA_PIN` was changed from `D8` to the raw integer `17`. After upload, the level shifter input (1A) showed the expected RMT pulse train and the WS2812E responded correctly.

[Return to Table of Contents](<#table-of-contents>)

---

## Resolution

Use raw GPIO numbers for any pin passed to the RMT peripheral (FastLED, NeoPixel data pin). Board label symbols are not suitable for RMT pin assignment on ESP32-S3 under core 3.3.7 remap mode.

Board label symbols remain valid for plain `digitalWrite()` / `digitalRead()` use.

**Before:**

```cpp
#if defined(ARDUINO_NANO_ESP32)
  #define DATA_PIN      D8  // Fails silently - logical pin 8, not GPIO17
  #define HEARTBEAT_PIN D5  // Does not drive - conflicts under remap
#endif
```

**After:**

```cpp
#if defined(ARDUINO_NANO_ESP32)
  #define DATA_PIN      17  // GPIO17 = board label D8
  #define HEARTBEAT_PIN D6  // GPIO9 = board label D6, plain GPIO only
#endif
```

[Return to Table of Contents](<#table-of-contents>)

---

## Verified Pin Assignments

Arduino Nano ESP32, Espressif core 3.3.7, remap mode active.

| Board Label | GPIO | Function | Notes |
|---|---|---|---|
| D8 | GPIO17 | WS2812E data → level shifter 1A | Use raw `17` for RMT. Confirmed. |
| D6 | GPIO9 | WS2812E data → level shifter 1A | Use raw `9` for RMT. Confirmed. |
| D5 | GPIO8 | — | Conflicts with D8 mapping; do not use alongside D8 |

Level shifter wiring (74AHCT125N):

```
VCC  → 5V
GND  → GND
1OE  → GND
2OE  → GND
1A   → Nano D8  (GPIO17, raw)
1Y   → WS2812E DIN
2A   → Nano D6  (GPIO9)
2Y   → Heartbeat LED anode
```

[Return to Table of Contents](<#table-of-contents>)

---

## Affected Sketches

| Sketch | Change |
|---|---|
| `projects/addressable_LED_tests/fastled_single_test/fastled_single_test.ino` | v1.4 — DATA_PIN → 17, HEARTBEAT_PIN → D6 |
| `projects/addressable_LED_tests/fastled_single_test/fastled_single_test.ino` | v1.5 — GPIO9 (D6) confirmed for RMT. Dual ring test: DATA_PIN → 9, DATA_PIN_2 → 17, LED_COUNT → 8 |
| `projects/addressable_LED_tests/neopixel_single_test/neopixel_single_test.ino` | v1.5 — DATA_PIN → 17, HEARTBEAT_PIN → D6 |
| `projects/addressable_LED_tests/fastled_diagnostic/fastled_diagnostic.ino` | v1.1 — DATA_PIN → 17, HEARTBEAT_PIN comment corrected |

[Return to Table of Contents](<#table-of-contents>)

---

## Version History

| Version | Date | Notes |
|---|---|---|
| 1.0 | 2026-02-26 | Initial release. |
| 1.1 | 2026-02-27 | GPIO9 (D6) confirmed working for FastLED RMT. Dual ring validation completed. Verified pin table updated. |

---

Copyright (c) 2025 William Watson. This work is licensed under the MIT License.
