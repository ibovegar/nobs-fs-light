// Nobs FS Light — Arduino Nano ESP32 (ESP32-S3) USB HID gamepad firmware.
//
// A lighter sibling of the Nobs Autopilot build: same encoder-decoding approach,
// scaled to 6 encoders, with 2 three-position (ON-OFF-ON) toggle switches standing in
// for the other two boxes' larger switch banks. Exposes 22 buttons in the exact order
// the nobs-fs app expects:
//   buttons  0..17  → 6 encoders × (CW, CCW, push)
//   buttons 18..21  → 2 toggle switches × 2 (SW2 pos1, SW2 pos3, SW1 pos1, SW1 pos3)
// See docs/arduino-esp-32-wiring.md for the full button + pin table.
//
// Host config: over the USB CDC serial port the app sets encoder acceleration
// sensitivity and can reassign the board's USB identity — see the serial protocol
// near handleSerialConfig().
//
// ── Arduino IDE Tools settings (required) ─────────────────────────────────────
//   Board:        "Arduino Nano ESP32"
//   USB Mode:     "Normal mode (TinyUSB)"    ← the default; needed for custom HID
//   Pin Numbers:  "By Arduino pin (default)"  so D0/A0… match the wiring doc
// No other Tools changes and NO edits to the installed Arduino core are needed — the
// whole USB-identity fix lives in this repo (this sketch + build_opt.h beside it).
//
// ── USB identity: dynamic, stored in NVS (Preferences) ────────────────────────
// The device enumerates with VID 0x303A (Espressif's vendor ID) and a PID + product
// name that are *configurable at runtime*. Each Nobs box gets its own PID so MSFS
// can't mirror/overwrite control bindings across panels. Out of the box this build
// defaults to PID 0x80FC / "Nobs FS Light" (matches the `light` entry in the app's
// device registry, src/panel/panel.ts).
//
// The PID and name live in flash via the native Preferences library (NVS). On boot
// the firmware reads them *before* USB.begin() and applies them to the descriptor.
// The configuration app can change them over the USB CDC serial port (see the serial
// protocol below); after a change the firmware reboots so USB re-enumerates.
//
// That only works because the Nano ESP32 normally forces "USB CDC On Boot" and "DFU On
// Boot" on, which would make the core bring USB up at boot (before setup()), freezing
// the descriptor with the stock 0x2341/0x0070 identity. The build_opt.h file next to
// this sketch overrides both to 0 (it is appended to every compile, including the
// core's), so the boot-time USB.begin() is skipped and our runtime identity wins.
//
// Because "CDC On Boot" is then off, the core's `Serial` is the hardware UART — so the
// app's config port is our own USB CDC interface (the USBSerial object below). That
// also frees D0/D1 (normally UART0 TX/RX) for plain digital input, which is how SW1
// gets one of its pins below.
//
// Re-flashing: the running firmware reports a custom PID (and, with DFU-on-boot off, no
// DFU runtime interface), so the IDE can't auto-enter the bootloader. Double-tap the
// RESET button (RGB LED pulses) before uploading. See this folder's README.md.

#include "USB.h"
#include "USBHID.h"
#include "USBCDC.h"
#include <EEPROM.h>
#include <Preferences.h>
#include "soc/usb_serial_jtag_reg.h"

#if ARDUINO_USB_MODE
#error "Set Tools > USB Mode to 'Normal mode (TinyUSB)'. Custom HID needs the TinyUSB stack."
#endif
#if ARDUINO_USB_CDC_ON_BOOT
#error "build_opt.h is not taking effect: ARDUINO_USB_CDC_ON_BOOT must be 0. Make sure build_opt.h sits next to this .ino (it disables CDC/DFU-on-boot so the runtime USB identity works)."
#endif

// ── Custom HID gamepad: 22 buttons, no axes ───────────────────────────────────
// 22 1-bit buttons + 2 bits of padding = a flat 3-byte report, driven through the
// ESP32 core's USBHID device interface (same descriptor shape as Nobs Autopilot).
static const uint8_t HID_REPORT_DESCRIPTOR[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop)
  0x09, 0x05,        // Usage (Game Pad)
  0xA1, 0x01,        // Collection (Application)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (Button 1)
  0x29, 0x16,        //   Usage Maximum (Button 22)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x16,        //   Report Count (22)
  0x81, 0x02,        //   Input (Data,Var,Abs)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x02,        //   Report Count (2)   — padding to a whole byte
  0x81, 0x03,        //   Input (Const,Var,Abs)
  0xC0               // End Collection
};

