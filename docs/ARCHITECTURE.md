# ANDROIDOS-RK Architecture

## System Overview

ANDROIDOS-RK is a layered embedded Linux operating system designed for RK3326 handheld gaming devices. Each layer is independent, testable, and production-grade.

## Layer Stack

### Layer 1: Linux Base System

**Components:**
- Buildroot build system
- Linux kernel (5.15 LTS recommended)
- U-Boot bootloader
- BusyBox utilities
- Core system libraries (glibc, musl)

**Responsibilities:**
- Boot process
- Kernel module loading
- Device initialization
- System call interface

**Files:**
- `buildroot/` — Buildroot configuration & customizations
- `kernel/` — Kernel config, patches, device trees
- `scripts/build.sh` — Main build orchestration

---

### Layer 2: Hardware Drivers

**Components:**
- GPU drivers (Mali-400MP2)
- WiFi/Bluetooth drivers
- Audio drivers
- Sensor drivers
- USB drivers

**Responsibilities:**
- Initialize hardware interfaces
- Provide hardware access to upper layers
- Handle hardware-specific quirks

**Files:**
- `hardware/gpu/` — GPU initialization & configuration
- `hardware/wifi/` — WiFi/Bluetooth setup
- `hardware/audio/` — Audio device configuration
- `hardware/sensors/` — Accelerometer, gyro, light sensors

---

### Layer 3: Power Management & Thermal Control

**Components:**
- Battery monitoring daemon
- Power state management
- CPU frequency scaling (cpufreq)
- Thermal throttling
- Sleep/suspend handling

**Responsibilities:**
- Monitor battery health & level
- Report power status to UI
- Manage CPU scaling based on load
- Prevent thermal damage
- Handle low-battery shutdown

**Hardware APIs Used:**
- `/sys/class/power_supply/` — Battery data
- `/sys/class/thermal/` — Thermal zones
- `/sys/devices/system/cpu/cpu*/cpufreq/` — CPU frequency scaling
- `/proc/uptime` — System uptime

**Files:**
- `power/battery.c` — Battery monitoring service
- `power/thermal.c` — Thermal management
- `power/cpufreq.c` — CPU frequency scaling
- `system/power-daemon.service` — systemd service

---

### Layer 4: Input System & Controller Mapping

**Components:**
- Input device manager
- Controller profile system
- Hotkey handler
- Touchscreen input (if applicable)
- Button mapping engine

**Responsibilities:**
- Detect & initialize input devices
- Map physical buttons to logical actions
- Handle hotkey combinations
- Route input to active application
- Support per-game controller profiles

**Hardware APIs Used:**
- `/dev/input/eventX` — Raw input events
- `/proc/bus/input/devices` — Input device enumeration
- `/sys/class/input/` — Input device attributes

**Files:**
- `input/input-manager.c` — Input device handling
- `input/controller-map.c` — Button/axis mapping
- `input/hotkey-handler.c` — Hotkey detection
- `configs/controller-profiles/` — Per-game profiles

---

### Layer 5: Emulator Layer

**Components:**
- Emulator launcher & manager
- Runtime configuration system
- Save state handler
- Per-emulator environment setup
- Emulator process monitoring

**Supported Emulators:**
- PPSSPP (PSP)
- RetroArch (multi-system frontend)
- Standalone emulators (as needed)

**Responsibilities:**
- Launch emulators with correct parameters
- Manage emulator configuration
- Handle save states
- Monitor emulator crashes
- Apply controller profiles to emulators
- Manage emulator-specific resources

**Files:**
- `emulators/launcher.c` — Emulator launching
- `emulators/ppsspp-config.c` — PPSSPP-specific setup
- `emulators/retroarch-config.c` — RetroArch integration
- `emulators/save-state.c` — Save/load state management
- `configs/emulator-configs/` — Per-emulator configurations

---

### Layer 6: ROM Management System

**Components:**
- ROM scanner & indexer
- Metadata cache system
- ROM launcher
- Favorites & recently-played tracking
- ROM metadata parser (cover art, descriptions)

**Responsibilities:**
- Scan ROM directories periodically
- Cache ROM metadata
- Provide ROM database interface
- Track play history
- Generate favorites list
- Launch ROMs through correct emulators

