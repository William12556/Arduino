/*
 * GPIO Toggle Test
 * 
 * Minimal test to verify GPIO pins can output signals.
 * Toggles D2 and D3 at 1Hz - measure with multimeter or scope.
 * 
 * HARDWARE: Arduino Nano ESP32
 * 
 * EXPECTED BEHAVIOR:
 * - LED_BUILTIN blinks at 2Hz
 * - D2 toggles LOW/HIGH at 1Hz
 * - D3 toggles LOW/HIGH at 1Hz (opposite phase to D2)
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

void loop() {
  // Toggle LED_BUILTIN every 250ms (2Hz)
  static unsigned long last_led = 0;
  unsigned long now = millis();
  
  if (now - last_led >= 250) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    last_led = now;
  }
  
  // Toggle data pins every 500ms (1Hz)
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
