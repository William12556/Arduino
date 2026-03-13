/*
 * bussard2.h — Bussard Collector Effect v2 Interface
 *
 * Declares the public interface for the Bussard collector lighting subsystem,
 * effect variant 2: independent per-LED pulsing from dim yellow through orange
 * to bright red and back, with probabilistic green flash overlays.
 *
 * Author:  William Watson
 * Target:  PolarLights 1/350 TOS Enterprise
 * License: MIT
 */

#ifndef BUSSARD2_H
#define BUSSARD2_H

#include "config.h"

// ─── Animation Parameters ─────────────────────────────────────────────────────

#define BUSSARD2_PHASE_STEP_DELAY   30    // ms between phase advance ticks
#define BUSSARD2_FLASH_DURATION     50    // ms green flash is held on
#define BUSSARD2_FLASH_PROB         15    // % chance of green flash at phase wrap

// ─── Public Interface ─────────────────────────────────────────────────────────

/*
 * bussard2_setup()
 * Registers FastLED strips, seeds per-LED state, and runs startup LED test.
 * Call once from setup().
 */
void bussard2_setup();

/*
 * bussard2_loop()
 * Advances all per-LED pulse and flash state machines.
 * Call every iteration from loop().
 */
void bussard2_loop();

#endif // BUSSARD2_H
