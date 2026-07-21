# Project 2: Temperature & Humidity Logger (DHT22) + WiFi — Write-up

## Objective

Read a DHT22 temperature/humidity sensor on an ESP32 every 10 seconds and POST the readings as JSON to a locally-hosted Flask server, which logs them to CSV. Built and validated over 7 stages, culminating in a 3+ hour unattended run with zero failures.

## Hardware and Setup

- ESP32 DevKit V1 (38-pin)
- DHT22 (AM2302) sensor, 3-pin blue module with an onboard pull-up resistor already soldered to the PCB
- Wiring: VCC → 3V3, DATA → GPIO4, GND → GND (no external resistor needed since the module includes one)
- PC running Python 3 + Flask, connected to the ESP32 over a shared mobile hotspot

## The Pull-up Resistor

The DHT22's DATA line uses **open-drain signaling**: the sensor can only pull the line LOW. It has no way to actively drive the line HIGH itself. A pull-up resistor connects DATA to 3.3V so that whenever the sensor isn't actively pulling low, the line settles back to a defined HIGH state instead of floating.

Without this pull-up, the line has no reference voltage between pulses — it behaves exactly like the floating-input problem seen with an unconnected button pin in the earlier debounce project: it picks up ambient electromagnetic noise. The DHT22 communicates its 40 bits of data using **pulse-width encoding** (a start signal, an acknowledgment, then bits distinguished by how long the line stays high after each ~50µs low pulse). This encoding depends on precise timing, so a noisy, undefined line corrupts the read and fails the library's checksum check.

The module used here has the 4.7k–10k pull-up already built onto its PCB, so no external resistor was wired — this was confirmed by measuring ~3.0–3.3V on the DATA line at rest with a multimeter, before any sensor read was triggered.

## Failure Experiment

Two deliberate failures were run to observe the sensor's actual failure modes, not just the ones described in documentation.

**1. Removing the pull-up reference (simulated by disconnecting DATA).**
Since the pull-up is on-module, the equivalent test was physically disconnecting the DATA jumper from GPIO4 and leaving it floating. Result: 5/5 consecutive reads failed with `Failed to read from DHT sensor!`. Reconnecting the wire and power-cycling restored clean reads immediately, confirming the disconnection was the sole cause. A DC voltage check on the floating pin showed an unstable, drifting reading rather than a steady 0V or 3.3V — direct evidence of the floating condition.

**2. Polling faster than the sensor's physical limit.**
`READ_INTERVAL` was set to 500ms, well under the DHT22's ~2 second minimum measurement cycle. The result was *not* the expected NaN/checksum errors. Instead, the Serial Monitor showed clean-looking readings repeating in blocks of about 4 (500ms × 4 ≈ 2000ms, matching the sensor's real cycle time), with small jumps between blocks. The protocol handshake itself was succeeding each time — the library was simply returning its last cached value because the sensor's physical measurement hadn't actually refreshed yet.

This second result was more interesting than expected: over-polling doesn't necessarily break the protocol, it produces **silent stale data** with no error indication at all. That is arguably a worse failure mode than an outright NaN, since nothing on the Serial Monitor flags it as wrong — a system polling too fast could report a false sense of high-resolution data. This directly justified using `READ_INTERVAL = 10000ms`, a 5x safety margin over the sensor's minimum.

## Non-blocking Timing

All recurring behavior — sensor reads, WiFi health checks, buffer flush attempts — runs on independent `millis()`-based timers within a single `loop()`, rather than using `delay()`, which would freeze the whole program. The one deliberate exception is the initial `WiFi.begin()` at boot, which blocks for up to 15 seconds since nothing else needs to run during startup.

## Server

