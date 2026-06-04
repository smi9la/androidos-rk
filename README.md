# ANDROIDOS-RK

**Production-Grade Handheld Gaming Operating System for RK3326 Devices**

A real, hardware-integrated Linux-based OS for R36S, R35S, and compatible RK3326 handheld gaming devices. No simulations. No mockups. Real emulators, real ROMs, real hardware.

## Overview

ANDROIDOS-RK combines:
- **ArkOS Stability** — Proven reliable base system
- **Android-Like UX** — Modern, intuitive interface
- **Real Linux** — Full hardware integration, no fake features
- **Real Emulation** — PPSSPP, RetroArch, standalone emulators
- **Production Grade** — Comprehensive logging, error handling, recovery

## Hardware Support

- **Primary Target**: RK3326 (ARM Cortex-A35)
- **Reference Devices**: R36S, R35S
- **RAM**: 1GB–2GB
- **Storage**: MicroSD Card
- **Boot**: U-Boot + Linux Kernel

## Architecture Layers

```
Layer 8: Android-Like Launcher & UI
Layer 7: Application Framework
Layer 6: ROM Management System
Layer 5: Emulator Layer (PPSSPP, RetroArch, etc.)
Layer 4: Input System & Controller Mapping
Layer 3: Power Management & Thermal Control
Layer 2: Hardware Drivers
Layer 1: Linux Base System (Buildroot)
```

## Project Structure

```
androidos-rk/
├── docs/                      # Architecture & design documentation
├── buildroot/                 # Buildroot configurations & customizations
├── kernel/                    # Kernel configs, device trees, patches
├── hardware/                  # Hardware abstraction layer
├── power/                     # Battery, thermal, power management
├── input/                     # Controller mapping, input handling
├── emulators/                 # Emulator configs & integration
├── roms/                      # ROM scanner, metadata, launcher
├── ui/                        # Android-like launcher & UI framework
├── system/                    # Core system services & daemons
├── scripts/                   # Build, deployment, recovery tools
├── configs/                   # Default configurations & profiles
└── tools/                     # Debugging, logging, utilities
```

## Build & Deploy

See `docs/BUILD.md` for detailed instructions.

**Quick Start:**
```bash
./scripts/build.sh r36s           # Build for R36S
./scripts/image.sh                # Create bootable SD image
./scripts/flash.sh /dev/sdX       # Flash to SD card
```

## Features

✅ Real hardware data (battery, CPU, RAM, temperature)
✅ Real emulator integration (PPSSPP, RetroArch)
✅ Real ROM launching & management
✅ Android-inspired launcher with real app drawer
✅ Power management with low-battery detection
✅ Per-game controller profiles
✅ Save state management
✅ System information & diagnostics
✅ Comprehensive logging & error recovery
✅ Package management for themes & extensions

## No Simulation

Every feature is connected to:
- Real executables
- Real hardware APIs
- Real files & services
- Real system information

**If it can't be implemented, it's removed.**

## Documentation

- `docs/ARCHITECTURE.md` — System design & layer details
- `docs/BUILD.md` — Build instructions & dependencies
- `docs/HARDWARE.md` — RK3326 hardware reference
- `docs/POWER.md` — Power management specifications
- `docs/EMULATORS.md` — Emulator configuration guide
- `docs/UI.md` — Launcher & UI design specifications

## License

MIT

## Status

🔨 **In Development** — Phase 1: Bootable Base System

---

**Built for RK3326. Built for real. Built to last.**
