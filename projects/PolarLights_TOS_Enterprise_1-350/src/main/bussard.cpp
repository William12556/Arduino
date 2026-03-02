/*
 * bussard.cpp — Bussard Collector Effect Implementation
 *
 * Implements dual counter-rotating WS2812 LED ring animations for the
 * PolarLights 1/350 TOS Enterprise Bussard collector nacelle end caps.
 *
 * Animation layers:
 *   Base wave  — red/green fade cycling, counter-rotating between rings
 *   Flash overlay — probabilistic blue or green bursts, independently fading
 *
 * Author:  William Watson
 * Target:  PolarLights 1/350 TOS Enterprise
 * License: MIT
 */

#include "bussard.h"

// ─── Module State ─────────────────────────────────────────────────────────────

static CRGB leds_port[LED_COUNT];
static CRGB leds_starboard[LED_COUNT];
static LEDState state_port[LED_COUNT];
static LEDState state_starboard[LED_COUNT];

static unsigned long last_rotation  = 0;
static unsigned long last_fade_step = 0;
static unsigned long last_flash_step = 0;

// ─── Private Functions ────────────────────────────────────────────────────────

/*
 * initialize_ring()
 * Seeds LED state array with randomised starting values.
 */
static void initialize_ring(LEDState* ring) {
  for (int i = 0; i < LED_COUNT; i++) {
    ring[i].base_red        = 255;
    ring[i].base_green      = random(1, 34);
    ring[i].base_blue       = 0;
    ring[i].fade_dir        = random(0, 2);
    ring[i].flash_red       = 0;
    ring[i].flash_green     = 0;
    ring[i].flash_blue      = 0;
    ring[i].flash_fade_dir  = 1;
    ring[i].flashing        = false;
    ring[i].flash_type      = 0;
    ring[i].flash_end_time  = millis() - random(0, BUSSARD_FLASH_COOLDOWN);
  }
}

/*
 * rotate_colors_ccw()
 * Shifts base wave state one position counter-clockwise through the ring.
 * Flash overlay state is not moved.
 */
static void rotate_colors_ccw(LEDState* ring) {
  uint8_t t_red   = ring[0].base_red;
  uint8_t t_green = ring[0].base_green;
  uint8_t t_blue  = ring[0].base_blue;
  uint8_t t_dir   = ring[0].fade_dir;
  for (int i = 0; i < LED_COUNT - 1; i++) {
    ring[i].base_red   = ring[i + 1].base_red;
    ring[i].base_green = ring[i + 1].base_green;
    ring[i].base_blue  = ring[i + 1].base_blue;
    ring[i].fade_dir   = ring[i + 1].fade_dir;
  }
  ring[LED_COUNT - 1].base_red   = t_red;
  ring[LED_COUNT - 1].base_green = t_green;
  ring[LED_COUNT - 1].base_blue  = t_blue;
  ring[LED_COUNT - 1].fade_dir   = t_dir;
}

/*
 * rotate_colors_cw()
 * Shifts base wave state one position clockwise through the ring.
 * Flash overlay state is not moved.
 */
static void rotate_colors_cw(LEDState* ring) {
  uint8_t t_red   = ring[LED_COUNT - 1].base_red;
  uint8_t t_green = ring[LED_COUNT - 1].base_green;
  uint8_t t_blue  = ring[LED_COUNT - 1].base_blue;
  uint8_t t_dir   = ring[LED_COUNT - 1].fade_dir;
  for (int i = LED_COUNT - 1; i > 0; i--) {
    ring[i].base_red   = ring[i - 1].base_red;
    ring[i].base_green = ring[i - 1].base_green;
    ring[i].base_blue  = ring[i - 1].base_blue;
    ring[i].fade_dir   = ring[i - 1].fade_dir;
  }
  ring[0].base_red   = t_red;
  ring[0].base_green = t_green;
  ring[0].base_blue  = t_blue;
  ring[0].fade_dir   = t_dir;
}

/*
 * update_fade()
 * Advances the base wave fade cycle for one LED.
 * Red is inversely proportional to green: full red at green=0.
 */
static void update_fade(LEDState* state) {
  if (state->base_green <= 50 && state->fade_dir == 1) {
    state->base_green++;
    if (state->base_green >= 50) state->fade_dir = 0;
  } else if (state->base_green >= 1 && state->fade_dir == 0) {
    state->base_green--;
    if (state->base_green <= 1) state->fade_dir = 1;
  }
  state->base_red  = 255 - (state->base_green * 2);
  state->base_blue = 0;
}

/*
 * count_active_flashes()
 * Returns total number of LEDs currently in a flash state across both rings.
 */
static int count_active_flashes() {
  int count = 0;
  for (int i = 0; i < LED_COUNT; i++) {
    if (state_port[i].flashing)      count++;
    if (state_starboard[i].flashing) count++;
  }
  return count;
}

/*
 * check_flash_trigger()
 * Evaluates flash trigger conditions for one LED.
 * Skips if: adjacent LED is flashing, global flash limit reached, or cooldown active.
 */
