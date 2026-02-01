/*
 * FastLED Single LED Test
 * 
 * Tests ONE LED on port ring only.
 * Isolates FastLED timing/configuration issues.
 * 
 * HARDWARE: Arduino Nano ESP32 + 74AHCT125N + WS2812B ring
 * 
 * TEST SEQUENCE:
 * 1. Upload and observe Serial output for GPIO mapping
 * 2. Check if LED[0] lights up
 * 3. If working, gradually increase LED_COUNT
 */

#include <FastLED.h>

#define PIN_PORT        D2      
#define LED_COUNT       1       // Test ONE LED only

CRGB leds[LED_COUNT];

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Print actual GPIO mapping
  Serial.println("\n=== FastLED Single LED Test ===");
  Serial.print("PIN_PORT (D2) maps to GPIO");
  Serial.println(PIN_PORT);
  Serial.print("LED_COUNT: ");
  Serial.println(LED_COUNT);
  
  // Startup blinks
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  
  // Try multiple FastLED configurations
  Serial.println("\nTesting GRB configuration...");
  FastLED.addLeds<WS2812B, PIN_PORT, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(255);  // Full brightness for visibility
  FastLED.clear();
  FastLED.show();
  
  Serial.println("Setup complete. Starting color cycle...\n");
}

void loop() {
  static unsigned long last_heartbeat = 0;
  unsigned long now = millis();
  
  // Heartbeat
  if (now - last_heartbeat >= 500) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    last_heartbeat = now;
  }
  
  // RED
  Serial.println("RED");
  leds[0] = CRGB(255, 0, 0);  // Explicit RGB values
  FastLED.show();
  delay(2000);
  
  // GREEN
  Serial.println("GREEN");
  leds[0] = CRGB(0, 255, 0);
  FastLED.show();
  delay(2000);
  
  // BLUE
  Serial.println("BLUE");
  leds[0] = CRGB(0, 0, 255);
  FastLED.show();
  delay(2000);
  
  // WHITE
  Serial.println("WHITE");
  leds[0] = CRGB(255, 255, 255);
  FastLED.show();
  delay(2000);
  
  // OFF
  Serial.println("OFF\n");
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
  delay(2000);
}