class NobsGamepad : public USBHIDDevice {
public:
  NobsGamepad() { HID.addDevice(this, sizeof(HID_REPORT_DESCRIPTOR)); }

  void begin() { HID.begin(); }

  // Set/clear one button bit in the pending report (mirrors Joystick.setButton).
  void setButton(uint8_t index, uint8_t value) {
    if (index >= 22) return;
    uint8_t byteIdx = index >> 3;
    uint8_t mask    = 1 << (index & 7);
    if (value) report[byteIdx] |= mask;
    else       report[byteIdx] &= ~mask;
  }

  // One USB report carrying every button change since the last call (mirrors
  // Joystick.sendState). Only transmitted when something actually changed — HID
  // hosts track button state edge-to-edge, so re-sending identical reports just
  // adds USB traffic and risks blocking the loop.
  void sendState() {
    if (memcmp(report, lastSent, sizeof(report)) == 0) return;
    if (HID.SendReport(0, report, sizeof(report))) {
      memcpy(lastSent, report, sizeof(report));
    }
  }

  // USBHIDDevice: hand the descriptor to the core during enumeration.
  uint16_t _onGetDescriptor(uint8_t* buffer) override {
    memcpy(buffer, HID_REPORT_DESCRIPTOR, sizeof(HID_REPORT_DESCRIPTOR));
    return sizeof(HID_REPORT_DESCRIPTOR);
  }

private:
  USBHID  HID;
  uint8_t report[3]   = { 0, 0, 0 };
  uint8_t lastSent[3] = { 0xFF, 0xFF, 0xFF }; // force the first real report out
};

NobsGamepad Joystick;

// USB CDC config port. With CDC-on-boot disabled (via build_opt.h) the core's `Serial`
// is the hardware UART, so we expose our own USB CDC interface for the app's Settings
// page. Like USBHID, USBCDC registers its interface in its constructor (static init),
// so it is in place before USB.begin() builds the composite descriptor in setup().
USBCDC USBSerial;

// ── Dynamic USB identity (NVS via Preferences) ────────────────────────────────
// VID is fixed (Espressif's vendor ID); PID + product name are stored in flash and
// host-configurable. Defaults are the light profile; an unconfigured chip uses them.
// (Acceleration uses the emulated-EEPROM store below; identity uses its own
// Preferences namespace, so the two don't collide.)
// NB: named NOBS_USB_VID, not USB_VID — the core's pins_arduino.h #defines USB_VID.
const uint16_t NOBS_USB_VID  = 0x303A;
const uint16_t DEFAULT_PID   = 0x80FC;
const char*    DEFAULT_NAME  = "Nobs FS Light";
const char*    NVS_NAMESPACE = "nobs";
const char*    NVS_KEY_PID   = "pid";
const char*    NVS_KEY_NAME  = "name";

Preferences prefs;
uint16_t     usbPid;  // active PID (kept alive for the whole session)
String       usbName; // active product name (c_str() must outlive USB.begin())

void loadIdentity() {
  prefs.begin(NVS_NAMESPACE, true); // read-only
  usbPid  = prefs.getUShort(NVS_KEY_PID, DEFAULT_PID);
  usbName = prefs.getString(NVS_KEY_NAME, DEFAULT_NAME);
  prefs.end();
}

void storeIdentity(uint16_t pid, const String& name) {
  prefs.begin(NVS_NAMESPACE, false); // read-write
  prefs.putUShort(NVS_KEY_PID, pid);
  prefs.putString(NVS_KEY_NAME, name);
  prefs.end();
}

