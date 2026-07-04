# Usage

Power the board. On the TV (AV mode) you get a black screen with a thick trace
and text at top-centre.

Press the **BOOT** button to cycle modes:

1. **AMPLITUDE** - turn the pot: the sine grows/shrinks. Shows `AMP 0-100`.
2. **FREQUENCY** - turn the pot: more/fewer cycles fit. Shows `CYCLES n`.
3. **SHAPE** - turn the pot: sine -> square -> triangle -> saw. Shows the name.
4. **LIVE** - plots the real ADC signal. Shows `VDC`, `Vpp`, `F` (Hz).

### Tuning (top of `src/main.c`)
| Define | Effect |
|---|---|
| `SAMPLE_DELAY_US` | timebase in LIVE mode (larger = more cycles on screen) |
| `TRACE_THICKNESS` | trace thickness in pixels |
| `TEST_SIGNAL_ENABLE` | 1 = built-in 250 Hz square on GPIO27 for self-test |

### What you can measure (LIVE mode)
- 1.5 V / 1.2 V / 3 V batteries (read VDC). Never 9 V or above 3.3 V.
- LDR, potentiometer, thermistor (as a divider)
- 3.3 V PWM / logic
- RC charge/discharge curves
- Analog mic module output (3.3 V powered), for a live audio waveform

Not measurable: mains, anything above 3.3 V, fast (>~kHz) signals.
