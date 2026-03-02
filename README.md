Created: 2026 January 13

# A collection of Arduino sketches

## Repository Structure

```
Arduino/
├── projects
│   └── <project>/          # Individual project directories
│       └── src/            # source code
├── lib/                    # Shared libraries
├── docs/
└── README.md               # This file
```

## Hardware

- Level shifter Texas instrument 25bd376d e4 sn74ahct125n pinout
```text
          ┌──────────┐
  1OE GND │ 1     14 │ VCC
  1A  DIN │ 2     13 │ GND 4OE
  1Y  OUT │ 3     12 │ DIN 4A
  2OE GND │ 4     11 │ OUT 4Y
  2A  DIN │ 5     10 │ GND 3OE
  2Y  OUT │ 6      9 │ DIN 3A
  GND     │ 7      8 │ OUT 3Y
          └──────────┘
  OE = Output Enable (active LOW — tie to GND to enable)
  A  = Input  (DIN  — connect to 3.3V logic source)
  Y  = Output (DOUT — connect to 5V peripheral data line)
```
- Arduino Nano ESP32 (ABX00083) pinout — 3.3V logic, USB-C, ESP32-S3
```text
                   ┌───────────────┐
         D1/TX0    │ 1          30 │  VIN
         D0/RX0    │ 2          29 │  GND
           RESET   │ 3          28 │  B1
             GND   │ 4          27 │  VUSB (+5V USB only)
              D2   │ 5          26 │  A7
              D3   │ 6          25 │  A6
              D4   │ 7          24 │  A5/SCL
              D5   │ 8          23 │  A4/SDA
              D6   │ 9          22 │  A3
              D7   │ 10         21 │  A2
              D8   │ 11         20 │  A1
              D9   │ 12         19 │  A0
             D10   │ 13         18 │  B0
             D11   │ 14         17 │  3V3
             D12   │ 15         16 │  D13/LED (SCK)
                   └───────────────┘
                         USB-C
  Note: All GPIO 3.3V logic. No dedicated 5V pin; VUSB only when
        USB powered. No AREF pin. B0/B1 connected to RGB LED pins.
        Level shifter required for 5V peripherals (e.g. WS2812B).
        SPI: D13(SCK), D12(CIPO), D11(COPI). I2C: A4(SDA), A5(SCL).
```

  Arduino Nano ESP32 — Board Label to GPIO Map (Espressif core 3.3.7, remap active)

  | Board Label | GPIO | Notes |
  |---|---|---|
  | D0 | GPIO44 | RX0 |
  | D1 | GPIO43 | TX0 |
  | D2 | GPIO5 | |
  | D3 | GPIO6 | CTS |
  | D4 | GPIO7 | DSR |
  | D5 | GPIO8 | Non-functional in testing; root cause not isolated — avoid |
  | D6 | GPIO9 | Safe for plain GPIO |
  | D7 | GPIO10 | |
  | D8 | GPIO17 | Use raw `17` for RMT (FastLED/NeoPixel data pin) |
  | D9 | GPIO18 | |
  | D10 | GPIO21 | SS |
  | D11 | GPIO38 | COPI/MOSI |
  | D12 | GPIO47 | CIPO/MISO |
  | D13 | GPIO48 | SCK, LED\_BUILTIN |
  | A0 | GPIO1 | DTR |
  | A1 | GPIO2 | |
  | A2 | GPIO3 | |
  | A3 | GPIO4 | |
  | A4 | GPIO11 | SDA |
  | A5 | GPIO12 | SCL |
  | A6 | GPIO13 | |
  | A7 | GPIO14 | |
  | LED\_RED (B1) | GPIO46 | RGB LED red channel |
  | LED\_GREEN (B0) | GPIO0 | RGB LED green channel — strapping pin, use with caution |
  | LED\_BLUE | GPIO45 | RGB LED blue channel |

  Strapping pins (sampled at boot): GPIO0, GPIO3, GPIO45, GPIO46. Avoid driving these at reset.
  SPI flash: GPIO26–GPIO32 reserved. PSRAM: GPIO33–GPIO37 reserved.
  USB-JTAG: GPIO19, GPIO20 reserved for USB.
  Raw GPIO numbers must be used for RMT peripheral (FastLED/NeoPixel). See `docs/kb-esp32s3-fastled-rmt-pin-remap.md`.

