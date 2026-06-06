# SO₂ Gas Detector

Portable sulphur dioxide gas detector using Arduino Uno and MQ-135 sensor. Detects SO₂ concentration in the 10–1000 PPM range with ~1% error, displays real-time readings on a 16x2 LCD, and triggers a buzzer alarm at 1000 PPM threshold. Built at under ₹1000 — a fraction of commercial SO₂ monitoring units.

---

## Overview

The MQ-135 is a chemiresistive gas sensor whose internal resistance drops as target gas concentration increases. A voltage divider converts this resistance change to an analog voltage, read by Arduino's 10-bit ADC. The firmware applies a power-law curve fit from the MQ-135 datasheet to convert the ADC reading to PPM.

Tested by burning a matchstick near the sensor to simulate SO₂ from sulphur combustion — readings exceeded 800 PPM during peak exposure, confirming sensor response and alert pipeline.

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | Arduino Uno (ATmega328P) |
| Gas sensor | MQ-135 (chemiresistive, analog output) |
| Display | 16x2 LCD (4-bit mode) |
| Alert | Buzzer — GPIO8 |
| ADC | 10-bit (0–1023 → 0–5V) |
| Sampling rate | 1 Hz |
| Power | USB 5V |

---

## Circuit

```
5V ──[RL=10kΩ]──┬── A0 (Arduino ADC)
                 │
              MQ-135
              (Rs varies with gas concentration)
                 │
               GND

LCD: RS=7, EN=6, D4=5, D5=4, D6=3, D7=2
Buzzer: GPIO8 → GND
```

---

## Sensor Principle

MQ-135 resistance (Rs) decreases as SO₂ concentration increases. PPM is calculated using a power-law curve fit from the datasheet:

```
PPM = A × (Rs/Ro)^B
```

Where:
- **Ro** = sensor resistance in clean air (baseline, ~76.63kΩ)
- **Rs** = sensor resistance at measured concentration
- **A = 116.602, B = -2.769** (SO₂ curve constants)
- **Rs/Ro** = ratio calculated from ADC voltage reading

---

## Alert Levels

| PPM Range | LCD Display | Buzzer |
|-----------|-------------|--------|
| < 500 | "Status: Safe" | OFF |
| 500–999 | "WARNING: High" | OFF |
| ≥ 1000 | "!! DANGER !!" | ON |

---

## Results

| Condition | Reading |
|-----------|---------|
| Clean air (baseline) | < 10 PPM |
| Matchstick combustion (SO₂ simulation) | 800–950 PPM |
| Peak exposure | > 1000 PPM — buzzer triggered |
| Sampling rate | 1 Hz (50-sample average per reading) |
| Measurement error | ~1% |

---

## Setup

1. Wire circuit per diagram above
2. Open `so2_gas_detector.ino` in Arduino IDE
3. Upload to Arduino Uno
4. Allow 60 seconds warm-up for MQ-135 to stabilise
5. Open Serial Monitor at 9600 baud for debug output

---

## Cost Comparison

| Item | Cost |
|------|------|
| Arduino Uno | ~₹500 |
| MQ-135 sensor | ~₹150 |
| 16x2 LCD | ~₹120 |
| Buzzer | ~₹20 |
| Resistors + wires | ~₹50 |
| **Total BOM** | **~₹840** |
| Commercial SO₂ detector | ₹6000+ |

---

## Dependencies

- `LiquidCrystal.h` — Arduino built-in library

---

## Author

**Abhiram Kurella**  
B.Tech Electronics and Instrumentation Engineering  
VNR Vignana Jyothi Institute of Engineering & Technology (2023–2027)  
abhiram.kurella@gmail.com
