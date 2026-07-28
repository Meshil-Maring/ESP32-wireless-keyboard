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

Status: In progress

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
6. Modifier keys
   - Shift, Ctrl, Alt, GUI
7. Multiple simultaneous keys
   - 6-key rollover and modifier combinations
8. Stress testing
   - Repeated typing, reconnects, rapid key changes

### Phase 3 - Keyboard Matrix

Planned:

- GPIO scanning
- Debouncing
- Ghost detection
- Key mapping

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
6. Test the result.
7. Continue only after successful verification.

## Notes

This roadmap is intended to keep the project consistent across chats and future development sessions.
