# Data

`attempts.csv` is written by `tools/logger.py`: one row per card attempt, with the columns
`datetime`, `millis`, `uid` and `result` (`GRANTED`, `DENIED` or `LOCKOUT`).

`measurements.csv` is the template for physical reader measurements.
[`simulated_measurements.csv`](simulated_measurements.csv) holds the 160 virtual trials reported on the site.

| Column | Meaning |
|---|---|
| `experiment` | `range`, `angle`, `barrier` or `barrier_range` |
| `target` | `card` or `keyfob` |
| `condition` | angle (`0deg` to `90deg`) or material between card and reader (`none` is the reference) |
| `trial` | 1 to 10 |
| `distance_mm` | distance between the target and the reader surface. Blank if a sweep made no read |
| `detected` | `1` for a successful read or `0` for a missed read |

For `range` and `barrier_range`, `distance_mm` records the first read while approaching the reader.
For `angle` and `barrier`, `distance_mm` is the fixed test distance, set to 27 mm in the simulation.

## Simulation model

The simulated signal is `F(d) × cos(θ) × m × g`, where `F(d) = (1 + (d / 20)²)⁻³ᐟ²` approximates the
on-axis field of a circular reader coil with an effective radius of 20 mm. `θ` is the tilt from parallel,
`m` is the assumed signal multiplier for the material, and `g` varies with the trial. A read succeeds if
the signal reaches `F(38)` for the card or `F(24)` for the key fob. These are assumed flat, unobstructed ranges
in millimetres. The NXP datasheet gives a typical operating distance of up to 50 mm, depending on antenna
size and tuning. [MFRC522 datasheet](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf)

Python's `random.Random(522)` draws ten values for `g` with `uniform(0.8, 1.2)`. Each condition uses those
same ten values in the same order. A distance sweep starts at 50 mm and tests every millimetre down to the
material's assumed thickness. The table reports the mean first-read distance across successful sweeps.

| Condition | Signal multiplier `m` | Minimum gap (mm) |
|---|---:|---:|
| No barrier | 1.00 | 0 |
| Metal card holder | 0.06 | 3 |
| Mobile phone | 0.44 | 8 |
| Paper notebook | 0.91 | 8 |
| Second RFID card | 0.56 | 1 |

The multipliers are scenario assumptions. Metal can weaken the field and detune an NFC antenna, so the model
uses stronger losses for the holder and phone than for paper. The second-card multiplier represents weaker coupling
to the target card. UID selection and anti-collision behaviour require a separate test. [NXP antenna design guide, §2.2](https://www.nxp.com/docs/en/application-note/AN11564.pdf)
