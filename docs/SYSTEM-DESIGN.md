# AndroidOS-RK System Design Specification

## Executive Summary

AndroidOS-RK is a complete ecosystem consisting of:
1. **R36S/R35S Operating System** - Full Linux-based OS on RK3326 hardware
2. **PC Application** - Cross-platform desktop application for management
3. **Mobile Application** - iOS/Android companion app for remote control

All components communicate via REST API and WebSocket for real-time synchronization.

## R36S Operating System (THE OS)

### Boot Process

```
1. Power On
   ↓
2. U-Boot Bootloader
   ↓
3. Linux Kernel Load
   ↓
4. Device Tree Processing
   ↓
5. InitRAMFS (Recovery/Mount)
   ↓
6. Systemd Init System
   ↓
7. Hardware Initialization
   ├─ GPIO Setup
   ├─ Display Initialization
   ├─ Input Device Detection
   └─ Audio System Start
   ↓
8. System Services Start
   ├─ Power Manager Daemon
   ├─ Hardware Monitor
   ├─ Network Manager
   ├─ Input Handler
   └─ Display Composer
   ↓
9. Android-Like Launcher Start
   ↓
10. Ready for User Interaction
```

### Core System Services (R36S)

#### 1. Power Management Service
```c
// File: system/power/power-manager.c

struct PowerState {
    int battery_level;      // 0-100%
    float temperature;      // CPU temp in °C
    int cpu_freq;          // Current MHz
    char state[32];        // awake, sleep, hibernation
    uint64_t uptime;       // milliseconds
};

functions:
- read_battery_level()      // /sys/class/power_supply/
- read_temperature()        // /sys/class/thermal/
- set_cpu_frequency()       // Scaling governor
- handle_low_battery()      // Shutdown sequence
- thermal_throttle()        // If temp > threshold
- monitor_power_states()    // Main loop
```

#### 2. Input Handler Service
```c
// File: system/input/input-handler.c

struct GamepadState {
    uint16_t buttons;       // A, B, X, Y, etc.
    int8_t analog_x;        // -127 to +127
    int8_t analog_y;
    uint16_t triggers;      // L1, L2, R1, R2
    uint32_t timestamp;     // Event time
};

functions:
- init_input_devices()      // /dev/input/event*
- read_gamepad_state()      // Poll or select()
- map_controller()          // Different controller types
- send_to_emulator()        // Pass to active app
- detect_special_combos()   // Power+Vol, etc.
```

#### 3. Hardware Monitor Service
```c
// File: system/hardware/hw-monitor.c

struct HardwareInfo {
    char cpu_model[64];
    int cpu_cores;
    int cpu_freq_max;
    uint64_t ram_total;
    uint64_t ram_available;
    uint64_t storage_total;
    uint64_t storage_free;
    float cpu_temp;         // Current
    float battery_health;   // Percentage
    int display_brightness; // 0-100
    char gpu_vendor[64];
};

functions:
- read_cpu_info()           // /proc/cpuinfo
- read_memory_info()        // /proc/meminfo
- read_storage_info()       // df command
- read_thermal_zones()      // /sys/class/thermal/
- read_battery_info()       // /sys/class/power_supply/
```

### Launcher & UI (R36S)

#### Main Launcher Screen
```
┌────────────────────────────────────┐
│ AndroidOS-RK              10:30 AM │  ← Time/Status
├────────────────────────────────────┤
│ Recent Games (Horizontal Scroll)   │
│ ┌─────────┐ ┌─────────┐ ┌──────┐  │
│ │ God of  │ │ Need for│ │Pokém-│  │
│ │ War     │ │ Speed   │ │ on   │  │
│ └─────────┘ └─────────┘ └──────┘  │
├────────────────────────────────────┤
│ Game Categories (Vertical)         │
│ ┌────────┐ ┌────────┐ ┌────────┐  │
│ │ PSP    │ │ NES    │ │ SNES   │  │
│ │ Games  │ │ Games  │ │ Games  │  │
│ └────────┘ └────────┘ └────────┘  │
├────────────────────────────────────┤
│ [Browse] [Settings] [File Mgr] ... │ ← Quick access
└────────────────────────────────────┘
```

#### All Apps Screen
```
┌────────────────────────────────────┐
│ < All Apps                         │
├────────────────────────────────────┤
│ System Apps:                       │
│ • File Manager                     │
│ • Settings                         │
│ • Games Browser                    │
│ • RetroArch                        │
│ • PPSSPP                           │
│ • Dolphin Emulator                 │
│ • System Monitor                   │
│ • Battery Manager                  │
│ • Gallery                          │
│ • Media Player                     │
│ • Terminal (Optional)              │
│                                    │
│ Installed Games:                   │
│ • [Long list scrollable]           │
└────────────────────────────────────┘
```

#### Settings Screen
```
┌────────────────────────────────────┐
│ < Settings            < General    │
├────────────────────────────────────┤
│ Display                    ○  ►    │
│ Sound                      ○  ►    │
│ Network                    ○  ►    │
│ Storage                    ○  ►    │
│ Battery                    ○  ►    │
│ About Device               ○  ►    │
│ Advanced                   ○  ►    │
│ System Update              ○  ►    │
└────────────────────────────────────┘
```

