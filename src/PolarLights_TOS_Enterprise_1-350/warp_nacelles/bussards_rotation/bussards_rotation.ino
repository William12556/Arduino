/*
 * Bussard Collector Rotation Effect
 * 
 * Counter-rotating color wave animation with random flash overlays for
 * Star Trek TOS Enterprise Bussard collectors. Uses two 8-LED WS2812 rings
 * with continuous rotation and probability-based color flashes.
 * 
 * Hardware:
 *   - Arduino Nano ESP32
 *   - 2x AZDelivery RGB LED Ring 8 Bit WS2812 5050 (port/starboard)
 *   - Port ring: Pin D2 (counter-clockwise rotation)
 *   - Starboard ring: Pin D3 (clockwise rotation)
 * 
 * Animation:
 *   - Base layer: Rotating red+green fade wave (~50 RPM)
 *   - Flash layer: Random red+blue or pure green bursts
 *   - Independent fade cycles per LED
 *   - Counter-rotating rings for visual symmetry
 * 
 * Author: William Watson
 * Target: PolarLights 1/350 TOS Enterprise
 * License: MIT
 */

#include <FastLED.h>

// Hardware Configuration
#define PIN_PORT        2
#define PIN_STARBOARD   3
#define LED_BUILTIN     LED_BUILTIN  // Arduino Nano ESP32 built-in LED
#define LED_COUNT       8
#define LED_TYPE        WS2812
#define COLOR_ORDER     GRB
#define BRIGHTNESS      255

// Animation Parameters
#define ROTATION_DELAY  150   // milliseconds between rotation steps (50 RPM: 60000ms/min ÷ 50rpm ÷ 8leds = 150ms)
#define FLASH_PROB      12    // Percentage chance per LED per frame (0-100)

// Flash type probabilities (must sum to 100)
#define PROB_RED_BLUE   16    // Red+blue flash
#define PROB_PURE_GREEN 4     // Pure green flash (no red)
// Remaining 80% = normal rotation color

// LED State Structure
struct LEDState {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t fade_dir;       // 0=fade down, 1=fade up
  bool flashing;          // true during flash override
  uint8_t flash_type;     // 1=red+blue, 2=pure_green, 0=none
  uint8_t probability;    // current probability value for flash trigger
};

// LED Arrays
CRGB leds_port[LED_COUNT];
CRGB leds_starboard[LED_COUNT];
LEDState state_port[LED_COUNT];
LEDState state_starboard[LED_COUNT];

// Timing
unsigned long last_rotation = 0;
unsigned long last_heartbeat = 0;
bool heartbeat_state = false;

/*
 * Initialize LED state with random starting values
 */
void initialize_ring(LEDState* ring) {
  for (int i = 0; i < LED_COUNT; i++) {
    ring[i].red = 255;
    ring[i].green = random(1, 64);
    ring[i].blue = 0;
    ring[i].fade_dir = random(0, 2);
    ring[i].flashing = false;
    ring[i].flash_type = 0;
    ring[i].probability = 100;
  }
}

/*
 * Rotate color state through ring
 * Port: counter-clockwise (i = i-1)
 * Starboard: clockwise (i = i+1)
 */
void rotate_colors_ccw(LEDState* ring) {
  LEDState temp = ring[0];
  for (int i = 0; i < LED_COUNT - 1; i++) {
    ring[i] = ring[i + 1];
  }
  ring[LED_COUNT - 1] = temp;
}

void rotate_colors_cw(LEDState* ring) {
  LEDState temp = ring[LED_COUNT - 1];
  for (int i = LED_COUNT - 1; i > 0; i--) {
    ring[i] = ring[i - 1];
  }
  ring[0] = temp;
}

/*
 * Update base color fade cycles
 */
void update_fade(LEDState* state) {
  // Only update base colors if not currently flashing
  if (!state->flashing) {
    // Green channel fade (0-63 range for base glow)
    if (state->green <= 63 && state->fade_dir == 1) {
      state->green++;
      if (state->green >= 63) {
        state->fade_dir = 0;
      }
    } else if (state->green >= 1 && state->fade_dir == 0) {
      state->green--;
      if (state->green <= 1) {
        state->fade_dir = 1;
      }
    }
    
    // Red stays constant at 255 for base color
    state->red = 255;
    state->blue = 0;
  }
}

/*
 * Check for flash trigger and initialize flash sequence
 */
