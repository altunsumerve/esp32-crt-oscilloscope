# Usage Examples (Potentiometer & LDR)

Both connect to the same pin (GPIO34), the single analog input where the
signal/knob is read.

## 1) Potentiometer
| Pot pin | To |
|---|---|
| Outer pin 1 | 3V3 |
| Middle pin (wiper) | GPIO34 |
| Outer pin 2 | GND |

Value: 10k ideal (1k-100k works).

**How to use:** in AMPLITUDE/FREQUENCY/SHAPE modes the pot is the control knob
(size / cycle count / waveform type). In LIVE mode it is a voltage level (VDC
changes as you turn it). Press the on-board BOOT button to change modes.

## 2) LDR (light sensor)
Chain: 3V3 -> LDR -> node -> resistor -> GND, node to GPIO34.

| Part | One end | Other end |
|---|---|---|
| LDR | 3V3 | node |
| Resistor | node | GND |
| Jumper | node | GPIO34 |

Resistor: 10k recommended (220 ohm is too low — output sticks near 3.3V).

**How to use:** in AMPLITUDE mode, shine light -> wave grows, shade it -> wave
shrinks. In LIVE mode the VDC value tracks the light level.

Both outputs stay within 0-3.3V, so no protection diodes are needed.
