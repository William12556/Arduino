/*
 * config.h — Shared Hardware Configuration and Types
 *
 * Central configuration header for the PolarLights 1/350 TOS Enterprise
 * lighting controller. Contains hardware constants, animation parameters,
 * and shared data structures used across all subsystem modules.
 *
 * Author:  William Watson
 * Target:  PolarLights 1/350 TOS Enterprise
 * License: MIT
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>

// ─── Hardware Configuration ───────────────────────────────────────────────────

#define LED_COUNT   8
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define BRIGHTNESS  255

// Bussard collector data pins
#define PIN_PORT       9    // GPIO9  = board label D6
#define PIN_STARBOARD  17   // GPIO17 = board label D8

// Blue flash enable — comment out to disable blue flashes
#define BUSSARD_BLUE_FLASH_ENABLED

// Built-in LED
#ifndef LED_BUILTIN
#define LED_BUILTIN LED_BUILTIN
#endif

// ─── LED State Structure ──────────────────────────────────────────────────────

/*
 * LEDState
 *
 * Tracks both the base wave layer and flash overlay layer for a single LED.
 * apply_colors() selects which layer to render based on the flashing flag.
 */
struct LEDState {
  // Base wave layer
  uint8_t base_red;
  uint8_t base_green;
  uint8_t base_blue;
  uint8_t fade_dir;           // 0=fade down, 1=fade up

  // Flash overlay layer
  uint8_t flash_red;
  uint8_t flash_green;
  uint8_t flash_blue;
  uint8_t flash_fade_dir;     // 0=fade down, 1=fade up
  bool    flashing;           // true during active flash
  uint8_t flash_type;         // 0=none, 1=blue, 2=green
  unsigned long flash_end_time; // millis() timestamp when last flash completed
};

#endif // CONFIG_H