- Arduino Nano (ATmega328P) pinout — 5V logic, Mini-USB
```text
                    ┌───────────────┐
            D1/TX   │ 1          30 │  VIN
            D0/RX   │ 2          29 │  GND
            RESET   │ 3          28 │  RESET
              GND   │ 4          27 │  +5V
               D2   │ 5          26 │  A7
              ~D3   │ 6          25 │  A6
               D4   │ 7          24 │  A5/SCL
              ~D5   │ 8          23 │  A4/SDA
              ~D6   │ 9          22 │  A3
               D7   │ 10         21 │  A2
               D8   │ 11         20 │  A1
              ~D9   │ 12         19 │  A0
             ~D10   │ 13         18 │  AREF
             ~D11   │ 14         17 │  +3V3
              D12   │ 15         16 │  D13/LED
                    └───────────────┘
                         Mini-USB
  Note: 5V logic. VIN accepts 7–12V. ~ denotes PWM-capable pin.
        SPI: D13(SCK), D12(CIPO), D11(COPI). I2C: A4(SDA), A5(SCL).
        ICSP header on board (not shown).
```

- Adafruit Pro Trinket 5V 16MHz (ATmega328P) pinout
```text
                    ┌───────────────┐
  RESET             │ 1          12 │  A5/SCL
  D0 RX             │ 2          11 │  A4/SDA
  D1 TX             │ 3          10 │  A3
  ~D3 (INT1/OC2B)   │ 4           9 │  A2
  D4                │ 5           8 │  A1
  ~D5 (OC0B)        │ 6           7 │  A0
  ~D6 (OC0A)        │ 7           6 │  AREF
  D8                │ 8           5 │  D13  (SCK/LED)
  5V out (150mA)    │ 9           4 │  D12  (CIPO)
  VBUS (USB 5V)     │ 10          3 │  ~D11 (OC2A/COPI)
  GND               │ 11          2 │  ~D10 (OC1B/SS)
  VBAT (5.5-16V)    │ 12          1 │  ~D9  (OC1A)
                    └───────────────┘
                          Micro-USB

  FTDI (right to left): GND  GND  VBUS  D0/RX  D1/TX  DTR/RTS

  Note: 5V logic. ~ denotes PWM-capable pin.
        D2, D7 not exposed. A6/A7 ADC-input only (not shown).
        USB via V-USB bootloader. Apple Silicon Macs may
        reject USB enumeration.
```

- Adafruit ItsyBitsy 5V 16MHz (ATmega32u4) pinout
```text
                    ┌───────────────┐
  D0 RX (INT2)      │ 1          14 │  3.3V
  D1 TX (INT3)      │ 2          13 │  MISO (PCINT3)
  SDA (INT1)        │ 3          12 │  MOSI (PCINT2)
  SCL (INT0)        │ 4          11 │  SCK  (PCINT1)
  ~D5 (OC3A/OC4A)   │ 5          10 │  A5   (ADC0)
  D7  (INT6)        │ 6           9 │  A4   (ADC1)
  ~D9  (OC1A/ADC12) │ 7           8 │  A3   (ADC4)
  ~D10 (OC1B/ADC13) │ 8           7 │  A2   (ADC5)
  ~D11 (OC0A/OC1C)  │ 9           6 │  A1   (ADC6)
  ~D12 (ADC9/!OC4D) │ 10          5 │  A0   (ADC7)
  D13  (OC4A)       │ 11          4 │  GND
  VBUS              │ 12          3 │  AREF
  GND               │ 13          2 │  5V
  VBAT              │ 14          1 │  RST
                    └───────────────┘
                          Micro-USB

  VBAT: up to 16V input. VBUS: USB 5V 500mA.

  FTDI (left to right): EN  GND  D8 (ADC11/A8)  D6 (ADC10/A7/OC4D)  D4 (ADC8/A6)

  Note: 5V logic. Native USB (ATmega32u4). ~ denotes PWM-capable pin.
        Hardware Serial1 on D0/D1. SPI on SCK/MOSI/MISO.
        I2C on SDA/SCL.
```

## License

Copyright (c) 2025 William Watson. This work is licensed under the MIT License.
