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
 *   The flash fades up to a peak then back to zero before normal cycling resumes.
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
  uint8_t       phase;          // current position in pulse cycle (0–255)
  uint8_t       phase_step;     // phase advance per tick (1–3, randomised at init)
  bool          flashing;       // true during active green flash
  uint8_t       flash_green;    // current green channel value during flash
  uint8_t       flash_dir;      // 0 = fading down, 1 = fading up
};

static CRGB    leds_port[LED_COUNT];
static CRGB    leds_starboard[LED_COUNT];
static B2State state_port[LED_COUNT];
static B2State state_starboard[LED_COUNT];

static unsigned long last_phase_step = 0;
static unsigned long last_flash_step = 0;

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
    ring[i].flashing    = false;
    ring[i].flash_green = 0;
    ring[i].flash_dir   = 1;
  }
}

/*
 * advance_phase()
 * Advances phase for one LED. Checks for phase wrap and triggers green flash.
 */
static void advance_phase(B2State* state) {
  if (state->flashing) return;  // hold phase during flash

  uint16_t next = (uint16_t)state->phase + state->phase_step;

  if (next >= 256) {
    // Phase wrap — evaluate flash trigger
    state->phase = (uint8_t)(next - 256);
    if (random(0, 100) < BUSSARD2_FLASH_PROB) {
      state->flashing    = true;
      state->flash_green = 0;
      state->flash_dir   = 1;
    }
  } else {
    state->phase = (uint8_t)next;
  }
}

/*
 * update_flash()
 * Advances the green flash fade for one LED.
 * Clears flash state when fade-down completes.
 */
static void update_flash(B2State* state) {
  if (!state->flashing) return;

  if (state->flash_dir == 1) {
    // Fading up
    if (state->flash_green < BUSSARD2_FLASH_PEAK) {
      state->flash_green++;
    } else {
      state->flash_dir = 0;
    }
  } else {
    // Fading down
    if (state->flash_green > 0) {
      state->flash_green--;
    } else {
      state->flashing    = false;
      state->flash_green = 0;
    }
  }
}

/*
 * apply_colors()
 * Writes computed colour to FastLED array.
 * Renders green flash overlay when active; base pulse colour otherwise.
 */
static void apply_colors(B2State* states, CRGB* leds) {
  for (int i = 0; i < LED_COUNT; i++) {
    if (states[i].flashing) {
      leds[i] = CRGB(0, states[i].flash_green, 0);
    } else {
      leds[i] = phase_to_color(states[i].phase);
    }
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

  initialize_ring(state_port);
  initialize_ring(state_starboard);

  Serial.println("Bussard2: Initialised");
  Serial.println("Bussard2: Per-LED pulse mode");

  // Startup LED test — all LEDs dim red for ~3s then ramp to full
  Serial.println("Bussard2: LED test...");
  for (int i = 0; i < LED_COUNT; i++) {
    leds_port[i]      = CRGB(32, 0, 0);
    leds_starboard[i] = CRGB(32, 0, 0);
  }
  FastLED.show();
  delay(3200);

  for (int b = 32; b <= 255; b++) {
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
  bool do_flash_step = (now - last_flash_step >= BUSSARD2_FLASH_STEP_DELAY);

  if (do_phase_step) last_phase_step = now;
  if (do_flash_step) last_flash_step = now;

  for (int i = 0; i < LED_COUNT; i++) {
    if (do_phase_step) {
      advance_phase(&state_port[i]);
      advance_phase(&state_starboard[i]);
    }
    if (do_flash_step) {
      update_flash(&state_port[i]);
      update_flash(&state_starboard[i]);
    }
  }

  apply_colors(state_port,      leds_port);
  apply_colors(state_starboard, leds_starboard);
  FastLED.show();
}