#### Battery Monitor Screen
```
┌────────────────────────────────────┐
│ < Battery Monitor                  │
├────────────────────────────────────┤
│                                    │
│          ┌──────────┐              │
│          │          │ 100%         │
│          │  ██████  │ Charging     │
│          │          │              │
│          └──────────┘              │
│                                    │
│ Voltage:      4.20 V               │
│ Temperature:  31.2 °C              │
│ Health:       Good                 │
│ Capacity:     3000 mAh             │
│ Technology:   Li-ion               │
│ Cycles:       150                  │
│                                    │
│ Charging Details:                  │
│ Current: 500 mA                    │
│ Est. Time: 2h 30m                  │
│                                    │
└────────────────────────────────────┘
```

### Emulator Management

```yaml
# File: configs/emulators.yml

emulators:
  ppsspp:
    name: "PPSSPP - PSP Emulator"
    executable: "/usr/bin/ppsspp"
    config_dir: "~/.config/ppsspp/"
    rom_extensions: [".iso", ".cso"]
    default_controller: "gamepad"
    performance: "high"
    
  retroarch:
    name: "RetroArch - Multi-System"
    executable: "/usr/bin/retroarch"
    config_dir: "~/.config/retroarch/"
    cores:
      nes: "nestopia_libretro.so"
      snes: "snes9x_libretro.so"
      genesis: "genesis_plus_gx_libretro.so"
    rom_extensions: [".nes", ".smc", ".bin"]
    
  dolphin:
    name: "Dolphin - GameCube/Wii"
    executable: "/usr/bin/dolphin-emu"
    config_dir: "~/.config/dolphin-emu/"
    rom_extensions: [".iso", ".gcz", ".wbfs"]
    performance: "medium"
```

## PC Desktop Application

### Architecture
```
┌───────────────────────────────────────┐
│ PC App Main Window                    │
├───────────────────────────────────────┤
│  Menu Bar (File, Edit, View, Help)    │
├───────────────────────────────────────┤
│ Sidebar      │                        │
│              │ Main Content Area      │
│ • Library    │                        │
│ • Devices    │ - Game Grid            │
│ • Settings   │ - Details Panel        │
│ • About      │ - Preview              │
│              │                        │
│              │                        │
├───────────────────────────────────────┤
│ Status Bar: Connected to R36S...      │
└───────────────────────────────────────┘
```

### Key Features

#### 1. ROM Library Manager
```typescript
interface Game {
  id: string;
  title: string;
  emulator: string;
  rom_path: string;
  cover_image: string;
  description: string;
  release_date: string;
  playtime: number;      // hours
  last_played: timestamp;
  rating: number;        // 1-5
  tags: string[];
  metadata: object;
}

class ROMLibrary {
  scan_directory(path: string): Game[];
  add_game(rom_path: string, emulator: string): void;
  remove_game(game_id: string): void;
  update_metadata(game_id: string, metadata: object): void;
  sync_to_device(device_ip: string): Promise<void>;
  export_library(format: 'csv' | 'json'): void;
}
```

#### 2. Device Management
```typescript
interface Device {
  id: string;
  name: string;
  ip_address: string;
  model: string;         // R36S, R35S, etc.
  os_version: string;
  connected: boolean;
  battery: number;       // 0-100
  storage_free: number;  // MB
}

class DeviceManager {
  discover_devices(): Device[];
  connect_to_device(ip: string): Promise<Device>;
  send_command(device: Device, cmd: string): Promise<void>;
  transfer_roms(device: Device, games: Game[]): Promise<void>;
  get_device_stats(device: Device): Promise<SystemStats>;
  remote_control(device: Device): void;  // Launch remote UI
}
```

#### 3. Remote Control Panel
```
┌──────────────────────────────┐
│ Remote Controller - R36S      │
├──────────────────────────────┤
│                              │
│      ← [Phone] [Screen] →    │
│                              │
│         D-PAD Display        │
│       ↑                       │
│     ← + →     PS Button      │
│       ↓      L1 R1  Y        │
│                    X  B      │
│                       A      │
│      L2      Volume    R2    │
│     ─ ○ ─   Brightness ─ ○ ─│
│                              │
│  Status: Connected           │
│  Signal: ████░  (4/5)        │
│  Latency: 45ms               │
└──────────────────────────────┘
```

## Mobile Application (iOS/Android)

### Screens

#### Home Screen
```
┌──────────────────────────────┐
│ AndroidOS-RK        Settings  │
├──────────────────────────────┤
│ Connected Devices            │
│ ┌────────────────────────┐   │
│ │ R36S Device            │   │
│ │ 🟢 Online              │   │
│ │ Battery: 87%           │   │
│ │ Model: R36S            │   │
│ │ Storage: 12.3/32GB     │   │
│ └────────────────────────┘   │
│                              │
│ Quick Actions                │
│ [▶ Start Game] [Settings]   │
│ [Sync] [Backup] [Monitor]   │
│                              │
│ Recent Games                 │
│ [Game 1] [Game 2] [Game 3]  │
│                              │
│ Device Stats                 │
│ CPU: 45% | RAM: 680MB       │
│ Temp: 42°C | FPS: 60       │
│                              │
└──────────────────────────────┘
```

