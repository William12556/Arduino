/*
 * GPIO Toggle Test
 * 
 * Minimal test to verify GPIO pins can output signals.
 * Toggles D2 and D3 at 1Hz - measure with multimeter or scope.
 * 
 * HARDWARE: Arduino Nano ESP32
 * 
 * EXPECTED BEHAVIOR:
 * - LED_BUILTIN double-flashes as heartbeat indicator.
 *   Pattern: ON 100ms, OFF 250ms, ON 100ms, OFF 1000ms, repeat.
 * - D2 toggles LOW/HIGH at 1.5s interval
 * - D3 toggles LOW/HIGH at 1.5s interval (opposite phase to D2)
 * - Multimeter should show ~1.65V average on both pins
 */

#define PIN_PORT        D2      // GPIO2
#define PIN_STARBOARD   D3      // GPIO3

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PORT, OUTPUT);
  pinMode(PIN_STARBOARD, OUTPUT);
  
  Serial.println("GPIO Toggle Test Started");
  Serial.print("PIN_PORT (D2) = GPIO");
  Serial.println(PIN_PORT);
  Serial.print("PIN_STARBOARD (D3) = GPIO");
  Serial.println(PIN_STARBOARD);
}

// Heartbeat timing table: {pin state, duration ms}
static const struct { uint8_t state; unsigned int duration; } HB[] = {
  { HIGH, 100  },   // Flash 1 on
  { LOW,  250  },   // Gap
  { HIGH, 100  },   // Flash 2 on
  { LOW,  1000 }    // Pause
};
#define HB_STEPS (sizeof(HB) / sizeof(HB[0]))

/**
 * Non-blocking heartbeat update.
 * Call as frequently as possible from loop().
 * Pattern: ON 100ms | OFF 250ms | ON 100ms | OFF 1000ms | repeat.
 */
void updateHeartbeat() {
  static uint8_t       step  = 0;
  static unsigned long timer = 0;
  unsigned long now = millis();

  if (now - timer >= HB[step].duration) {
    step  = (step + 1) % HB_STEPS;
    timer = now;
    digitalWrite(LED_BUILTIN, HB[step].state);
  }
}

void loop() {
  unsigned long now = millis();

  updateHeartbeat();

  // Toggle data pins every 1.5s
  static unsigned long last_toggle = 0;
  static bool state = false;

  if (now - last_toggle >= 1500) {
    state = !state;
    digitalWrite(PIN_PORT, state ? HIGH : LOW);
    digitalWrite(PIN_STARBOARD, state ? LOW : HIGH);  // Opposite phase

    Serial.print("Toggle: PORT=");
    Serial.print(state ? "HIGH" : "LOW");
    Serial.print(", STARBOARD=");
    Serial.println(state ? "LOW" : "HIGH");

    last_toggle = now;
  }
}
