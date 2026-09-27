# Data

`attempts.csv` is written by `tools/logger.py`: one row per card attempt, with the columns
`datetime`, `millis`, `uid` and `result` (`GRANTED`, `DENIED` or `LOCKOUT`).

`measurements.csv` holds the reader measurements, one row per trial, ten trials per condition.

| Column | Meaning |
|---|---|
| `experiment` | `range`, `angle` or `barrier` |
| `target` | `card` or `keyfob` |
| `condition` | angle (`0deg` to `90deg`) or material between card and reader (`none` is the reference) |
| `trial` | 1 to 10 |
| `distance_mm` | distance between the target and the reader surface |
| `detected` | `1` if the card was read, `0` if not |

For `range`, the target moves slowly towards the reader and `distance_mm` is where it is first read.
For `angle` and `barrier`, the target is held at a fixed distance.