// ── USB string descriptors: name the interfaces after the product ─────────────
// The core hardcodes the HID/CDC *interface* strings to "TinyUSB HID"/"TinyUSB CDC".
// For a composite device (HID + CDC) Windows and MSFS show that interface string as
// the controller's name — so without this it shows up as "TinyUSB HID" even though
// iProduct is "Nobs FS Light". tud_descriptor_string_cb is weak in the core, so
// defining it here replaces it (no core edit needed) and points every interface
// string at the product name. Index map: 0=language, 1=manufacturer, 2=product,
// 3=serial (from the chip MAC, stable per board), 4+=interface strings → product.
extern "C" const uint16_t* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void)langid;
  static uint16_t desc[40];

  if (index == 0) {
    desc[1] = 0x0409; // English (United States)
    desc[0] = (uint16_t)((3 << 8) | (2 * 1 + 2));
    return desc;
  }
  // Beyond our real strings (1..3 + a handful of interface strings). Returning NULL
  // mirrors the core and lets Windows' MS-OS probe at index 0xEE fail cleanly.
  if (index >= 16) return NULL;

  char serial[16];
  const char* str;
  switch (index) {
    case 1: str = "Arduino"; break;       // manufacturer (matches USB.manufacturerName)
    case 2: str = usbName.c_str(); break; // product (iProduct)
    case 3:                               // serial — stable per board
      snprintf(serial, sizeof(serial), "%012llX", (unsigned long long)ESP.getEfuseMac());
      str = serial;
      break;
    default: str = usbName.c_str(); break; // HID/CDC/config interface strings
  }

  uint8_t chr_count = strlen(str);
  if (chr_count > 38) chr_count = 38; // desc holds 39 UTF-16 chars + the header word
  for (uint8_t i = 0; i < chr_count; i++) desc[1 + i] = str[i];
  desc[0] = (uint16_t)((3 << 8) | (2 * chr_count + 2));
  return desc;
}

// ── Pin assignments (Arduino Nano ESP32 labels — see arduino-esp-32-wiring.md) ─
// Encoders: A/B quadrature pins + S push button. Encoder common terminals and the
// switches' common terminals are all wired to GND.
//
// Every D/A header pin is used (the status LED sits on B0, see below). A0..A7 are the
// same pins as D17..D24. D13 doubles as
// the built-in amber LED (GPIO48), so that LED stays lit while the firmware runs.
const uint8_t encA[6]    = { D12, D9, D6, D3, D13, A2 };
const uint8_t encB[6]    = { D11, D8, D5, D2, A0,  A3 };
const uint8_t encPush[6] = { D10, D7, D4, D0, A1,  A4 };

// 3-position ON-OFF-ON toggle switches SW1..SW2: terminals 1 and 3 go to the MCU and
// the common (2) to GND. Centre = neither pin LOW. Order: SW2 t1, SW2 t3, SW1 t1, SW1 t3.
const uint8_t swPin[4]   = { A5, A6, A7, D1 };

// NOTE: every pin above is driven with INPUT_PULLUP, so the closed-to-GND wiring
// reads LOW = pressed. All of these GPIOs support the ESP32-S3 internal pull-up;
// if a future revision moves a signal onto a pull-up-less pad, add an external
// 10 kΩ pull-up to 3V3 there.

// ── Status LED (B0) ────────────────────────────────────────────────────────────
// Blinks while booting/waiting for USB enumeration, steady once enumerated. All the
// D/A pins are taken by the controls, so the LED sits on the B0 header pin. B0 is
// GPIO46, which the core names LED_RED: it also drives the red channel of the
// on-board RGB LED, active-low, so that one shows the inverse of the status LED.
// GPIO46 is a boot strapping pin that must read LOW at reset; the LED + resistor to
// GND keeps it there. (Never use B1 instead: that is GPIO0, the boot-mode pin.)
const uint8_t STATUS_LED_PIN = LED_RED;

// ── Button index map (must match the wiring doc / src/panel/panel.ts) ─────────
const uint8_t encBase[6] = { 0, 3, 6, 9, 12, 15 }; // CW = base, CCW = base + 1, push = base + 2
const uint8_t swBase     = 18;

