# Build Instructions

> 📸 This module hasn't been physically built yet, so unlike the other Nobs repos this guide has
> no step photos. The sequence below follows the same two-stage process (controller board, then
> enclosure) as [Nobs Panel](https://github.com/ibovegar/nobs-fs-panel/blob/main/docs/build-instructions.md)
> and [Nobs Autopilot](https://github.com/ibovegar/nobs-fs-autopilot/blob/main/docs/build-instructions.md).
> Photos will be added here once a first unit is assembled.

Before starting, gather everything on the [bill of materials](bill-of-materials.md) and have the
[wiring map](arduino-esp-32-wiring.md) open for reference.

## 1. Controller board

1. Solder the two 26-position female pin header sockets to the stripboard, spaced to match the
   Arduino Nano ESP32's own header pitch, then seat the Arduino into them.
2. Solder the 4 six-position terminal blocks to the stripboard, giving you screw terminals for the
   20 signal wires (6 encoders × 3 terminals + 2 switches × 1 terminal) plus a shared ground rail.
3. Wire the 120 Ω resistor in series with the status LED's anode, and run the LED's anode/cathode
   to **D4** / **GND** as described in the wiring map.
4. Double-check every connection against [arduino-esp-32-wiring.md](arduino-esp-32-wiring.md)
   before applying USB power. A multimeter continuity check across the ground loop catches most
   wiring mistakes early.

## 2. Encoders and switches

1. Print the mounting plate ([`models/nobs_light_mounting_plate.stl`](../models/nobs_light_mounting_plate.stl))
   and mount the 6 rotary encoders and 2 toggle switches to it.
2. Each encoder has 5 terminals: Phase A, Phase B, the push-switch's two terminals (bridge them or
   wire one to signal and the other to the shared ground rail, per the encoder's datasheet), and
   the encoder body/shaft ground tab if present. Wire Phase A, Phase B, and the push switch's
   signal terminal to their pins per the wiring map; wire every remaining common terminal to GND.
3. Each toggle switch has 2 terminals: wire one to its signal pin and the other to GND.
4. Insulate each soldered joint with heat-shrink tubing.
5. Press the knurled knob caps onto the encoder shafts and the bat-lever caps onto the toggle
   switches.

## 3. Firmware

Flash the sketch in [`firmware/arduino_eps32_nano/`](../firmware/arduino_eps32_nano/) following
that folder's [README](../firmware/arduino_eps32_nano/README.md). Do this before final enclosure
assembly so you can test every control (encoder rotation, encoder push, both switches) while
everything is still accessible.

## 4. Enclosure

1. Press the M4 and M3 heat-set inserts into the enclosure top/bottom, mounting plate, and front
   plate with a soldering iron or dedicated insert tool.
2. Fasten the controller board to its standoffs in the enclosure bottom with the M3 pan-head
   screws.
3. Fasten the mounting plate (with encoders and switches already installed) to the rear of the
   enclosure bottom with the M4 × 10 mm screws.
4. Fasten the front plate to the mounting plate assembly with the M3 countersunk screws.
5. Route the USB-C cable out through the enclosure's cutout, then close the top and bottom halves
   with the M4 × 30 mm screws.

## 5. Check it works

Follow [Check it works](../firmware/arduino_eps32_nano/README.md#check-it-works) in the firmware
README: confirm all 6 encoders register CW/CCW/push and both switches register on/off, either in
the Nobs app or Windows' **Set up USB game controllers** panel.