void check_flash_trigger(LEDState* state) {
  if (!state->flashing && random(0, 100) < FLASH_PROB) {
    int prob = random(0, 100);
    
    if (prob < PROB_RED_BLUE) {
      // Red+blue flash
      state->flash_type = 1;
      state->flashing = true;
      state->blue = 1;
      state->red = 255;
      state->green = 0;
      state->fade_dir = 1; // Start fading up
    } else if (prob < (PROB_RED_BLUE + PROB_PURE_GREEN)) {
      // Pure green flash (no red)
      state->flash_type = 2;
      state->flashing = true;
      state->green = 1;
      state->red = 0;
      state->blue = 2; // Slight blue tint
      state->fade_dir = 1;
    }
  }
}

/*
 * Update flash animation (if active)
 */
void update_flash(LEDState* state) {
  if (!state->flashing) return;
  
  if (state->flash_type == 1) {
    // Red+blue flash (0-127 range)
    if (state->blue <= 127 && state->fade_dir == 1) {
      state->blue++;
      if (state->blue >= 127) {
        state->fade_dir = 0;
      }
    } else if (state->blue >= 1 && state->fade_dir == 0) {
      state->blue--;
      if (state->blue <= 1) {
        state->flashing = false;
        state->flash_type = 0;
        state->fade_dir = 1;
      }
    }
  } else if (state->flash_type == 2) {
    // Pure green flash (0-127 range)
    if (state->green <= 127 && state->fade_dir == 1) {
      state->green++;
      if (state->green >= 127) {
        state->fade_dir = 0;
      }
    } else if (state->green >= 1 && state->fade_dir == 0) {
      state->green--;
      if (state->green <= 1) {
        state->flashing = false;
        state->flash_type = 0;
        state->fade_dir = 1;
      }
    }
  }
}

/*
 * Apply final color output to FastLED array
 */
void apply_colors(LEDState* states, CRGB* leds) {
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = CRGB(states[i].red, states[i].green, states[i].blue);
  }
}

void setup() {
  delay(100);
  
  // Initialize built-in LED for heartbeat
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Startup blink sequence - 3 fast blinks to confirm code is running
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  
  // Initialize serial for debugging (optional)
  Serial.begin(115200);
  delay(100);
  
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, PIN_PORT, COLOR_ORDER>(leds_port, LED_COUNT);
  FastLED.addLeds<LED_TYPE, PIN_STARBOARD, COLOR_ORDER>(leds_starboard, LED_COUNT);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Initialize random seed
  randomSeed(analogRead(A0));
  
  // Initialize ring states
  initialize_ring(state_port);
  initialize_ring(state_starboard);
  
  Serial.println("Bussard Rotation Effect Initialized");
  Serial.println("Port: Counter-clockwise | Starboard: Clockwise");
  Serial.print("Rotation Speed: ~50 RPM (");
  Serial.print(ROTATION_DELAY);
  Serial.println(" ms delay)");
  Serial.println("Heartbeat LED: Blinking every 1 second");
  
  // Test LEDs - set all to dim red briefly
  Serial.println("LED Test: All LEDs dim red for 2 seconds...");
  for (int i = 0; i < LED_COUNT; i++) {
    leds_port[i] = CRGB(32, 0, 0);
    leds_starboard[i] = CRGB(32, 0, 0);
  }
  FastLED.show();
  delay(2000);
  Serial.println("Starting animation...");
}

void loop() {
  unsigned long current_time = millis();
  
  // Rotation timing
  if (current_time - last_rotation >= ROTATION_DELAY) {
    last_rotation = current_time;
    
    // Rotate color states through rings
    rotate_colors_ccw(state_port);        // Port: counter-clockwise
    rotate_colors_cw(state_starboard);    // Starboard: clockwise
  }
  
  // Update all LEDs in both rings
  for (int i = 0; i < LED_COUNT; i++) {
    // Port ring
    update_fade(&state_port[i]);
    check_flash_trigger(&state_port[i]);
    update_flash(&state_port[i]);
    
    // Starboard ring
    update_fade(&state_starboard[i]);
    check_flash_trigger(&state_starboard[i]);
    update_flash(&state_starboard[i]);
  }
  
  // Apply colors to FastLED arrays
  apply_colors(state_port, leds_port);
  apply_colors(state_starboard, leds_starboard);
  
  // Update display
  FastLED.show();
  
  // Heartbeat blink (1Hz)
  if (current_time - last_heartbeat >= 1000) {
    last_heartbeat = current_time;
    heartbeat_state = !heartbeat_state;
    digitalWrite(LED_BUILTIN, heartbeat_state ? HIGH : LOW);
  }
}