// ── Encoder rotation state ────────────────────────────────────────────────────
// Full quadrature decode: every loop read both A and B and accumulate only valid
// Gray-code transitions. Sampling B at a single A edge mis-read direction (CCW
// triggered false CW); here a bounce that jumps two bits, or wiggles back and
// forth, nets to zero — no false steps in either direction, and it doesn't care
// where the detent rests. One detent = one full cycle = 4 counts, so we emit a
// CW/CCW step each time the accumulator crosses ±DETENT_STEPS.
//
// Each detent becomes one or more momentary CW/CCW presses, *queued* rather than
// held: a held-and-re-armed button (the old approach) merged into one long press
// when you spun fast, so rapid rotation lost counts. The generator emits each
// queued press as a pulse (PULSE_ON_MS) + forced low gap (PULSE_GAP_MS) so each
// one is counted as a distinct press.
//
// Pulse length is sized for MSFS, NOT the web app. The app reads every ~1 ms USB
// report, so it counts even 2 ms pulses — but MSFS only samples gamepad state
// about once per frame (~16–33 ms). A pulse shorter than a frame can begin and end
// between two MSFS samples and never register, which is why fast spins moved the
// heading bug barely or not at all. To make MSFS see each press we hold it (and the
// gap after it) for about one frame at 60 fps (~16 ms), capping the emit rate at
// ~1000/(ON+GAP) ≈ 33 presses/s — the practical ceiling, since MSFS counts each
// press as 1° so max heading-bug rate ≈ that many °/s. If the sim runs below 60 fps
// (or stutters), a press can again fall between samples; bump ON/GAP back toward 20
// ms if steps go missing. The queue clamp is small so a fast flick's tail drains in
// ~0.25 s, not seconds.
const int8_t        DETENT_STEPS   = 4;  // quadrature counts per detent
const unsigned long PULSE_ON_MS    = 15; // CW/CCW button held high per press (~1 MSFS frame @ 60 fps)
const unsigned long PULSE_GAP_MS   = 15; // forced low gap between presses (~1 MSFS frame @ 60 fps)
const int8_t        STEP_QUEUE_MAX = 8;  // clamp pending steps (bounds post-spin tail to ~0.25 s)

// Rotational acceleration: presses emitted per detent grow with spin speed (the
// time since the previous detent on that encoder). A slow, deliberate turn stays
// 1:1 for single-unit precision; a fast flick multiplies, so e.g. the MSFS heading
// bug races instead of crawling 1° per detent. Tune the thresholds/multipliers to
// taste. Kept within the emit ceiling so the queue (and its tail) stays small.
const unsigned long ACCEL_T1 = 80; // ms between detents; slower than this → ×1
const unsigned long ACCEL_T2 = 40; // ms
const unsigned long ACCEL_T3 = 20; // ms; faster than this → fastest multiplier
const int8_t        ACCEL_M1 = 2;  // ACCEL_T2..ACCEL_T1 ms apart
const int8_t        ACCEL_M2 = 5;  // ACCEL_T3..ACCEL_T2 ms apart
const int8_t        ACCEL_M3 = 10; // faster than ACCEL_T3 (fast flick)

// Acceleration sensitivity (0..255), *per encoder*, host-configurable and saved in
// emulated EEPROM (flash). It scales the *extra* presses from the curve above: 0 =
// acceleration off (always 1:1), 255 = the full ACCEL_M* multipliers. Each encoder
// i uses EEPROM byte EEPROM_ADDR_ACCEL + i; an uninitialised byte reads 0xFF (=255),
// so the out-of-box default is full acceleration on every knob.
const int     EEPROM_SIZE       = 8;
const int     EEPROM_ADDR_ACCEL = 0;
uint8_t       accelSens[6] = { 255, 255, 255, 255, 255, 255 };

// ESP32's EEPROM is emulated in flash: writes are buffered until commit(). Mirror
// the AVR EEPROM.update() — only touch flash when the value actually changes.
void eepromUpdate(int addr, uint8_t value) {
  if (EEPROM.read(addr) != value) {
    EEPROM.write(addr, value);
    EEPROM.commit();
  }
}

// Direction of each (prevAB << 2 | currAB) transition: +1 CW, -1 CCW, 0 none/invalid.
// Derived from the confirmed orientation: A-falling-with-B-high (11→01) is CW.
const int8_t qdec[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0,
};