A minimal Flask server exposes `POST /readings`, validates the incoming JSON has `temp`, `hum`, and `ts`, and appends a row to `readings.csv` with a server-generated `received_at` UTC timestamp. It listens on `0.0.0.0:5000` so the ESP32 (a separate device on the network) can reach it — Flask's default `127.0.0.1` binding only accepts connections from the same machine. `received_at` was used for all charting and timing analysis, since the ESP32's own `ts` field is just `millis()` — elapsed time since boot, meaningless as an absolute time and reset to 0 on every reboot.

One real debugging incident is worth recording: an early integration attempt returned an HTTP 404 styled like an IIS default error page, not Flask's own 404 format — a signal the request wasn't reaching Flask at all. This was isolated by hitting the server's URL directly from a phone browser (which correctly returned Flask's own 405, confirming the server itself was fine), narrowing the bug to the ESP32 code. The cause was a missing `:5000` port in `SERVER_URL`, which had silently defaulted to port 80 and was hitting an unrelated service on the network.

## Error-Handling Strategy: Buffer and Send on Reconnect

Three strategies were considered: skip-and-log, retry-3x-then-skip, and buffer-and-resume. Buffer-and-resume was chosen and implemented, since it preserves data through outages of arbitrary length (bounded only by buffer size) rather than losing data on anything longer than a few seconds.

Implementation: a circular buffer holding up to 360 readings (~1 hour of coverage at the 10s interval, ~4.3KB of RAM — trivial for the ESP32's 520KB). Failed sends are appended to the buffer; a separate 5-second timer attempts to flush it, oldest-first, whenever WiFi is connected. On buffer overflow, the oldest entry is overwritten rather than refusing new data, since recent readings are generally more valuable than very old ones during an extended outage.

This was verified twice:

- **Manually stopping the Flask server** mid-run: readings buffered correctly during the outage, then flushed in a tight burst the instant the server came back, with zero data loss.
- **A real WiFi drop** (killing the mobile hotspot): this produced a more realistic, two-phase failure than a clean kill/restore. First, full loss of WiFi association — readings buffered silently, no HTTP attempt even made. Then, after the hotspot came back, the ESP32 reconnected before the PC did (which had briefly failed over to a different WiFi network), so the ESP32 reported "WiFi connected" but POSTs failed with "connection refused" since the server wasn't reachable on that network yet. Once the PC rejoined the hotspot, all pending buffered readings flushed correctly. Eight readings were affected across the outage; none were lost. This showed that the buffering design didn't need to special-case different failure causes — it treats any send failure identically.

## Multi-Hour Run and Results

The system ran unattended for **3 hours, 13 minutes, 50 seconds**, logging **1164 readings** with **zero NaN failures and zero gaps between readings exceeding 15 seconds** (max observed gap: 11.2s, within normal jitter of the 10s interval).

- **Temperature:** 33.4°C – 34.0°C, rising slowly and steadily across the run. This is consistent with self-heating from the ESP32 and nearby powered electronics rather than a real ambient temperature change, since the rise was smooth and monotonic rather than tracking any obvious environmental event.
- **Humidity:** 59.0% – 65.6%, showing a longer, independent oscillation — a dip early in the run, a rise to a local peak around the 1h40m mark, a second dip, then a steady climb through the end of the run. This pattern was decoupled from the temperature trend, suggesting a different physical driver (air movement, ambient moisture) rather than a temperature-coupled effect.
- A single isolated reading of 33.9°C appeared mid-run surrounded by 33.8°C readings on both sides — a one-sample sensor noise spike, not a sustained event, and not a failure (no NaN was triggered).

A chart of both variables against real wall-clock time (`received_at`) was generated with matplotlib and is included as a separate deliverable.

## Conclusion

The system correctly interfaces with the DHT22 over its open-drain protocol, demonstrates both of its primary failure modes (floating input, over-polling) with distinct and informative results, logs data reliably over WiFi to a local server, and recovers cleanly from both simulated and real network outages without data loss. The 3+ hour zero-failure run and the two WiFi-drop sub-phases together validate the system under both normal and adverse conditions.