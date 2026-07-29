# ESP32 Bluetooth HID Keyboard Project Roadmap

This document preserves the working plan for the firmware project so future sessions can continue from the same architecture and milestones without drift.

## Project Goal

Build a professional Bluetooth HID keyboard firmware for ESP32 with:

- Bluetooth HID keyboard support
- Keyboard matrix support
- Media keys and consumer control
- Multiple layers
- Battery service
- Deep sleep and low-power advertising
- NKRO where feasible
- Macros
- Future OTA support
- Future RGB support
- A clean, reusable firmware architecture

## Current Foundation

The Bluetooth HID layer is already working and should not be redesigned.

Completed and preserved:

- BLE advertising
- HID service
- HID report descriptor
- HID input report
- HID output report
- Battery service
- Manufacturer information
- PnP information
- HID information
- Windows recognition as a Bluetooth keyboard
- Connection establishment
- Basic key press/release support
- Enter key press/release confirmed working on the host

## Working Rules

- Never rewrite the entire project.
- Modify one function, one class, or one file at a time.
- Compile after each milestone before moving on.
- Build on the existing architecture rather than introducing future features early.
- Preserve compatibility with the current toolchain and libraries:
  - ESP32 Arduino Core 3.3.11
  - NimBLE-Arduino 2.5.0
- Never downgrade libraries or replace NimBLE.

## Development Phases

### Phase 1 - Bluetooth HID Foundation

Status: Complete

Do not change unless fixing a bug.

### Phase 2 - Keyboard Engine

Status: Complete

Milestones:

1. Connection callbacks
   - Detect connect
   - Detect disconnect
   - Maintain connected state
   - Restart advertising
2. Keyboard report engine
   - 8-byte HID report
   - sendReport()
3. Keyboard API
   - press()
   - release()
   - releaseAll()
   - write()
4. Character mapping
   - ASCII to HID mapping
   - Uppercase/lowercase/numbers/symbols
5. Typing engine
   - print()
   - println()
   - Test note: when connected, pressing the physical test button should send Enter, Shift+A, and Ctrl+Enter to the host. If these actions appear in a text field, the milestone is working.
6. Modifier keys
   - Shift, Ctrl, Alt, GUI
   - Test note: pressing the physical test button should send Shift+A, Ctrl+Enter, and Alt+Tab to the host one step at a time.
7. Multiple simultaneous keys
   - 6-key rollover and modifier combinations
   - Implemented: a public pressMany() helper and a button-driven rollover/combo test step
8. Stress testing
   - Repeated typing, reconnects, rapid key changes
   - Implemented: repeated typing and rapid key-change test steps in the main sketch

### Phase 3 - Keyboard Matrix

Status: Complete

Completed:

- GPIO row/column scanning
- Per-key debounce state machine
- Ghost detection and rollover behavior
- Matrix key position to HID keycode mapping
- Integration with the existing BleHID press/release API
- Minimal hardware abstraction for future row/column sizes

Phase 3 architecture:

- Create a dedicated matrix scanner module (e.g. `KeyboardMatrix`) separate from HID logic.
- Define matrix hardware in one place with row pins, column pins, and keymap data.
- Scan rows by driving one row low/high at a time and reading column inputs.
- Use a small debounce tracker per key to stabilize presses/releases before emitting HID events.
- Detect ghosted combinations and avoid sending invalid simultaneous key reports.
- Translate stable matrix coordinates into HID usage codes via a lookup table.
- Keep the existing `BleHID` API unchanged; feed key events through `press()`, `release()`, and `releaseAll()`.

Milestones:

1. Matrix config and pin definitions
   - Define row/column pins and key positions.
   - Keep config hardware-agnostic for future board versions.
2. Basic scanning engine
   - Implement row strobes and column reads.
   - Report raw matrix activity without debounce.
3. Debounce and stable key state
   - Add per-key timing to suppress bounce.
   - Require a stable scan count before changing state.
4. Key mapping and HID translation
   - Map matrix positions to HID codes.
   - Use the existing ASCII/HID translation helpers for normal keys.
5. Ghost handling / rollover behavior
   - Detect unsupported ghost combinations and avoid invalid reports.
   - Document that physical diodes are expected for safe NKRO.
6. Integration test
   - Wire matrix logic into `main.cpp`.
   - Preserve current button test path until matrix scanning is validated.

Immediate next step:

- Draft the `KeyboardMatrix` module and the first scan loop.
- Keep changes small and compile after the scanner module is added.
- Verify with a simple matrix detection test before adding HID output.

### Phase 4 - Media Keys

Planned:

- Volume, mute, brightness, play, pause, next, previous

### Phase 5 - Power Management

Planned:

- Battery percentage reporting
- Deep sleep
- Wakeup support

### Phase 6 - Advanced Features

Planned:

- Layers
- Fn layer
- Macros
- Configuration and preferences

### Phase 7 - Firmware Optimization

Planned:

- Refactoring
- Modularization
- Performance and memory optimization
- Documentation
- GitHub release preparation

## Current Working Style

- Keep changes small and focused.
- Prefer encapsulation and reusable helpers.
- Use modern C++ practices.
- Avoid unnecessary global state.
- Keep comments only where they add real value.

## Working Procedure

1. Explain the current milestone.
2. Explain why it is needed.
3. Describe the expected architecture change.
4. Apply minimal code changes.
5. Compile and verify.
6. Run the milestone test from the firmware.
7. Record the result in the roadmap and main sketch.
8. Continue only after successful verification.

## Notes

This roadmap is intended to keep the project consistent across chats and future development sessions.