#### Remote Controller Screen
```
┌──────────────────────────────┐
│ Controller         [||] ← →   │
├──────────────────────────────┤
│                              │
│           ▲ Y                │
│         ◄ ► X B              │
│           ▼ A                │
│                              │
│     L1: ___    R1: ___       │
│     L2: ___    R2: ___       │
│                              │
│  Mode: [Gamepad▼]           │
│                              │
│  Analog Stick Left           │
│  ┌─────────────┐             │
│  │      ●      │  Sensitivity│
│  └─────────────┘  [◄────────►]│
│                              │
│  Settings | Calibrate | Info │
└──────────────────────────────┘
```

#### System Monitor
```
┌──────────────────────────────┐
│ System Monitor          ⟳     │
├──────────────────────────────┤
│ CPU Usage: 45%               │
│ ████░░░░░░░░░░░              │
│                              │
│ RAM Usage: 680/1024 MB       │
│ ██████░░░░░░░░░░             │
│                              │
│ Storage: 12.3/32 GB          │
│ ████████░░░░░░░░             │
│                              │
│ Temperature: 42°C ✓          │
│ Battery: 87% (Discharging)   │
│                              │
│ Network: WiFi 5GHz (45dBm)  │
│ Bluetooth: Off               │
│                              │
│ FPS: 60                      │
│ Uptime: 2d 14h 32m           │
│                              │
│ [Details] [History] [Log]    │
└──────────────────────────────┘
```

## Network Protocol Specification

### REST API Endpoints

```
# Base URL: http://<device_ip>:8888/api/v1

# Device Information
GET  /system/info              → {device_info}
GET  /system/stats             → {cpu, ram, temp, battery}
GET  /system/time              → {timestamp}
POST /system/shutdown          → {status}
POST /system/reboot            → {status}

# Games & ROMs
GET  /games/list               → [games]
GET  /games/{id}               → {game_details}
POST /games/launch/{id}        → {pid, status}
POST /games/{id}/save          → {status}
GET  /games/{id}/savestates    → [saves]

# Storage
GET  /storage/info             → {total, used, free}
GET  /storage/mount            → [mounts]
POST /storage/scan             → {status}

# Input
POST /input/event              → {button: value}
POST /input/calibrate          → {status}

# Power
GET  /power/battery            → {level, temp, health}
GET  /power/thermal            → {zones: [temps]}
POST /power/settings           → {brightness, performance}

# Settings
GET  /settings/all             → {settings}
POST /settings/{key}           → {value}
GET  /settings/{category}      → {settings}

# Files
GET  /files/browse/{path}      → [files]
POST /files/upload             → {status}
DELETE /files/{path}           → {status}
```

### WebSocket Events (Real-time)

```javascript
// Connect: ws://<device_ip>:8888/ws

// Server → Client
{
  "event": "system_stats",
  "data": {
    "cpu_usage": 45,
    "ram_usage": 680,
    "temperature": 42.5,
    "battery": 87,
    "fps": 60
  }
}

{
  "event": "game_started",
  "data": {
    "game_id": "psp_god_of_war",
    "pid": 2341,
    "timestamp": 1717600000
  }
}

{
  "event": "input_event",
  "data": {
    "button": "A",
    "state": "pressed",
    "timestamp": 1717600000
  }
}

// Client → Server
{
  "action": "launch_game",
  "game_id": "psp_god_of_war"
}

{
  "action": "send_input",
  "button": "A",
  "state": "pressed"
}
```

## Security & Authentication

### Device Access Control
```
1. First Connection:
   - Generate pairing code on device
   - Show QR code on device screen
   - Scan from PC/Mobile app
   - Verify PIN on both devices
   - Exchange public keys

2. Subsequent Connections:
   - Use stored public key
   - TLS 1.3 for all traffic
   - Mutual authentication
   - Token-based sessions

3. Revocation:
   - Remove trusted device from settings
   - Revoke token immediately
   - Clear cached credentials
```

## Deployment & Updates

### Over-The-Air (OTA) Updates
```
R36S Device Flow:
1. Check for updates (daily)
2. Download delta if available
3. Verify signature
4. Decompress to partition
5. Mark for update on next boot
6. Graceful shutdown
7. Bootloader applies update
8. Verify installation
9. Boot into new version
```

## Performance Targets

```
R36S (RK3326):
- Boot time: < 30 seconds
- Launcher startup: < 2 seconds
- Game launch: 2-5 seconds
- UI responsiveness: < 100ms
- Thermal shutdown: > 85°C
- Battery idle: < 2% per hour

PC App (x86_64):
- Startup: < 1 second
- Library load (1000 games): < 2 seconds
- Remote control latency: < 100ms
- RAM usage: < 200MB

Mobile App (iOS/Android):
- Startup: < 2 seconds
- Connection latency: < 200ms
- Memory usage: < 150MB
- Battery drain: < 5% per hour (active use)
```
