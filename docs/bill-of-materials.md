# Bill of Materials (Shopping List)

This is the full shopping list for building one Nobs FS Light: every part you need, from the
electronics to the screws. The "Manufacturer / Part Number" column lists parts known to work,
linked to their Mouser listing, so you can search for them or find equivalents.

A few friendly notes before you start buying:

- **Exact parts aren't mandatory.** The encoders, switches, terminal blocks, and stripboard can be
  swapped for similar parts you like or already have. The part numbers here are just what's known
  to work.
- **The enclosure parts (section 2) are 3D printed**: the top/bottom shells are shared with the
  rest of the Nobs family, and the mounting plate and front plate are modeled for this module's
  6-encoder / 2-switch layout. Print them yourself from the files in the `models/` folder, or have
  a print service make them.
- Quantities are for **one** panel.

## Core Electronics & Switches

| Item # | Qty | Component Description | Manufacturer / Part Number | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **1.1** | 1 | Arduino Nano ESP32 (with headers) | [Arduino / ABX00083](https://no.mouser.com/ProductDetail/Arduino/ABX00083) | Main microcontroller (ESP32-S3, USB HID, VID 0x303A / PID 0x80FC) |
| **1.2** | 6 | Rotary Encoder, 12 mm, with Momentary Push Switch, 24 detents | [Bourns / PEC11R-4215F-S0024](https://www.mouser.com/ProductDetail/Bourns/PEC11R-4215F-S0024) | ENC1–ENC6 |
| **1.3** | 2 | SPDT Toggle Switch, ON-OFF-ON (3-position) | *Part number to be chosen* | SW1–SW2 |
| **1.4** | 1 | Stripboard, 50 × 80 mm | [BusBoard Prototype Systems / ST1](https://no.mouser.com/ProductDetail/BusBoard-Prototype-Systems/ST1) | Controller board carrier for the Arduino, pin headers, and terminal blocks |
| **1.5** | 2 | Female Pin Header Socket, 26-position, 2.54 mm pitch | [3M Electronic Solutions Division / 929850-01-26-RA](https://no.mouser.com/ProductDetail/3M-Electronic-Solutions-Division/929850-01-26-RA) | Sockets soldered to the stripboard to seat the Arduino Nano ESP32 |
| **1.6** | 4 | Fixed Terminal Block, 6-position, 2.54 mm pitch | [GCT / TBC05-06-1-G-G](https://no.mouser.com/ProductDetail/GCT/TBC05-06-1-G-G) | Screw-down wiring for the 22 encoder/switch signal wires and ground |
| **1.7** | 1 | 3 mm Green LED, Diffused, 2.2 V | [Lumex / SSL-LX30FT4GD](https://no.mouser.com/ProductDetail/Lumex/SSL-LX30FT4GD?qs=7jmq9uQgtYG4HR2h%252BlgbMw%3D%3D) | Front-plate status indicator |
| **1.8** | 1 | 120 Ω Resistor, 1/4 W, ±1%, Metal Film | [KOA Speer / MFS1/4DCT52R1200F](https://no.mouser.com/ProductDetail/KOA-Speer/MFS1-4DCT52R1200F?qs=ddCg%252BR5cWn20oJHZLuYO%2FQ%3D%3D) | Current-limiting resistor for the status LED |
| **1.9** | 6 | Knurled Encoder Knob, 6 mm D-shaft | Generic aftermarket | Grip/cap for ENC1–ENC6 |
| **1.10** | 2 | Toggle Switch Cap, Bat Lever, White | [NKK Switches / AT434B](https://no.mouser.com/ProductDetail/NKK-Switches/AT434B?qs=O05gvr8fLT2J5h7R71L7Aw%3D%3D) | Color-coded cap for toggle switches |

## Structural & Enclosure Hardware

| Item # | Qty | Component Description | Manufacturer / Part Number | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **2.1** | 1 | Enclosure Top | 3D Printed (`models/enclosure_top.stl`) | Main upper housing shell |
| **2.2** | 1 | Enclosure Bottom | 3D Printed (`models/enclosure_bottom.stl`) | Main lower housing base, carries the controller board |
| **2.3** | 1 | Mounting Plate | 3D Printed (`models/nobs_light_mounting_plate.stl`) | Carries the 6 encoders and 2 toggle switches |
| **2.4** | 1 | Front Plate | 3D Printed (`models/nobs_light_frontpanel.stl`) | Visual faceplate with control labels |
| **2.5** | 10 | M4 Heat-Set Insert | [SI (PennEngineering) / IUTB-M4](https://no.mouser.com/ProductDetail/SI/IUTB-M4) | Threaded anchors moulded into the enclosure top/bottom |
| **2.6** | 11 | M3 Heat-Set Insert | [SI (PennEngineering) / IUTB-M3](https://no.mouser.com/ProductDetail/SI/IUTB-M3) | Threaded anchors moulded into the mounting plate and front plate |
| **2.7** | 4 | M4 × 0.7, 30 mm Socket-Head Screw | — | Clamps the enclosure top and bottom halves together |
| **2.8** | 6 | M4 × 0.7, 10 mm Socket-Head Screw | — | Fastens the mounting plate to the rear of the enclosure bottom |
| **2.9** | 4 | M3 × 0.5, 8 mm Pan-Head Screw | — | Fastens the controller board to its standoffs in the enclosure bottom |
| **2.10** | 7 | M3 × 0.5, 8 mm Countersunk Screw | — | Fastens the front plate to the mounting plate, and the assembly to the enclosure |

> ⚠️ Section 2 quantities are carried over from the other Nobs modules as a starting estimate; treat
> them as a first pass until a build confirms exact fastener counts for this module's parts.

## Connectors & Wiring

| Item # | Qty | Component Description | Manufacturer / Part Number | Purpose / Application |
| :--- | :--- | :--- | :--- | :--- |
| **3.1** | 1 | USB-C to USB-A Cable, 1.5 m to 2 m, data-sync capable | — | Connects the Arduino Nano ESP32 to your PC |
| **3.2** | 1 | Heat Shrink Tubing Assortment Pack, 1.5 mm to 3.5 mm diameters | — | Insulates soldered connections on the encoder and switch terminals |

## Tools Recommended for Assembly (Not in BOM Cost)

* **Soldering Iron & Lead-Free Rosin-Core Solder:** for attaching wires to the encoder/switch
  terminals and seating the pin headers/terminal blocks on the stripboard.
* **Wire Strippers & Flush Cutters:** for prepping the signal and ground wires.
* **Soldering Iron / Insert Tool for Heat-Set Inserts:** for pressing the brass inserts squarely
  into the 3D-printed parts.
* **Multimeter:** for testing continuity and checking for short circuits across the common ground
  loop before plugging into USB power.

## Project Costs

Rough cost estimate for a single panel, in USD. These are ballpark figures only, actual prices
vary significantly by supplier, region, and quantity, and **exclude shipping and taxes**. Several
items (heat-set inserts, screws) are sold in multi-build packs, so the per-build cost is lower if
you build more than one or already have them on hand.

| Category | Items | Estimated Cost |
| :--- | :--- | ---: |
| Microcontroller | Arduino Nano ESP32 (genuine; clones are cheaper) | $20 – $28 |
| Encoders & switches | 6 × Bourns PEC11R-4215F-S0024 + 2 × 3-position toggle switches | $30 – $50 |
| Board & connectors | Stripboard, 2 × pin headers, 4 × terminal blocks, status LED | $12 – $18 |
| Fasteners | Heat-set inserts (21) and M3/M4 screws (21) | $10 – $18 |
| Enclosure (3D printed) | Filament for top, bottom, mounting plate, front plate | $5 – $15 |
| Connectors & wiring | USB-C cable, heat-shrink tubing pack, encoder knobs | $10 – $20 |
| **Total** | | **≈ $87 – $149** |

> **Notes:**
> * The **enclosure** assumes you print the parts yourself (filament cost only). Ordering them
>   from a print service will add roughly $20 – $50.
> * This estimate doesn't include hookup wire, since it's usually bought once in bulk and reused
>   across builds.
> * Bulk/pack purchasing of the inserts, screws, and heat-shrink tubing spreads their cost across
>   multiple builds, lowering the effective per-panel total.
