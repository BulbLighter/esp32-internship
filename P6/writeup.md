# Project 6: Pulse Counting with Interrupts (Machine Counter)

## Objective

Count pulses using a hardware interrupt on the ESP32, compute a rolling parts-per-minute rate, and POST `{count_total, rate_per_min}` over WiFi every 30 seconds — simulating the core logic of an OEE counting node.

## 1. Polling vs Interrupts

A polling-based counter was built first, using `digitalRead()` inside `loop()` with a `delay(400)` to simulate real CPU load (e.g. a WiFi call or sensor read). With this busy-window in place:

- **21 button presses → only 3 counted** (polling)
- **5 button presses → 5 counted** (interrupt-based, same busy-delay in `loop()`)

This demonstrates that polling only sees the pin when it happens to check it — any transition occurring while the CPU is occupied elsewhere is invisible to it. A hardware interrupt fires the instant the electrical edge occurs, regardless of what `loop()` is doing.

## 2. ISR Discipline

The interrupt handler was written to standard embedded practice:
- **`volatile`** on the shared counter, so the compiler always re-reads it from memory instead of caching a stale value in a register.
- **`IRAM_ATTR`** on the ISR, placing it in internal RAM so it remains executable even if Flash is momentarily busy.
- **`portMUX_TYPE` critical sections** around all reads/writes of the shared counter, to keep access atomic across the ESP32's two cores.

## 3. Debounce — Button

Without any debounce, a button on GPIO21 (`INPUT_PULLUP`, `FALLING` interrupt) gave:

- **20 presses → 24 counted** (~20% inflated)

Diagnosis wasn't straightforward. Interval-based software debounce was tried across a wide range of values (10ms, 50ms, 100ms), all producing similar inflation, which ruled out simple bounce-duration tuning as the fix. Raw diagnostic logging was used to find the real cause: a `digitalRead()` polling log of a single click showed exactly one clean LOW and one clean HIGH transition — the physical contact wasn't bouncing. But a raw ISR-fire timestamp log of a single click showed **one interrupt fire on press, and 19–57 spurious fires on release**. The actual problem was electrical glitching at the interrupt controller level — most likely EMI/static coupling near the jumper wire, triggered by hand movement during release — not mechanical bounce.

**Fix:** on any valid trigger, detach the interrupt immediately, and reattach it only after a 200ms cooldown checked from `loop()` (not from timestamp math inside the ISR itself, since the glitch had no fixed timing relationship to the press).

- **With fix: 5 presses → 5 counted** (clean)

## 4. Two Pulse Sources

- **Source A** — the debounced push button (manual "part produced" trigger), covered above.
- **Source B** — a second ESP32 programmed as a 10Hz square-wave generator (`digitalWrite` toggling every 50ms), connected via GPIO23(gen) → GPIO21(counter) with common GND. The generator's own signal was independently verified as clean and precise using raw transition logging (consistent 50ms/50ms spacing, no jitter).

Applying the button's 200ms detach/reattach fix directly to this signal caused severe **under**-counting (~5/sec instead of 10/sec), since the cooldown — tuned for human press timing — was too long for pulses arriving every 100ms. Removing it and going back to a plain mutex-protected ISR still showed a ~15–20% **over**-count, even though the generator signal was already confirmed clean. Gap-timestamp logging on the counter side showed the extra triggers landing consistently at almost exactly the 50ms mark (the signal's rising edge), not randomly — pointing to electrical ringing on the fast GPIO transition rather than a source or wiring fault. A 100nF RC low-pass filter added between GPIO21 and GND reduced this over-count to roughly 3–10% across test runs, without fully eliminating it.

## 5. Rolling Rate Computation

Rate is computed once every 60-second window rather than as a cumulative average:

```
rate = (count - countAtWindowStart) * 60000.0 / elapsed
```

This keeps the reported rate responsive to current behavior instead of being smoothed/hidden by the entire run's history, which matters for a live production-monitoring use case.

## 6. WiFi POST

A minimal Flask server was built from scratch (`/pulses` POST + GET, CSV logging). The counter ESP32 connects to WiFi and POSTs `{count_total, rate_per_min}` every 30 seconds. Verified end-to-end — all POSTs returned HTTP 200, and every entry was correctly logged to `pulse_data.csv` with a timestamp.

## 7. 10-Minute Accuracy Test

Using server log timestamps as the ground truth, a clean 600-second window was extracted:

| Metric | Value |
|---|---|
| Duration | 600 s |
| Expected count (10 Hz × 600 s) | 6000 |
| Actual count | 6581 |
| Absolute error | 581 |
| **Error %** | **+9.68%** |

## 8. Justified Tolerance

The residual ~9.68% error is attributed to incomplete suppression of GPIO edge ringing on the counter's interrupt pin. This was directly measured, not assumed: without any filtering the error was ~20%; with a 100nF RC filter it dropped to ~3–10% in shorter test runs, and to 9.68% over the full 10-minute run. Further reduction would likely require a larger/lower-ESR filter capacitor, a hardware Schmitt-trigger buffer on the signal line, or shielded/shorter wiring between the two boards — none of which were pursued here given time constraints. The error is one-directional (consistently over, never under), which is consistent with a real electrical-ringing mechanism rather than random noise or missed counts.

## Deliverables Summary

- **Accuracy report**: 6581 actual vs 6000 expected, +9.68% error, root cause identified and partially mitigated.
- **Debounce fix demonstration**: 24/20 inflated → 5/5 clean (button, after detach/reattach fix).
- **Live rate data**: confirmed via Flask server CSV log, `count_total` and `rate_per_min` updating correctly every 30s.