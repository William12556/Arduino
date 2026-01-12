Created: 2026 January 13

# PolarLights TOS Enterprise 1:350 Scale - Lighting Project

## Table of Contents

- [Overview](<#overview>)
- [Model Specifications](<#model specifications>)
- [Electronics Architecture](<#electronics architecture>)
- [Lighting Effects](<#lighting effects>)
- [Board Configuration](<#board configuration>)
- [Power Requirements](<#power requirements>)
- [Assembly Notes](<#assembly notes>)
- [Testing Procedures](<#testing procedures>)
- [Version History](<#version history>)

[Return to Table of Contents](<#table of contents>)

## Overview

MicroPython-based lighting effects for PolarLights 1:350 scale TOS Enterprise model. This project implements screen-accurate warp nacelle pulse effects, impulse engine glow, and deflector dish illumination.

[Return to Table of Contents](<#table of contents>)

## Model Specifications

- **Manufacturer:** PolarLights
- **Scale:** 1:350
- **Subject:** USS Enterprise NCC-1701 (TOS)
- **Model Length:** Approximately 92 cm (36 inches)

[Return to Table of Contents](<#table of contents>)

## Electronics Architecture

### Microcontroller Options

- **Primary:** Arduino Nano ESP32
- **Alternative:** Adafruit ItsyBitsy RP2040
- **Development:** Adafruit ItsyBitsy M4 Express

### LED Hardware

- **Type:** NeoPixel (WS2812B) addressable RGB strips
- **Control Protocol:** Single-wire data signal

[Return to Table of Contents](<#table of contents>)

## Lighting Effects

### Warp Nacelles

**Location:** Port and starboard nacelle interiors  
**Sketch:** `warp_nacelles/`  
**Description:** Pulsing blue/white effect simulating warp drive plasma flow through Bussard collectors and nacelle grilles.

### Impulse Engines

**Location:** Aft secondary hull  
**Sketch:** `impulse_engines/`  
**Description:** TBD - Steady amber/orange glow.

### Deflector Dish

**Location:** Forward secondary hull  
**Sketch:** `deflector_dish/`  
**Description:** TBD - Pulsing blue navigational deflector.

[Return to Table of Contents](<#table of contents>)

## Board Configuration

### Pin Assignments

TBD - To be documented during refactoring phase.

### Board Placement

TBD - Physical mounting locations within model.

[Return to Table of Contents](<#table of contents>)

## Power Requirements

TBD - Voltage, current draw, power supply specifications.

[Return to Table of Contents](<#table of contents>)

## Assembly Notes

TBD - Wiring routing, LED strip mounting, model modifications.

[Return to Table of Contents](<#table of contents>)

## Testing Procedures

TBD - Bench testing protocols before model integration.

[Return to Table of Contents](<#table of contents>)

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 0.1.0   | 2025-01-13 | Initial directory structure and documentation |

[Return to Table of Contents](<#table of contents>)

---

Copyright (c) 2025 William Watson. This work is licensed under the MIT License.
