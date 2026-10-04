# Nobs FS Light

A build-it-yourself control panel for flight simulators: **6 rotary encoders** and **2 toggle
switches**, wired to an **Arduino Nano ESP32**. It plugs in over USB and shows up as a standard
game controller (HID gamepad) named **"Nobs FS Light"**, recognised directly by MSFS and the Nobs
app with no drivers to install.

Same physical layout as [Nobs Panel](https://github.com/ibovegar/nobs-fs-panel) (an 8-position
front panel), but 6 of the 8 toggle-switch positions are rotary encoders instead: each has a
push button and reports CW/CCW rotation as momentary presses, for **22 buttons** total (6 × 3 for
the encoders + 2 × 2 for the three-position switches).

## Design Philosophy

**This panel pairs precise, continuous controls with tactile, no-fuss toggles**: rotary encoders
for values you dial in (headings, altitudes, frequencies, anything you'd otherwise nudge one
degree/unit at a time), plain mechanical toggles for the handful of binary states that don't need
a knob. Both are things you can operate by feel alone, no looking needed, which keeps this
VR-friendly, and because it enumerates as a plain USB game controller, there's nothing to install
and nothing that can fall out of sync with a sim update:

* **Encoders for dialing, not clicking:** turning a knob to set a value is more natural (and more
  precise, with acceleration for fast spins) than repeatedly pressing a button.
* **Physical Switches, Not Software:** the 2 toggle switches are genuine switches with a positive
  snap, so flipping one is unambiguous even without looking.
* **Driver-Free by Design:** the board presents itself as a standard HID gamepad, so MSFS and the
  Nobs app recognise it the moment it's plugged in.

> 📸 This module is a fresh design: the enclosure, mounting plate, and front panel are modeled
> ([models/](models/)), but it hasn't been physically built yet, so there are no product photos
> here. See [docs/build-instructions.md](docs/build-instructions.md).

## Docs

- **[Build instructions](docs/build-instructions.md)**: step-by-step assembly (no photos yet, see
  above).
- **[Which wire goes where](docs/arduino-esp-32-wiring.md)**: the button + pin map.
- **[Loading the firmware](firmware/arduino_eps32_nano/README.md)**: first-time flashing,
  re-flashing, and the encoder acceleration protocol, step by step.
- **[Setting the device ID & name](docs/board-identity.md)**: how the board names itself, how to
  rename it, and how to run several panels at once (each gets its own ID + name).
- **[Bill of materials](docs/bill-of-materials.md)**: parts list.

## Nobs FS Companion App

The [**Nobs FS app**](https://github.com/ibovegar/nobs-fs-app) is the companion application for
communicating with and configuring the Nobs family of panels, including this one. It automatically
detects Nobs devices by their USB identity (VID `303A`, per-product PID block), so the right
device is selected even when other game controllers are connected.

Use it to:
* **Verify wiring & test inputs:** watch every encoder and switch register live as you turn or
  flip it, handy for confirming the build before binding anything in the sim.
* **Tune encoder feel:** adjust each encoder's acceleration sensitivity from the Settings page.
* **Track multiple panels:** the Devices page lets you add extra instances of the panel, each with
  its own ID and name (see [Setting the device ID & name](#setting-the-device-id--name-in-brief)
  below), so the app and the sim can tell them apart.

See the app repository for installation and usage details: <https://github.com/ibovegar/nobs-fs-app>

## Setting the device ID & name (in brief)

The board's name and USB product ID aren't compiled in; they're stored on the board, so the same
firmware can be set up as any Nobs profile. Out of the box this is **"Nobs FS Light"** (`303A` /
`80FC`). To change it, the configuration app sends a single line over the board's serial port:

```
SET_ID:80FC:Nobs FS Light
```

The board saves the new name + ID, replies `OK:80FC:Nobs FS Light`, and reboots so it takes effect
(`GET_ID` reads back the current values). For multiple panels, give each one the next ID in the
block, e.g. `SET_ID:80FD:Nobs FS Light 2`. Full details, including the Windows name-cache refresh,
are in **[docs/board-identity.md](docs/board-identity.md)**.
