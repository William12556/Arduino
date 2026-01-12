Created: 2026 January 13

# Warp Nacelle Lighting Effects

## Table of Contents

- [Overview](<#overview>)
- [Effect Description](<#effect description>)
- [Hardware Requirements](<#hardware requirements>)
- [Configuration](<#configuration>)
- [Implementation Status](<#implementation status>)
- [Version History](<#version history>)

[Return to Table of Contents](<#table of contents>)

## Overview

Pulsing blue/white effect simulating warp drive plasma flow through Bussard collectors and nacelle grilles. This sketch controls both port and starboard nacelle lighting synchronized to create screen-accurate TOS Enterprise warp effect.

[Return to Table of Contents](<#table of contents>)

## Effect Description

The warp nacelle effect combines multiple lighting elements:

- **Bussard Collectors:** Pulsing red/orange glow at nacelle front caps
- **Nacelle Grilles:** Sequential blue/white pulse traveling aft along nacelle length
- **Synchronization:** Port and starboard nacelles pulse in phase

[Return to Table of Contents](<#table of contents>)

## Hardware Requirements

- **LED Type:** NeoPixel RGBW strips (WS2812B or SK6812)
- **Port Nacelle:** TBD pixel count
- **Starboard Nacelle:** TBD pixel count
- **Data Pins:** TBD

[Return to Table of Contents](<#table of contents>)

## Configuration

### Board-Specific Settings

Configuration parameters isolated in `config.py` for portability across boards.

**Parameters:**
- Pin assignments
- LED pixel counts
- Brightness levels
- Pulse timing parameters
- Color definitions

[Return to Table of Contents](<#table of contents>)

## Implementation Status

**Current State:** Directory structure created. Awaiting code refactoring from legacy Arduino sketches.

**Source Material:**
- `/Users/williamwatson/Documents/Arduino/src/Bussards/Bussards.ino`
- `/Users/williamwatson/Documents/Arduino/src/nacells/` sketches

**Migration Task:** Port Arduino/C++ NeoPixel code to MicroPython in next session.

[Return to Table of Contents](<#table of contents>)

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 0.1.0   | 2025-01-13 | Initial documentation structure |

[Return to Table of Contents](<#table of contents>)

---

Copyright (c) 2025 William Watson. This work is licensed under the MIT License.
