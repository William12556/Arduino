Created: 2025 January 13

# Bussard Rotation Effect

## Table of Contents

- [Overview](<#overview>)
- [Hardware Configuration](<#hardware configuration>)
- [Animation Algorithm](<#animation algorithm>)
- [Configuration Parameters](<#configuration parameters>)
- [Implementation Notes](<#implementation notes>)
- [Version History](<#version history>)

[Return to Table of Contents](<#table of contents>)

## Overview

Counter-rotating color wave animation for Star Trek TOS Bussard collectors using two 8-LED WS2812 rings. Implements continuous rotation with probability-based flash overlays to simulate the organic lighting characteristics observed in original series footage.

**Key Features:**
- Independent counter-rotating rings (port CCW, starboard CW)
- Rotating red+green base color wave
- Random red+blue and pure green flash overlays
- ~50 RPM rotation speed
- Independent LED fade cycles

[Return to Table of Contents](<#table of contents>)

## Hardware Configuration

**Board:** Arduino Nano ESP32

**LED Hardware:**
- 2x AZDelivery RGB LED Ring 8 Bit WS2812 5050
- Port ring: Pin D2
- Starboard ring: Pin D3
- Color order: GRB
- Protocol: WS2812 (800kHz)

**Power Requirements:**
- 8 LEDs × 60mA max × 2 rings = 960mA maximum
- Typical operation: ~400-600mA (due to color mixing)

[Return to Table of Contents](<#table of contents>)

## Animation Algorithm

### Base Layer: Rotating Color Wave

Each LED maintains independent state (red, green, blue, fade_direction). Color states rotate through ring each frame:
- Port: Counter-clockwise rotation
- Starboard: Clockwise rotation
- Base color: Red (255) + Green fade (0-63)
- Rotation delay: 24ms per step (~50 RPM)

### Flash Layer: Probability-Based Overlays

Each LED independently checks for flash trigger each frame:
- 12% total flash probability per LED per frame
- Flash types:
  - Red+Blue: 16% of flashes (255, 0, 0-127)
  - Pure Green: 4% of flashes (0, 0-127, 2)
- Flash overrides base color during fade-up/fade-down cycle
- Returns to rotation pattern after flash completes

### Update Sequence

1. Check rotation timing (24ms intervals)
2. Rotate color states through rings
3. Update base color fade cycles
4. Check flash triggers (random probability)
5. Update active flash animations
6. Apply composite colors to FastLED
7. Display output

[Return to Table of Contents](<#table of contents>)

## Configuration Parameters

### Timing Parameters

```cpp
#define ROTATION_DELAY  24    // Rotation step delay in milliseconds
#define FLASH_PROB      12    // Flash probability per LED per frame (%)
```

**Rotation Speed Calculation:**
- 50 RPM = 50 rotations/minute = 60000ms / 50 = 1200ms per rotation
- 8 LEDs per rotation = 1200ms / 8 = 150ms per LED step
- Current setting: 24ms = ~312 RPM (faster than original design)
- Adjust ROTATION_DELAY to achieve desired visual effect

### Color Probabilities

```cpp
#define PROB_RED_BLUE   16    // Percentage of flashes that are red+blue
#define PROB_PURE_GREEN 4     // Percentage of flashes that are pure green
```

### Hardware Parameters

```cpp
#define PIN_PORT        2     // Port ring data pin
#define PIN_STARBOARD   3     // Starboard ring data pin
#define LED_COUNT       8     // LEDs per ring
#define BRIGHTNESS      255   // Global brightness (0-255)
```

[Return to Table of Contents](<#table of contents>)

## Implementation Notes

### Thread Safety

All state updates occur within single-threaded `loop()` function. No interrupt conflicts with FastLED output.

### Memory Usage

- 2 rings × 8 LEDs × 3 bytes (CRGB) = 48 bytes for LED output
- 2 rings × 8 LEDs × sizeof(LEDState) = ~112 bytes for state tracking
- Total: ~160 bytes SRAM for LED management

### Performance

- Frame rate: Limited by rotation delay (24ms) + processing time
- Processing overhead: <1ms per frame for 16 LEDs
- Effective frame rate: ~41 FPS

### Debugging

Serial output provides initialization confirmation. Monitor at 115200 baud.

### Migration from Original Code

**Changes from Bussards.ino:**
- Reduced from 10 to 8 LEDs per ring
- Adafruit_NeoPixel → FastLED library
- Added rotation algorithm
- Unified flash probability system
- Counter-rotation support
- Removed redundant code duplication

[Return to Table of Contents](<#table of contents>)

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 1.0.0   | 2025-01-13 | Initial implementation with counter-rotation and FastLED |

[Return to Table of Contents](<#table of contents>)

---

Copyright (c) 2025 William Watson. This work is licensed under the MIT License.
