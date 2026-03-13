/*
 * main.ino — Central Lighting Controller
 *
 * Coordinator for all onboard lighting and sound effects on the
 * PolarLights 1/350 scale Star Trek TOS Enterprise model kit.
 *
 * Subsystems (current):
 *   - Bussard collectors (port/starboard nacelle end caps)
 *
 * Subsystems (planned):
 *   - Impulse engines
 *   - Navigation lights
 *   - Deflector dish
 *   - Sound effects
 *
 * Each subsystem is implemented in its own .h/.cpp module.
 * This file contains only setup() and loop() coordination.
 *
 * Hardware:
 *   - Arduino Nano ESP32
 *   - 2x AZDelivery RGB LED Ring 8 Bit WS2812 5050 (Bussard, port/starboard)
 *
 * Author:  William Watson
 * Target:  PolarLights 1/350 TOS Enterprise
 * License: MIT
 */

#include "config.h"
// #include "bussard.h"    // Effect 1: counter-rotating wave + random flash
#include "bussard2.h"      // Effect 2: per-LED pulse (yellow/orange/red) + green flash

// ─── Heartbeat ────────────────────────────────────────────────────────────────

static unsigned long last_heartbeat  = 0;
static bool          heartbeat_state = false;

// ─── Setup ───────────────────────────────────────────────────────────────────

void setup() {
  delay(100);

  Serial.begin(115200);
  delay(100);

  randomSeed(analogRead(A0));

  Serial.println("PolarLights TOS Enterprise — Central Controller");

  // bussard_setup();    // Effect 1
  bussard2_setup();      // Effect 2 — called first to clear LEDs before startup blink

  pinMode(LED_BUILTIN, OUTPUT);

  // Startup blink — 3 fast blinks to confirm firmware is running
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH); delay(100);
    digitalWrite(LED_BUILTIN, LOW);  delay(100);
  }

  // Add additional subsystem setup calls here:
  // navlights_setup();
  // impulse_setup();

  Serial.println("Controller: Ready");
}

// ─── Loop ────────────────────────────────────────────────────────────────────

void loop() {
  // bussard_loop();    // Effect 1
  bussard2_loop();      // Effect 2

  // Add additional subsystem loop calls here:
  // navlights_loop();
  // impulse_loop();

  // Heartbeat — 1Hz blink on built-in LED
  unsigned long now = millis();
  if (now - last_heartbeat >= 1000) {
    last_heartbeat  = now;
    heartbeat_state = !heartbeat_state;
    digitalWrite(LED_BUILTIN, heartbeat_state ? HIGH : LOW);
  }
}
