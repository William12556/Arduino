Created: 2026 January 13

# Shared MicroPython Libraries

## Table of Contents

- [Overview](<#overview>)
- [Library Modules](<#library modules>)
- [Usage Guidelines](<#usage guidelines>)
- [Version History](<#version history>)

[Return to Table of Contents](<#table of contents>)

## Overview

Common utility functions and effect algorithms shared across multiple model projects. Centralizing reusable code eliminates redundancy and maintains consistency.

[Return to Table of Contents](<#table of contents>)

## Library Modules

### neopixel_effects.py

Reusable NeoPixel effect functions.

**Functions:** TBD during refactoring phase

### pulse_engine.py

Timing algorithms for pulsing effects with independent fade cycles.

**Functions:** TBD during refactoring phase

### color_utils.py

Color space conversions and palette management.

**Functions:** TBD during refactoring phase

[Return to Table of Contents](<#table of contents>)

## Usage Guidelines

Import shared libraries from project sketches:

```python
# Example usage in main.py
import sys
sys.path.append('../../lib')

from neopixel_effects import pulse_fade
from color_utils import hsv_to_rgb
```

[Return to Table of Contents](<#table of contents>)

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 0.1.0   | 2025-01-13 | Initial directory structure |

[Return to Table of Contents](<#table of contents>)

---

Copyright (c) 2025 William Watson. This work is licensed under the MIT License.
