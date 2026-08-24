# Changelog

All notable changes to the Nobs FS Light project (hardware, firmware, and documentation) are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Project scaffold, modeled on [nobs-fs-panel](https://github.com/ibovegar/nobs-fs-panel): same
  folder layout, `.gitignore`, and AGPLv3 license.
- Wiring map ([docs/arduino-esp-32-wiring.md](docs/arduino-esp-32-wiring.md)): 6 encoders (18
  pins) + 2 toggle switches (2 pins) + status LED, all fit onto the Arduino Nano ESP32's pins.
- Bill of Materials ([docs/bill-of-materials.md](docs/bill-of-materials.md)): core electronics,
  encoders, switches, structural/enclosure hardware (pending final mounting-plate design), and a
  per-panel project cost estimate.
- Build instructions ([docs/build-instructions.md](docs/build-instructions.md)): textual walkthrough
  covering controller board, encoders/switches, firmware, and enclosure; no step photos yet since
  the module hasn't been physically built.
- Board identity guide ([docs/board-identity.md](docs/board-identity.md)), reserving PID block
  `80FC`–`80FF` / "Nobs FS Light" in the shared Nobs USB ID scheme (after Panel `80F0`–`80F3`,
  Autopilot `80F4`–`80F7`, Approach `80F8`–`80FB`).
- Arduino Nano ESP32 firmware sketch
  ([firmware/arduino_eps32_nano/arduino_eps32_nano.ino](firmware/arduino_eps32_nano/arduino_eps32_nano.ino)):
  6-encoder quadrature decoding, per-encoder acceleration, and dynamic USB identity, adapted from
  [nobs-fs-autopilot](https://github.com/ibovegar/nobs-fs-autopilot)'s proven encoder code, plus 2
  single-pin toggle switches.
- Enclosure, mounting plate, and front panel CAD
  ([models/](models/): shared `enclosure_top`/`enclosure_bottom` shells plus new
  `nobs_light_mounting_plate.stl` and `nobs_light_frontpanel.stl` for the 6-encoder / 2-switch
  layout).
- `images/` placeholder, pending a first physical build.

[Unreleased]: https://github.com/ibovegar/nobs-fs-light/commits/main
