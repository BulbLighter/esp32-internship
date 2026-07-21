```markdown
# Project 1: Button + LED Debounce Counter — Write-Up

## Objective
Build a reliable button press counter on ESP32 that displays the count via LED
blinks and Serial Monitor output, understanding each stage electrically rather
than just in code.

## Hardware Used
- ESP32 DevKit V1
- Tactile push button
- 10kΩ resistor (pull-up/pull-down)
- 100nF ceramic capacitor (RC debounce)
- 330Ω resistor (LED current limiting)
- LED
- Breadboard, jumper wires, DT9205A multimeter

---

## 1. Floating Input

Button wired to GPIO 22 with no resistor of any kind — the other side of the
button left unconnected. Serial Monitor showed the pin drifting between 0 and
1 on its own, with no correlation to actual button presses.

**Why:** A disconnected input pin is high-impedance and draws almost no
current, so it has no defined voltage. Ambient electromagnetic noise and the
pin's own internal capacitance are enough to push the read voltage above or
below the logic threshold randomly. This is not a code fault — it's a direct
consequence of leaving an input pin without a resistor path to a known
voltage rail.

---

## 2. Pull-Down vs Pull-Up

**External pull-down (10kΩ):** Resistor from GPIO 22 to GND, button connects
GPIO 22 to 3.3V when pressed. Result: pin reads stable LOW at rest, stable
HIGH when pressed.

**External pull-up (10kΩ):** Resistor from GPIO 22 to 3.3V, button connects
GPIO 22 to GND when pressed. Result: pin reads stable HIGH at rest, stable
LOW when pressed — logic inverted compared to pull-down.

**Internal pull-up (`INPUT_PULLUP`):** Removed the external resistor,
changed `pinMode()` to `INPUT_PULLUP`. Identical behavior to the external
pull-up, using the ESP32's built-in ~45kΩ internal resistor instead of a
physical component. Confirms the ESP32 has this resistor available on-chip,
trading a slightly weaker pull for one less part on the board.

In all three configurations, the previously random floating behavior
disappeared entirely — the pin held a rock-solid, repeatable value at rest.

---

## 3. Switch Bounce

Using the pull-up wiring, added a raw press counter with no debounce logic —
counted every HIGH→LOW transition directly.

**Result:** 5 deliberate, slow button presses registered as 13 counts.

**Why:** The button's physical metal contacts bounce against each other for
a few milliseconds during each press, producing several rapid open/close
transitions instead of one clean transition. The ESP32 reads fast enough to
catch every one of these bounces as a separate press.

---

## 4. Hardware RC Debounce

Added a 100nF ceramic capacitor in parallel with the button (from the GPIO
22 node to GND), keeping the external 10kΩ pull-up.

**RC time constant calculation:**
```
τ = R × C = 10,000Ω × 0.0000001F = 0.001s = 1ms
Settling time (3τ–5τ) ≈ 3ms–5ms
```

The capacitor resists sudden voltage changes, smoothing out the fast bounce
transitions before the GPIO pin ever sees them.

**Result:** 10 deliberate presses → exactly 10 counts. No software
debounce logic involved — the fix happens entirely at the electrical signal
level.

**Note:** the first button tested gave inconsistent results even with the
capacitor correctly wired. Switching to a second physical button resolved
it — bounce severity varies between individual buttons, and not all cheap
tactile switches behave the same.

---

## 5. Software Debounce

Removed the capacitor and external resistor, reverted to `INPUT_PULLUP`.
Implemented a `millis()`-based debounce: a transition is only accepted if at
least 50ms has passed since the last accepted transition.

`millis()` was used instead of `delay()` because `delay()` blocks the entire
processor for its duration — fine for a single-purpose test, but unusable in
any project where the ESP32 needs to do more than one thing at once.
`millis()` allows the debounce timing check to happen without halting
execution.

**Result:** 15 deliberate presses → exactly 15 counts.

Both hardware and software debounce solved the identical bounce problem
through different mechanisms — hardware filters the signal before the pin
reads it, software filters the readings after, in logic.

---

## 6. LED Current-Limiting Resistor

**Calculation** (ESP32 GPIO output, 3.3V, red LED, target ~15mA):
```
R = (V_supply - V_LED) / I_LED
R = (3.3V - 2.0V) / 0.015A
R = 86.7Ω  (minimum safe resistor)
```

Using the available 330Ω resistor instead of the calculated minimum:
```
I = (3.3V - 2.0V) / 330Ω = 3.94mA
```

330Ω is well above the minimum required, resulting in lower brightness than
the 15mA target but a safe, conservative current for both the LED and the
GPIO pin.

---

## 7. 3.3V Logic Levels

The ESP32's GPIO pins operate at 3.3V and are **not 5V tolerant**. All
supply and signal connections in this project (button pull-up/down, LED
driver) were kept at 3.3V throughout. Connecting a 5V signal directly to a
GPIO pin would exceed its rated input voltage and risk permanently damaging
the pin or the chip — this was avoided at every stage by sourcing all logic
signals from the ESP32's own 3.3V rail rather than any external 5V source.

---

## 8. Final Integration

Combined hardware RC debounce (10kΩ + 100nF on GPIO 22) with an LED output
circuit on GPIO 23 (330Ω resistor + LED to GND). Each debounced press
increments a counter, prints it to Serial Monitor, and blinks the LED that
many times.

**Result:** 16 deliberate presses → 16 counts, LED blinked correctly each
time.

**Problem faced during integration:** the LED did not light initially,
despite the resistor value, LED polarity, and counter logic all checking out
individually correct. Continuity testing traced the fault to the LED's anode
and cathode both being placed in the same connected breadboard row segment.
Since all holes in that row segment are electrically identical, this shorted
the LED out — current flowed straight from the resistor to GND across the
row, bypassing the LED entirely. Fixed by moving the cathode to a separate,
unconnected row.

This was the main lesson of the project: individually correct components do
not guarantee a correct circuit — every junction has to be verified as an
actual physical connection, not assumed from the wiring diagram.

---

## Component Roles Summary

| Component | Role | If removed |
|---|---|---|
| 10kΩ pull-up/down resistor | Defines a stable resting voltage on the input pin | Pin floats, random readings return |
| 100nF capacitor | Smooths rapid bounce transitions via RC filtering | Bounce returns, overcounting resumes |
| 330Ω resistor (LED) | Limits LED current to a safe level | Risk of damage to LED and/or GPIO pin from excess current |

---

## Results Summary

| Condition | Presses | Counted |
|---|---|---|
| No resistor (floating) | — | Random, unusable |
| Pull-down / pull-up (external + internal) | — | Stable, correct logic level |
| No debounce | 5 | 13 |
| Hardware RC debounce | 10 | 10 |
| Software `millis()` debounce | 15 | 15 |
| Final integrated (debounce + LED) | 16 | 16 |

## Conclusion
All stages specified in the project brief were built and verified on
hardware: floating input behavior, pull-down and pull-up (external and
internal), bounce demonstration, both debounce methods with calculated RC
time constant, LED resistor calculation, and adherence to 3.3V logic levels
throughout. Final firmware combines hardware RC debounce with LED blink
output, demonstrating clean single-increment counting.
```