**ROM Directory Structure:**
```
/roms/
├── psp/
├── ps1/
├── gba/
├── snes/
├── nes/
├── md/
└── ...
```

**Persistent Storage:**
- SQLite database for ROM metadata
- JSON files for user preferences
- Save states stored per-game

**Files:**
- `roms/scanner.c` — ROM directory scanning
- `roms/database.c` — SQLite metadata management
- `roms/launcher.c` — ROM execution
- `roms/favorites.c` — Favorites tracking

---

### Layer 7: Application Framework

**Components:**
- Application lifecycle manager
- Settings system
- File manager
- Package manager
- Notification system
- System services (logging, telemetry)

**Responsibilities:**
- Manage application execution
- Provide system configuration API
- Handle file operations
- Manage package installation/removal
- Display system notifications
- Centralized logging

**Files:**
- `system/app-manager.c` — Application lifecycle
- `system/settings.c` — System settings manager
- `system/file-manager.c` — File operations
- `system/package-manager.c` — Package management
- `system/notification.c` — Notification system
- `system/logger.c` — Centralized logging

---

### Layer 8: Android-Like Launcher & UI

**Components:**
- Home screen launcher
- App drawer
- Game library view
- Settings UI
- Status bar (battery, time, etc.)
- Quick settings panel
- System dialogs & alerts

**Responsibilities:**
- Display user interface
- Respond to user input
- Launch applications & games
- Display system status
- Provide settings UI
- Handle animations & transitions

**Technology:**
- SDL2 or similar for graphics
- Custom widget framework
- Theme system

**Files:**
- `ui/launcher.c` — Main launcher application
- `ui/home-screen.c` — Home screen UI
- `ui/app-drawer.c` — Application drawer
- `ui/game-library.c` — Game library view
- `ui/settings-ui.c` — Settings interface
- `ui/status-bar.c` — Status bar
- `ui/theme.c` — Theme system

---

## Data Flow Examples

### ROM Launch Flow

1. User selects ROM in UI (Layer 8)
2. ROM Manager queries database (Layer 6)
3. ROM Manager determines correct emulator
4. Emulator Layer loads configuration (Layer 5)
5. Input System applies controller profile (Layer 4)
6. Emulator launches via system exec
7. Power Management monitors CPU/battery (Layer 3)
8. Hardware drivers provide GPU/audio (Layer 2)
9. Linux kernel handles execution (Layer 1)

### Battery Status Update Flow

1. Power daemon reads `/sys/class/power_supply/` (Layer 3)
2. Data cached in memory
3. UI queries Application Framework (Layer 7)
4. Battery widget updated (Layer 8)
5. If low battery: notification triggered

### Input Handling Flow

1. Kernel receives input event from `/dev/input/eventX` (Layer 1)
2. Input Manager reads event (Layer 4)
3. Applies active controller profile
4. Routes to active application
5. Application handles input or passes to emulator

---

## Communication Between Layers

- **Layers 1-3**: Direct system calls, `/proc`, `/sys` APIs
- **Layers 4-5**: Message queues, shared memory, D-Bus
- **Layers 6-7**: IPC (Inter-Process Communication), REST-like local APIs
- **Layer 8**: DBus, local sockets to lower layers

---

## Error Handling & Recovery

- Each layer implements comprehensive logging
- Failed operations log to `/var/log/androidos/`
- Critical failures trigger recovery procedures
- Graceful degradation when components fail
- Watchdog timers for hung processes

---

## Performance Considerations

- Minimal background services (reduce RAM usage)
- GPU acceleration for UI rendering
- Efficient emulator launching (pre-load libraries)
- Caching of ROM metadata (fast game loading)
- CPU frequency scaling (extend battery life)
- Low-latency input handling (<5ms)

---

## Security Considerations

- Restricted file permissions for system components
- No root-privileged UI operations
- Input validation for all user inputs
- Safe ROM launching (sandboxed emulator processes)
- Secure storage for user data

---

## Testing Strategy

- Unit tests for each layer
- Integration tests between layers
- Hardware-in-the-loop testing on R36S
- Emulator compatibility testing
- Power/thermal stress testing
- Long-duration gaming sessions