uint8_t       prevAB[6];     // last (A<<1 | B) phase per encoder
int8_t        accum[6];      // accumulated quadrature counts toward a detent
int8_t        pending[6];    // queued steps not yet pulsed (signed: + CW, − CCW)
unsigned long lastStepMs[6]; // millis() of the previous detent (for acceleration)
bool          pulseOn[6];    // is a CW/CCW press currently asserted?
int           pulseBtn[6];   // which button that press is on
unsigned long phaseUntil[6]; // millis() when the current on/gap phase ends

// ── Host serial config ───────────────────────────────────────────────────────
// Line protocol over the USB CDC port (the app's Settings page).
//
// Acceleration — each encoder has its own sensitivity, addressed by a single-digit
// index i (0..5):
//   "A<i><n>\n"  set encoder i sensitivity to n (0..255), persisted to EEPROM
//   "A<i>?\n"    query encoder i sensitivity
// Either way we reply "A<i>=<n>\n" so the host can confirm/read the stored value.
//
// Identity — reassign the board's USB PID + product name (persisted to NVS):
//   "SET_ID:<pidHex>:<name>\n"  store PID (hex, e.g. 80FC) + name, then soft-reboot
//                               so the new identity takes effect; replies "OK:<pid>:<name>"
//   "GET_ID\n"                  reply "ID:<pidHex>:<name>" with the stored values
char    cmdBuf[64];
uint8_t cmdLen = 0;

void applyAccelCommand(const char* s) {
  if (s[0] != 'A') return;
  uint8_t i = s[1] - '0';
  if (i >= 6) return;          // need a valid encoder index (0..5)
  const char* rest = s + 2;    // value or '?' follows the index
  if (rest[0] != '?' && rest[0] != '\0') {
    long v = atol(rest);
    if (v < 0) v = 0;
    else if (v > 255) v = 255;
    accelSens[i] = (uint8_t)v;
    eepromUpdate(EEPROM_ADDR_ACCEL + i, accelSens[i]); // only writes if changed
  }
  USBSerial.print('A');
  USBSerial.print(i);
  USBSerial.print('=');
  USBSerial.println(accelSens[i]);
}

void printIdentity(const char* prefix) {
  char pidHex[5];
  snprintf(pidHex, sizeof(pidHex), "%04X", usbPid);
  USBSerial.print(prefix);
  USBSerial.print(':');
  USBSerial.print(pidHex);
  USBSerial.print(':');
  USBSerial.println(usbName);
}

void applyIdCommand(const char* s) {
  if (strcmp(s, "GET_ID") == 0) {
    printIdentity("ID");
    return;
  }
  if (strncmp(s, "SET_ID:", 7) != 0) return;

  const char* p     = s + 7;          // "<pidHex>:<name>"
  const char* colon = strchr(p, ':');
  if (!colon || colon == p) return;   // need a PID and a separator

  char pidStr[8] = { 0 };
  size_t n = colon - p;
  if (n >= sizeof(pidStr)) return;
  memcpy(pidStr, p, n);
  uint16_t pid = (uint16_t)strtol(pidStr, nullptr, 16);

  String name = colon + 1;            // everything after the second ':'
  name.trim();
  if (name.length() == 0) return;

  storeIdentity(pid, name);
  usbPid  = pid;                       // reflect the just-stored values
  usbName = name;
  printIdentity("OK");
  USBSerial.flush();
  delay(50);                           // let the CDC packet drain before reset
  ESP.restart();
}

void applyConfigCommand(const char* s) {
  if (strncmp(s, "SET_ID:", 7) == 0 || strcmp(s, "GET_ID") == 0) applyIdCommand(s);
  else applyAccelCommand(s);
}

void handleSerialConfig() {
  while (USBSerial.available()) {
    char c = USBSerial.read();
    if (c == '\n' || c == '\r') {
      cmdBuf[cmdLen] = '\0';
      if (cmdLen > 0) applyConfigCommand(cmdBuf);
      cmdLen = 0;
    } else if (cmdLen < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdLen++] = c;
    } else {
      cmdLen = 0; // overlong line (no newline) — drop it and resync
    }
  }
}

