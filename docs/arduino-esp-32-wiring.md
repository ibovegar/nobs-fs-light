# Wiring Map: Which Wire Goes Where

This table tells you which pin on the board each wire connects to, for the **Arduino Nano ESP32**.

This box has **6 rotary encoders** (ENC1–ENC6, each with an integrated push button) and **2 toggle
switches** (SW1–SW2, 2-position ON-OFF). Every encoder has three signal terminals (Phase A, Phase
B, and its push-button switch terminal) plus a common terminal; every toggle switch has one signal
terminal plus a common terminal. All the common terminals go to **GND**.

**How to read a row:** find the control (e.g. "ENC1"), look at which terminal it is ("Phase A
Terminal"), then run a wire from that terminal to the pin listed in the board's column. Pin names
like `D13` or `A0` are printed on the board itself. Every control's common terminal connects to
**GND** (ground), see the last row.

The "Virtual HID Button" column is just for reference: it's the button number the sim sees when you
turn, push, or flip that control. You don't wire anything for it.

> 💡 Each terminal has one wire to its signal pin (from this table) and the common terminal wires
> to **GND**. No resistors or extra parts needed; the firmware handles that.

| Component Group | Component Label | Hardware Connection | Arduino Nano ESP32 Pin | Virtual HID Button | Action Type |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **ENC1** | Encoder 1 | Phase A Terminal | **A0** | Button 1 | CW Pulse |
| | Encoder 1 | Phase B Terminal | **A1** | Button 2 | CCW Pulse |
| | Encoder 1 | Switch Terminal | **A2** | Button 3 | Push |
| **ENC2** | Encoder 2 | Phase A Terminal | **A3** | Button 4 | CW Pulse |
| | Encoder 2 | Phase B Terminal | **A4** | Button 5 | CCW Pulse |
| | Encoder 2 | Switch Terminal | **A5** | Button 6 | Push |
| **ENC3** | Encoder 3 | Phase A Terminal | **A6** | Button 7 | CW Pulse |
| | Encoder 3 | Phase B Terminal | **A7** | Button 8 | CCW Pulse |
| | Encoder 3 | Switch Terminal | **D12** | Button 9 | Push |
| **ENC4** | Encoder 4 | Phase A Terminal | **D11** | Button 10 | CW Pulse |
| | Encoder 4 | Phase B Terminal | **D10** | Button 11 | CCW Pulse |
| | Encoder 4 | Switch Terminal | **D9** | Button 12 | Push |
| **ENC5** | Encoder 5 | Phase A Terminal | **D8** | Button 13 | CW Pulse |
| | Encoder 5 | Phase B Terminal | **D7** | Button 14 | CCW Pulse |
| | Encoder 5 | Switch Terminal | **D6** | Button 15 | Push |
| **ENC6** | Encoder 6 | Phase A Terminal | **D5** | Button 16 | CW Pulse |
| | Encoder 6 | Phase B Terminal | **D3** | Button 17 | CCW Pulse |
| | Encoder 6 | Switch Terminal | **D2** | Button 18 | Push |
| **Switches** | Switch 1 (SW1) | Signal Terminal | **D1** | Button 19 | Toggle |
| | Switch 2 (SW2) | Signal Terminal | **D0** | Button 20 | Toggle |
| **Ground Loop** | All Components | Common / GND Terminals | **GND** | *None* | System Ground |
| **Status LED** | Status LED | Anode, via 120 Ω resistor | **D4** | *None* | Boot/Connect Status |
| | Status LED | Cathode | **GND** | *None* | Boot/Connect Status |

> 💡 **Status LED:** wired with a 120 Ω current-limiting resistor from **D4** to **GND**. It blinks
> while the board is booting/waiting for USB enumeration, and lights steady once the host PC has
> enumerated it.

> 💡 **D0/D1 as plain GPIO:** these pins double as the hardware UART (Serial) on most Arduino
> boards, but this firmware disables "USB CDC On Boot" (see
> [`firmware/arduino_eps32_nano/build_opt.h`](../firmware/arduino_eps32_nano/build_opt.h)) and never
> calls `Serial.begin()`, so they're free for plain digital input. This is the same trick
> [Nobs Autopilot](https://github.com/ibovegar/nobs-fs-autopilot) uses for its 4th encoder.

> 💡 **D13 is deliberately unused.** On the Nano ESP32, D13 doubles as the built-in amber LED
> (`LED_BUILTIN` / GPIO48); wiring a control to it would leave that LED lit whenever the firmware
> runs. Every other pin on the board (`D0`–`D12`, `A0`–`A7`) is used above, so D13 is left free as
> spare.
