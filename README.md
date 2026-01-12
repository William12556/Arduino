Created: 2026 January 13

# Arduino Model Lighting Projects

## Table of Contents

- [Overview](<#overview>)
- [Repository Structure](<#repository structure>)
- [Supported Models](<#supported models>)
- [Technology Stack](<#technology stack>)
- [Getting Started](<#getting started>)
- [Contributing](<#contributing>)
- [License](<#license>)
- [Version History](<#version history>)

[Return to Table of Contents](<#table of contents>)

## Overview

MicroPython-based lighting effects for scale model starships. This repository contains modular, reusable code for implementing screen-accurate lighting effects in plastic model kits.

[Return to Table of Contents](<#table of contents>)

## Repository Structure

```
Arduino/
├── src/                                    # Model-specific projects
│   └── PolarLights_TOS_Enterprise_1-350/  # Individual model directory
│       ├── warp_nacelles/                 # Effect-specific sketches
│       ├── impulse_engines/
│       ├── deflector_dish/
│       └── MODEL_README.md                # Model documentation
├── lib/                                    # Shared libraries
│   ├── neopixel_effects.py
│   ├── pulse_engine.py
│   ├── color_utils.py
│   └── README.md
└── README.md                               # This file
```

[Return to Table of Contents](<#table of contents>)

## Supported Models

- **PolarLights TOS Enterprise 1:350** - In development
- **AMT Enterprise Bridge 1:32** - Planned
- **Klingon D7** - Planned

[Return to Table of Contents](<#table of contents>)

## Technology Stack

### Hardware

- Arduino Nano ESP32 (primary platform)
- Adafruit ItsyBitsy RP2040 (alternative platform)
- NeoPixel addressable RGB LED strips (WS2812B/SK6812)

### Software

- MicroPython firmware
- NeoPixel library for MicroPython
- Modular effect libraries

[Return to Table of Contents](<#table of contents>)

## Getting Started

### Prerequisites

- MicroPython-compatible microcontroller board
- USB cable for programming
- NeoPixel LED strips
- Power supply (voltage/current rated for LED count)

### Installation

1. Flash MicroPython firmware to board
2. Upload model-specific sketch to board
3. Configure pin assignments in `config.py`
4. Connect LED strips to designated pins
5. Apply power and test

Detailed instructions in individual model README files.

[Return to Table of Contents](<#table of contents>)

## Contributing

This is a personal project repository. External contributions are not currently accepted.

[Return to Table of Contents](<#table of contents>)

## License

Copyright (c) 2025 William Watson. This work is licensed under the MIT License.

[Return to Table of Contents](<#table of contents>)

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 0.1.0   | 2025-01-13 | Initial repository structure for PolarLights TOS Enterprise |

[Return to Table of Contents](<#table of contents>)
