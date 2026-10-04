# Wiring Map: Which Wire Goes Where

This table tells you which pin on the board each wire connects to, for the **Arduino Nano ESP32**.

This box has **6 rotary encoders** (ENC1–ENC6, each with an integrated push button) and **2 toggle
switches** (SW1–SW2, 3-position ON-OFF-ON). Every encoder has three signal terminals (Phase A,
Phase B, and its push-button switch terminal) plus a common terminal; every toggle switch has two
signal terminals (1 and 3) plus a common terminal (2). All the common terminals go to **GND**.

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
| **ENC1** | Encoder 1 | Phase A Terminal | **D12** | Button 1 | CW Pulse |
| | Encoder 1 | Phase B Terminal | **D11** | Button 2 | CCW Pulse |
| | Encoder 1 | Switch Terminal | **D10** | Button 3 | Push |
| **ENC2** | Encoder 2 | Phase A Terminal | **D9** | Button 4 | CW Pulse |
| | Encoder 2 | Phase B Terminal | **D8** | Button 5 | CCW Pulse |
| | Encoder 2 | Switch Terminal | **D7** | Button 6 | Push |
| **ENC3** | Encoder 3 | Phase A Terminal | **D6** | Button 7 | CW Pulse |
| | Encoder 3 | Phase B Terminal | **D5** | Button 8 | CCW Pulse |
| | Encoder 3 | Switch Terminal | **D4** | Button 9 | Push |
| **ENC4** | Encoder 4 | Phase A Terminal | **D3** | Button 10 | CW Pulse |
| | Encoder 4 | Phase B Terminal | **D2** | Button 11 | CCW Pulse |
| | Encoder 4 | Switch Terminal | **D0** | Button 12 | Push |
| **ENC5** | Encoder 5 | Phase A Terminal | **D13** | Button 13 | CW Pulse |
| | Encoder 5 | Phase B Terminal | **D17 (A0)** | Button 14 | CCW Pulse |
| | Encoder 5 | Switch Terminal | **D18 (A1)** | Button 15 | Push |
| **ENC6** | Encoder 6 | Phase A Terminal | **D19 (A2)** | Button 16 | CW Pulse |
| | Encoder 6 | Phase B Terminal | **D20 (A3)** | Button 17 | CCW Pulse |
| | Encoder 6 | Switch Terminal | **D21 (A4)** | Button 18 | Push |
| **Switches** | Switch 1 (SW1) | Terminal 1 | **D24 (A7)** | Button 21 | Toggle (position 1) |
| | Switch 1 (SW1) | Terminal 3 | **D1** | Button 22 | Toggle (position 3) |
| | Switch 2 (SW2) | Terminal 1 | **D22 (A5)** | Button 19 | Toggle (position 1) |
| | Switch 2 (SW2) | Terminal 3 | **D23 (A6)** | Button 20 | Toggle (position 3) |
| **Ground Loop** | All Components | Common / GND Terminals (incl. SW terminal 2) | **GND** | *None* | System Ground |
| **Status LED** | Status LED | Anode, via 120 Ω resistor | **B0** | *None* | Boot/Connect Status |
| | Status LED | Cathode | **GND** | *None* | Boot/Connect Status |

> 💡 **Status LED:** wired with a 120 Ω current-limiting resistor from **B0** to **GND**. It blinks
> while the board is booting/waiting for USB enumeration and lights steady once the host PC has
> enumerated it. If no host shows up within 10 seconds it goes dark. B0 also drives the red
> channel of the board's own RGB LED (inverted), so that one glows red whenever the status LED is
> off. Leave **B1** (next to it) unconnected: it selects the chip's boot mode, and pulling it to
> ground stops the board from starting.

> 💡 **Three-position switches:** SW1 and SW2 are ON-OFF-ON toggles. Terminal 2 (centre) goes to
> **GND**; flipping to one side connects terminal 2 to terminal 1, the other side to terminal 3,
> and the centre position connects neither (both buttons released).

> 💡 **D17–D24 are the same pins as A0–A7.** Both labels are printed on the board.

> 💡 **D0/D1 as plain GPIO:** these pins double as the hardware UART (Serial) on most Arduino
> boards, but this firmware disables "USB CDC On Boot" (see
> [`firmware/arduino_eps32_nano/build_opt.h`](../firmware/arduino_eps32_nano/build_opt.h)) and never
> calls `Serial.begin()`, so they're free for plain digital input.

> 💡 **D13 is the built-in LED pin.** On the Nano ESP32, D13 doubles as the built-in amber LED
> (`LED_BUILTIN` / GPIO48), so that LED stays lit while the firmware runs. Every `D` and `A` pin on
> the board is used, which is why the status LED sits on **B0**.
