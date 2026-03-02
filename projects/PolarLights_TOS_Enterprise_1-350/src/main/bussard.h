/*
 * bussard.h — Bussard Collector Effect Interface
 *
 * Declares the public interface for the Bussard collector lighting subsystem.
 * Manages dual 8-LED WS2812 rings with counter-rotating base wave animations
 * and probabilistic flash overlays.
 *
 * Author:  William Watson
 * Target:  PolarLights 1/350 TOS Enterprise
 * License: MIT
 */

#ifndef BUSSARD_H
#define BUSSARD_H

#include "config.h"

// ─── Animation Parameters ─────────────────────────────────────────────────────

#define BUSSARD_ROTATION_DELAY  300   // ms between rotation steps (~25 RPM)
#define BUSSARD_FADE_STEP_DELAY  20   // ms between base wave fade steps
#define BUSSARD_FLASH_STEP_DELAY  1   // ms between flash fade steps
#define BUSSARD_FLASH_PEAK       25   // flash channel peak value
#define BUSSARD_FLASH_COOLDOWN  2500  // ms an LED must wait before retriggering
#define BUSSARD_FLASH_PROB        4   // % chance per LED per flash step
#define BUSSARD_MAX_FLASHES       3   // maximum simultaneous active flashes

// Flash type probabilities (must sum ≤ 100; remainder = base rotation colour)
#define BUSSARD_PROB_BLUE        10   // % chance of blue flash
#define BUSSARD_PROB_PURE_GREEN   5   // % chance of pure green flash

// ─── Public Interface ─────────────────────────────────────────────────────────

/*
 * bussard_setup()
 * Registers FastLED strips, seeds LED state, and runs startup LED test.
 * Call once from setup().
 */
void bussard_setup();

/*
 * bussard_loop()
 * Advances all Bussard animation state machines (rotation, fade, flash).
 * Call every iteration from loop().
 */
void bussard_loop();

#endif // BUSSARD_H
