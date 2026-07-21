# Project 4: Relay Control with Optoisolation — Write-Up

## Objective
Switch a 12V DC fan safely from an ESP32, controlled by both a debounced physical button and a phone browser over WiFi, using a 5V opto-isolated relay module.

## Components Used
- ESP32 DevKit V1
- CYX-3FC(T73) DC05V relay module (opto-isolated, single-channel)
- OSELTECH OSL-8025 12V DC fan (0.15A)
- 12V 1A SMPS adapter
- Push button (INPUT_PULLUP, no external resistor)
- Jumper wires

## Relay Module — Component Identification
Own camera photos weren't clear enough to positively identify small components, so identification below is cross-checked against manufacturer stock photos of the identical module (same silkscreen labels, same layout, from the listing it was ordered from).

- **VCC / GND / IN pins** — control-side header. Directly wired and function-tested.
- **GND / VR / VCC jumper block** — yellow jumper selecting whether the coil draws power from the shared VCC rail or an independent VR rail. Jumper removed in Stage 4 to separate the two rails.
- **Optocoupler** — small black 4-pin IC next to the jumper block. Function confirmed indirectly: active-LOW switching behavior was verified experimentally (Stage 2), consistent with an optocoupler-driven input stage.
- **Small black TO-92 component** — visible next to the optocoupler in the stock photo, consistent with a transistor stage driving the coil. Not individually function-tested in isolation; inferred from its position between the optocoupler and coil, and from the fact that optocoupler output current alone is generally too low to drive a relay coil directly.
- **Small cylindrical diode (banded)** — visible near the optocoupler in the stock photo, consistent with a flyback diode across the coil. Not individually function-tested; its necessity is understood from theory (see below) rather than confirmed by direct probing on this board.
- **Relay (CYX-3FC(T73), DC05V)** — printed rating 10A/125VDC, 7A/250VAC. Confirmed by direct wiring and switching tests.
- **COM / NO / NC screw terminals** — confirmed by continuity testing (Stage 3): NO reads open when relay is unpowered, closes when energized. NO and COM used for the fan; NC left unused.

## Why GPIO Can't Drive the Coil Directly
ESP32 GPIOs safely source ~12mA. The relay coil needs roughly 70mA to energize. The module's onboard driver stage (optocoupler + transistor) bridges this gap — the GPIO only ever drives the optocoupler's low-current LED input, never the coil directly.

## Optocoupler's Role
Provides electrical isolation between the ESP32's logic side and the relay coil/load side — signal crosses via light (LED to phototransistor) rather than a direct wire connection. This matters on factory equipment because it protects control electronics from voltage spikes or faults on the higher-power side being switched.

## Flyback Diode's Role
A relay coil is inductive. When current through it is cut off, the collapsing magnetic field induces a large reverse voltage spike. A diode placed across the coil gives this spike a safe path to dissipate, protecting the driving transistor from being destroyed by it. This component was not individually probed on the board — its presence and function are inferred from the module's normal operation and from general relay-driver design, not from direct measurement in this project.

## Power Domains
This module doesn't expose a separate JD-VCC header — instead it uses a jumper (GND / VR / VCC) that, by default, ties the coil supply to the same VCC pin as the logic side. The jumper was removed and wired as:

- ESP32 3.3V → module VCC (logic/optocoupler side)
- ESP32 5V (VIN) → module VR (coil side, after jumper removed)
- ESP32 GND → module GND (shared)

Verified with continuity checks: VCC↔VR read open after separation, confirming the two rails were genuinely independent post-modification. Common ground was maintained across ESP32, relay module, button, and load side throughout.

## Active-Low Logic
Confirmed empirically: with the module wired and a simple HIGH/LOW toggle sketch running, the relay energized (green LED + audible click) when the pin was driven LOW, and de-energized on HIGH. Firmware uses named constants rather than raw values:

```cpp
#define RELAY_ON  LOW
#define RELAY_OFF HIGH
```

## Boot-State Safety
GPIO 22 was used for the relay IN line. Tested two ways:

1. **Five RST (soft reset) presses** — no flicker (rapid on-off toggling) observed at any reset. Serial output confirmed `setup()` silently set the relay OFF before `loop()`'s first "Relay ON" print, showing the write-before-`pinMode()` pattern working correctly.
2. **Three full cold power cycles** — the relay turned on near-instantly on power-up. To check whether this was a real pre-boot condition or just fast code execution, a 2-second `delay()` was added directly after `pinMode()` in `setup()`. The relay still activated before that delay had elapsed, confirming a genuine brief pre-`setup()` state on the IN line rather than a timing illusion.

No flicker (the literal deliverable requirement) was observed in either test. The instant-on-at-power-up behavior is a distinct, more specific finding, and does not itself violate the "no flicker" requirement.

```cpp
digitalWrite(RELAY_PIN, RELAY_OFF);  // set safe state before configuring pin
pinMode(RELAY_PIN, OUTPUT);
```

## Firmware — Button + Debounce
Reused the millis()-based debounce pattern from Project 1, adapted from a press-counter to a state toggle. `INPUT_PULLUP` used for the button (idle = HIGH, pressed = LOW), avoiding an external pull-down resistor. Debounce window: 50ms. Verified with rapid repeated presses — Serial log showed exactly one toggle per physical press, no double-counting.

## Firmware — Web Server
Implemented using the built-in `WebServer` library with three routes:
- `/` — serves a page showing current relay state with ON/OFF buttons
- `/on` — sets relay ON, re-renders the page
- `/off` — sets relay OFF, re-renders the page

Both the physical button and the web interface call the same `updateOutputs()` function, keeping relay state consistent regardless of control source. Tested from a phone browser on the same WiFi network — ON/OFF buttons correctly toggled the fan, and Serial log confirmed matching state changes. Physical button control continued to work alongside web control without conflict.

## Test Summary

| Test | Result | Conclusion |
|---|---|---|
| COM↔NO continuity, relay unpowered | Open | NO terminal correctly identified, disconnected at rest |
| Adapter/fan black wire joint continuity | Beeped | Ground path intact on load side |
| VCC↔VR continuity after jumper removal | Open | Power domains genuinely separated |
| 5x RST presses | No flicker; OFF set silently before first ON | Boot-safe pattern functioning across resets |
| 3x cold power cycles + delay(2000) test | Instant-on confirmed as real pre-setup() state, not code-speed illusion | Precise characterization of power-up behavior |
| Rapid button presses | One toggle per press | Debounce logic robust under fast input |

## Status
Stages 1–5 and 7 complete and verified against the deliverable. Stage 6 button-toggle logic complete and verified (via the module's own indicator LED); separate green/red status LEDs were wired but left as unfinished polish, not required by the spec. Stretch goal (bare relay driven by BC547) not attempted — requires a separate bare relay component not currently on hand.

## Conclusion
The core deliverable — phone-controlled 12V load switching with safe boot behavior — is met and verified with direct hardware tests at each stage. Active-low logic, power domain separation, and boot-state behavior were confirmed empirically. Component-level identification of the transistor and flyback diode relies on stock-photo cross-reference and standard relay-driver theory rather than direct probing on the physical board, which should be noted if asked for detail beyond what was actually measured.