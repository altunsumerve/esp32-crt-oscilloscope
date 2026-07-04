# Wiring

## Video output
| From | To |
|---|---|
| GPIO26 | Yellow RCA centre pin (video) |
| GND | Yellow RCA outer shield |
| 100 ohm | between the GPIO26 line and GND (75 ohm-ish termination) |

TV in AV mode, PAL. Video is on **GPIO26 (DAC2)** — the library default DAC1 is
patched to DAC2 by `scripts/setup.*`.

## Signal input (GPIO34 / ADC1_CH6)
Simplest (safe 0-3.3 V sources): connect the source directly to GPIO34, GND common.

Potentiometer (used as the control knob): outer pins to 3V3 and GND, wiper to GPIO34.

LDR: 3V3 -> LDR -> node -> 10k -> GND, node to GPIO34.

Optional protection for unknown external signals: 220 ohm in series from the probe
to GPIO34, plus two 1N4148 clamp diodes (one to 3V3, one to GND). Turkish
step-by-step: [baglanti.md](baglanti.md).

## Mode button
The on-board **BOOT** button (GPIO0) cycles the modes. No extra wiring.