static void check_flash_trigger(LEDState* ring, int index) {
  int prev = (index - 1 + LED_COUNT) % LED_COUNT;
  int next = (index + 1) % LED_COUNT;
  unsigned long now = millis();

  if (!ring[index].flashing
      && !ring[prev].flashing
      && !ring[next].flashing
      && count_active_flashes() < BUSSARD_MAX_FLASHES
      && (now - ring[index].flash_end_time >= BUSSARD_FLASH_COOLDOWN)) {

    if (random(0, 100) < BUSSARD_FLASH_PROB) {
      int prob = random(0, 100);

#ifdef BUSSARD_BLUE_FLASH_ENABLED
      if (prob < BUSSARD_PROB_BLUE) {
        ring[index].flash_type      = 1;
        ring[index].flashing        = true;
        ring[index].flash_blue      = 1;
        ring[index].flash_red       = 0;
        ring[index].flash_green     = 0;
        ring[index].flash_fade_dir  = 1;
      } else if (prob < (BUSSARD_PROB_BLUE + BUSSARD_PROB_PURE_GREEN)) {
#else
      if (prob < (BUSSARD_PROB_BLUE + BUSSARD_PROB_PURE_GREEN)) {
#endif
        ring[index].flash_type      = 2;
        ring[index].flashing        = true;
        ring[index].flash_green     = 1;
        ring[index].flash_red       = 0;
        ring[index].flash_blue      = 0;
        ring[index].flash_fade_dir  = 1;
      }
    }
  }
}

/*
 * update_flash()
 * Advances the flash overlay fade cycle for one LED.
 * Clears flash state and records end time when fade completes.
 */
static void update_flash(LEDState* state) {
  if (!state->flashing) return;

  if (state->flash_type == 1) {
    // Blue flash
    if (state->flash_blue <= BUSSARD_FLASH_PEAK && state->flash_fade_dir == 1) {
      state->flash_blue++;
      if (state->flash_blue >= BUSSARD_FLASH_PEAK) state->flash_fade_dir = 0;
    } else if (state->flash_blue >= 1 && state->flash_fade_dir == 0) {
      state->flash_blue--;
      if (state->flash_blue <= 1) {
        state->flashing       = false;
        state->flash_type     = 0;
        state->flash_blue     = 0;
        state->flash_end_time = millis();
      }
    }
  } else if (state->flash_type == 2) {
    // Green flash
    if (state->flash_green <= BUSSARD_FLASH_PEAK && state->flash_fade_dir == 1) {
      state->flash_green++;
      if (state->flash_green >= BUSSARD_FLASH_PEAK) state->flash_fade_dir = 0;
    } else if (state->flash_green >= 1 && state->flash_fade_dir == 0) {
      state->flash_green--;
      if (state->flash_green <= 1) {
        state->flashing       = false;
        state->flash_type     = 0;
        state->flash_green    = 0;
        state->flash_end_time = millis();
      }
    }
  }
}

/*
 * apply_colors()
 * Writes flash overlay to FastLED array when active; base wave otherwise.
 */
static void apply_colors(LEDState* states, CRGB* leds) {
  for (int i = 0; i < LED_COUNT; i++) {
    if (states[i].flashing) {
      leds[i] = CRGB(states[i].flash_red, states[i].flash_green, states[i].flash_blue);
    } else {
      leds[i] = CRGB(states[i].base_red, states[i].base_green, states[i].base_blue);
    }
  }
}

// ─── Public Interface ─────────────────────────────────────────────────────────

/*
 * bussard_setup()
 * Registers FastLED strips, initialises ring state, runs startup LED test.
 */
void bussard_setup() {
  FastLED.addLeds<LED_TYPE, PIN_PORT, COLOR_ORDER>(leds_port, LED_COUNT);
  FastLED.addLeds<LED_TYPE, PIN_STARBOARD, COLOR_ORDER>(leds_starboard, LED_COUNT);
  FastLED.setBrightness(BRIGHTNESS);

  initialize_ring(state_port);
  initialize_ring(state_starboard);

  Serial.println("Bussard: Initialised");
  Serial.println("Bussard: Port=CCW | Starboard=CW");
  Serial.print  ("Bussard: Rotation delay=");
  Serial.print  (BUSSARD_ROTATION_DELAY);
  Serial.println("ms");

  // Startup LED test — all LEDs dim red for ~3s then ramp to full
  Serial.println("Bussard: LED test...");
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
    delay(4); // ~223 steps × 4ms ≈ 1s
  }
  Serial.println("Bussard: Ready");
}

/*
 * bussard_loop()
 * Advances rotation, fade, and flash state machines. Writes to FastLED arrays.
 * Must be called every iteration of loop().
 */
void bussard_loop() {
  unsigned long now = millis();

  // Rotation
  if (now - last_rotation >= BUSSARD_ROTATION_DELAY) {
    last_rotation = now;
    rotate_colors_ccw(state_port);
    rotate_colors_cw(state_starboard);
  }

  // Fade and flash steps
  bool do_fade_step  = (now - last_fade_step  >= BUSSARD_FADE_STEP_DELAY);
  bool do_flash_step = (now - last_flash_step >= BUSSARD_FLASH_STEP_DELAY);

  if (do_fade_step)  last_fade_step  = now;
  if (do_flash_step) last_flash_step = now;

  for (int i = 0; i < LED_COUNT; i++) {
    if (do_fade_step) {
      update_fade(&state_port[i]);
      update_fade(&state_starboard[i]);
    }
    if (do_flash_step) {
      check_flash_trigger(state_port,      i);
      check_flash_trigger(state_starboard, i);
      update_flash(&state_port[i]);
      update_flash(&state_starboard[i]);
    }
  }

  apply_colors(state_port,      leds_port);
  apply_colors(state_starboard, leds_starboard);
  FastLED.show();
}
