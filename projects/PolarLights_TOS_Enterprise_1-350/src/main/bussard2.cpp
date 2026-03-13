/*
 * bussard2.cpp — Bussard Collector Effect v2 Implementation
 *
 * Implements independent per-LED pulsing animation for the PolarLights 1/350
 * TOS Enterprise Bussard collector nacelle end caps.
 *
 * Animation:
 *   Each LED cycles independently through a phase value (0–255).
 *   Phase drives a colour interpolation from dim yellow (phase 0/255)
 *   through orange to bright red (phase 127), then back.
 *   Each LED is seeded at a random phase offset to desynchronise pulses.
 *   A slight per-LED phase step variation (1–3) adds further organic spread.
 *
 *   At each phase wrap (255 → 0), a probabilistic green flash may fire.
 *   The flash is a simple fixed-duration on/off pulse at full green.
 *
 * Colour mapping (phase half = 0 at dim, 127 at peak):
 *   R = 100 + (155 * half / 127)    →  100 .. 255
 *   G =  60 - ( 60 * half / 127)    →   60 ..   0
 *   B = 0
 *
 * Author:  William Watson
 * Target:  PolarLights 1/350 TOS Enterprise
 * License: MIT
 */

#include "bussard2.h"

// ─── Internal State ───────────────────────────────────────────────────────────

/*
 * B2State
 * Per-LED state for the v2 pulse effect.
 */
struct B2State {
  uint8_t       phase;            // current position in pulse cycle (0–255)
  uint8_t       phase_step;       // phase advance per tick (1–3, randomised at init)
  bool          flashing;         // true during active green flash
  unsigned long flash_end_time;   // millis() timestamp when flash expires
};

static CRGB    leds_port[LED_COUNT];
static CRGB    leds_starboard[LED_COUNT];
static B2State state_port[LED_COUNT];
static B2State state_starboard[LED_COUNT];

static unsigned long last_phase_step = 0;

// ─── Private Functions ────────────────────────────────────────────────────────

/*
 * phase_to_color()
 * Maps a phase value (0–255) to a CRGB colour along the dim-yellow → red arc.
 * Phase 0 and 255 are both dim yellow. Phase 127 is bright red.
 */
static CRGB phase_to_color(uint8_t phase) {
  // Fold phase into a 0–127 triangle (0 = dim, 127 = peak)
  uint8_t half = (phase <= 127) ? phase : (255 - phase);

  uint8_t r = (uint8_t)(100 + ((uint16_t)155 * half / 127));
  uint8_t g = (uint8_t)( 60 - ((uint16_t) 60 * half / 127));

  return CRGB(r, g, 0);
}

/*
 * initialize_ring()
 * Seeds per-LED state with randomised phase offsets and step sizes.
 */
static void initialize_ring(B2State* ring) {
  for (int i = 0; i < LED_COUNT; i++) {
    ring[i].phase       = (uint8_t)random(0, 256);
    ring[i].phase_step  = (uint8_t)random(1, 4);   // 1, 2, or 3
    ring[i].flashing      = false;
    ring[i].flash_end_time = 0;
  }
}

/*
 * advance_phase()
 * Advances phase for one LED. Returns true if a phase wrap occurred.
 * Flash triggering is handled separately by check_flash_trigger().
 */
static bool advance_phase(B2State* state) {
  if (state->flashing) return false;  // hold phase during flash

  uint16_t next = (uint16_t)state->phase + state->phase_step;

  if (next >= 256) {
    state->phase = (uint8_t)(next - 256);
    return true;   // phase wrapped — caller may evaluate flash trigger
  } else {
    state->phase = (uint8_t)next;
    return false;
  }
}

/*
 * check_flash_trigger()
 * Evaluates green flash trigger for one LED at phase wrap.
 * Skips if the LED or either neighbour is already flashing (adjacency guard).
 */
static void check_flash_trigger(B2State* ring, int index) {
  int prev = (index - 1 + LED_COUNT) % LED_COUNT;
  int next = (index + 1) % LED_COUNT;

  if (ring[index].flashing) return;
  if (ring[prev].flashing)  return;
  if (ring[next].flashing)  return;

  if (random(0, 100) < BUSSARD2_FLASH_PROB) {
    ring[index].flashing      = true;
    ring[index].flash_end_time = millis() + BUSSARD2_FLASH_DURATION;
  }
}

/*
 * apply_colors()
 * Writes computed colour to FastLED array.
 * Renders green flash overlay when active; base pulse colour otherwise.
 */
static void apply_colors(B2State* states, CRGB* leds) {
  unsigned long now = millis();
  for (int i = 0; i < LED_COUNT; i++) {
    if (states[i].flashing) {
      if (now >= states[i].flash_end_time) {
        states[i].flashing = false;   // flash expired — return to pulse
      } else {
        leds[i] = CRGB(0, 255, 0);
        continue;
      }
    }
    leds[i] = phase_to_color(states[i].phase);
  }
}

// ─── Public Interface ─────────────────────────────────────────────────────────

/*
 * bussard2_setup()
 * Registers FastLED strips, initialises ring state, runs startup LED test.
 */
void bussard2_setup() {
  FastLED.addLeds<LED_TYPE, PIN_PORT,      COLOR_ORDER>(leds_port,      LED_COUNT);
  FastLED.addLeds<LED_TYPE, PIN_STARBOARD, COLOR_ORDER>(leds_starboard, LED_COUNT);
  FastLED.setBrightness(BRIGHTNESS);

  // Ensure LEDs are off before any startup sequence
  FastLED.clear();
  FastLED.show();

  initialize_ring(state_port);
  initialize_ring(state_starboard);

  Serial.println("Bussard2: Initialised");
  Serial.println("Bussard2: Per-LED pulse mode");

  // Startup ramp — LEDs ramp from off to full red over ~1s
  Serial.println("Bussard2: LED test...");

  for (int b = 0; b <= 255; b++) {
    for (int i = 0; i < LED_COUNT; i++) {
      leds_port[i]      = CRGB(b, 0, 0);
      leds_starboard[i] = CRGB(b, 0, 0);
    }
    FastLED.show();
    delay(4);
  }
  Serial.println("Bussard2: Ready");
}

/*
 * bussard2_loop()
 * Advances phase and flash state machines for all LEDs. Writes to FastLED arrays.
 * Must be called every iteration of loop().
 */
void bussard2_loop() {
  unsigned long now = millis();

  bool do_phase_step = (now - last_phase_step >= BUSSARD2_PHASE_STEP_DELAY);

  if (do_phase_step) last_phase_step = now;

  if (do_phase_step) {
    for (int i = 0; i < LED_COUNT; i++) {
      if (advance_phase(&state_port[i]))      check_flash_trigger(state_port,      i);
      if (advance_phase(&state_starboard[i])) check_flash_trigger(state_starboard, i);
    }
  }

  apply_colors(state_port,      leds_port);
  apply_colors(state_starboard, leds_starboard);
  FastLED.show();
}