// ── USB enumeration tracking (for the status LED) ─────────────────────────────
// This device is read purely over HID for normal use — the CDC port is only opened
// for one-off config (the app's Settings page), so it can't tell us whether the
// flight-sim app is actively using the device. The best available signal is USB
// enumeration itself: ARDUINO_USB_STARTED_EVENT fires once the host finishes
// recognizing the composite device (tud_mount_cb), ARDUINO_USB_STOPPED_EVENT if it's
// unplugged or the bus drops (tud_umount_cb). So "connected" here means "enumerated
// by a host", not "the app has it open" — there's no such signal for a plain HID
// gamepad with no application-level handshake.
volatile bool hostConnected = false;

void onUsbEvent(void* arg, esp_event_base_t base, int32_t id, void* eventData) {
  (void)arg;
  (void)base;
  (void)eventData;
  if (id == ARDUINO_USB_STARTED_EVENT) hostConnected = true;
  else if (id == ARDUINO_USB_STOPPED_EVENT) hostConnected = false;
}

// Blink while booting/waiting for USB enumeration; steady once enumerated
// (hostConnected, set from the USB started/stopped events above). If a host never
// shows up (unit powered from a data-less USB port/charger, or no PC attached), give
// up blinking after STATUS_LED_TIMEOUT_MS and go dark rather than blink forever — the
// wait window restarts if the host later disconnects. Non-blocking so it never stalls
// loop().
const uint32_t STATUS_LED_TIMEOUT_MS = 10000;

void updateStatusLed() {
  static bool     wasConnected = false;
  static uint32_t waitStart    = 0;
  static uint32_t lastToggle   = 0;
  static bool     ledOn        = false;
  uint32_t now = millis();

  if (hostConnected) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    wasConnected = true;
    return;
  }

  if (wasConnected) {
    // Just dropped from connected to disconnected — restart the blink window.
    wasConnected = false;
    waitStart = now;
  }

  if (now - waitStart >= STATUS_LED_TIMEOUT_MS) {
    digitalWrite(STATUS_LED_PIN, LOW); // gave up waiting; stay dark until enumerated
    return;
  }

  if (now - lastToggle >= 150) {
    lastToggle = now;
    ledOn = !ledOn;
    digitalWrite(STATUS_LED_PIN, ledOn);
  }
}

// ── Clean USB hand-over on a cold plug-in ─────────────────────────────────────
// At power-up the ESP32-S3 first appears on the bus as its built-in "USB JTAG/serial
// debug unit" (303A:1001). USB.begin() then hands the USB pins to TinyUSB, but if the
// host never sees the old device drop off the bus it keeps the stale 303A:1001 entry
// and never enumerates ours — the board then only works after a re-flash (which
// restarts straight into TinyUSB). Dropping the D+ pull-up for a moment makes the host
// register a real disconnect first, so it enumerates the Nobs device afresh.
const uint32_t USB_DETACH_MS = 250;

void detachBuiltinUsb() {
  SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
  CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP);
  delay(USB_DETACH_MS);
}

// Hand pull-up control back (register defaults) once TinyUSB owns the pins.
void releaseBuiltinUsbPulls() {
  SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP);
  CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
}

void setup() {
  for (uint8_t i = 0; i < 6; i++) {
    pinMode(encA[i], INPUT_PULLUP);
    pinMode(encB[i], INPUT_PULLUP);
    pinMode(encPush[i], INPUT_PULLUP);
    prevAB[i] = (digitalRead(encA[i]) << 1) | digitalRead(encB[i]);
    accum[i] = 0;
    pending[i] = 0;
    lastStepMs[i] = 0;
    pulseOn[i] = false;
    pulseBtn[i] = -1;
    phaseUntil[i] = 0;
  }
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(swPin[i], INPUT_PULLUP);
  }
  pinMode(STATUS_LED_PIN, OUTPUT);
  // Load each encoder's saved acceleration sensitivity (0xFF on a fresh chip → 255 = full).
  EEPROM.begin(EEPROM_SIZE);
  for (uint8_t i = 0; i < 6; i++) accelSens[i] = EEPROM.read(EEPROM_ADDR_ACCEL + i);

  // Load the saved identity (PID + name) from NVS, falling back to the light
  // defaults on a fresh chip.
  loadIdentity();

  // USB identity — set BEFORE USB.begin() so the descriptor is built with the stored
  // VID/PID/name (defaults: VID 0x303A / PID 0x80FC / "Nobs FS Light", matching the
  // app's gamepad filter). This works because build_opt.h disabled CDC/DFU-on-boot, so
  // USB has NOT been started yet (see the header).
  USB.VID(NOBS_USB_VID);
  USB.PID(usbPid);
  USB.productName(usbName.c_str());
  USB.manufacturerName("Arduino");

  Joystick.begin();        // create the HID report semaphores/mutex
  USB.onEvent(onUsbEvent); // detect USB enumeration, for the status LED
  USBSerial.begin(115200); // USB CDC config port; do NOT wait on it (would stall)
  detachBuiltinUsb();      // make the host drop the boot-time 303A:1001 device first
  USB.begin();             // build + start the composite USB device (HID + CDC)
  releaseBuiltinUsbPulls();
}

void loop() {
  unsigned long now = millis();

  handleSerialConfig(); // apply any pending acceleration-sensitivity command
  updateStatusLed();    // blink while waiting for USB enumeration, steady once enumerated

  // ── Encoders ────────────────────────────────────────────────────────────────
  for (uint8_t i = 0; i < 6; i++) {
    // Accumulate valid quadrature transitions; emit a step each full detent.
    uint8_t ab = (digitalRead(encA[i]) << 1) | digitalRead(encB[i]);
    if (ab != prevAB[i]) {
      accum[i] += qdec[(prevAB[i] << 2) | ab];
      prevAB[i] = ab;

      int8_t dir = 0;
      if (accum[i] >= DETENT_STEPS) {
        dir = 1;            // clockwise  → CW
        accum[i] -= DETENT_STEPS;
      } else if (accum[i] <= -DETENT_STEPS) {
        dir = -1;           // counter-cw → CCW
        accum[i] += DETENT_STEPS;
      }

      if (dir != 0) {
        // Acceleration: scale presses-per-detent by how fast we're turning, then
        // by the host-set sensitivity (0 = off/1:1, 255 = full curve).
        unsigned long delta = now - lastStepMs[i];
        lastStepMs[i] = now;
        int8_t baseMult = (delta < ACCEL_T3) ? ACCEL_M3
                        : (delta < ACCEL_T2) ? ACCEL_M2
                        : (delta < ACCEL_T1) ? ACCEL_M1
                        : 1;
        int16_t mult = 1 + ((int16_t)(baseMult - 1) * accelSens[i]) / 255;
        // Queue the (scaled) steps, clamped; the generator below paces them out.
        int16_t want = pending[i] + (int16_t)dir * mult;
        if (want > STEP_QUEUE_MAX) want = STEP_QUEUE_MAX;
        else if (want < -STEP_QUEUE_MAX) want = -STEP_QUEUE_MAX;
        pending[i] = (int8_t)want;
      }
    }

    // Pulse generator: turn queued steps into distinct press/gap cycles so even
    // rapid rotation counts every detent.
    if (pulseOn[i]) {
      if (now >= phaseUntil[i]) {           // end the press → start the low gap
        Joystick.setButton(pulseBtn[i], 0);
        pulseOn[i] = false;
        phaseUntil[i] = now + PULSE_GAP_MS;
      }
    } else if (now >= phaseUntil[i] && pending[i] != 0) {
      int8_t dir = (pending[i] > 0) ? 1 : -1; // start the next queued press
      pending[i] -= dir;
      pulseBtn[i] = (dir > 0) ? encBase[i] : encBase[i] + 1;
      Joystick.setButton(pulseBtn[i], 1);
      pulseOn[i] = true;
      phaseUntil[i] = now + PULSE_ON_MS;
    }

    // Push button is reported as a held level (pressed = LOW with INPUT_PULLUP).
    Joystick.setButton(encBase[i] + 2, digitalRead(encPush[i]) == LOW ? 1 : 0);
  }

  // ── Toggle switches ────────────────────────────────────────────────────────
  for (uint8_t i = 0; i < 4; i++) {
    Joystick.setButton(swBase + i, digitalRead(swPin[i]) == LOW ? 1 : 0);
  }

  // One USB report per loop with all the updates above (sent only when changed).
  Joystick.sendState();
}
