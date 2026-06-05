# R36s Smart System - Advanced Mobile Device Controller

![R36s Smart System](https://img.shields.io/badge/version-2.0.0-brightgreen)
![License](https://img.shields.io/badge/license-MIT-blue)
![Platform](https://img.shields.io/badge/platform-Android%20Phone%20%7C%20R36s%20Device-orange)

> **Powerful remote control system for R36s portable devices with Android phone controller app**

---

## Overview

**R36s Smart System** is an advanced control platform for R36s portable devices. Manage and monitor your R36s from your Android phone with real-time Bluetooth connectivity.

---

## Key Features

- 🎮 **Real-time Device Control** - Control R36s from your phone
- 📱 **Android Phone Controller** - Beautiful interface for device management
- ⚙️ **Device Configuration** - Complete R36s system setup
- 🔄 **Data Synchronization** - Automatic sync between devices
- 📊 **Performance Analytics** - Monitor device status
- 💾 **Local Storage** - No cloud connection required
- 🚀 **Fast & Lightweight** - Low resource usage

---

## Downloads

### Android Phone App
```bash
https://github.com/smi9la/androidos-rk/releases/download/v2.0.0/r36s-controller-2.0.0.apk
```

### R36s Device Firmware
```bash
https://github.com/smi9la/androidos-rk/releases/download/v2.0.0/r36s-firmware-2.0.0.bin
```

---

## Quick Start

### Install Phone App
1. Download `r36s-controller-2.0.0.apk`
2. Transfer file to your phone
3. Open file manager and tap to install
4. Grant necessary permissions

### Install R36s Firmware
1. Download `r36s-firmware-2.0.0.bin`
2. Connect R36s to your computer
3. Run firmware installation tool
4. Reboot R36s device

---

## System Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| Android Phone | 6.0 API 23 | 10+ |
| R36s Device | V1.0 | V2.0+ |
| Storage | 100 MB | 500 MB |

---

## System Architecture

```
┌──────────────────────┐
│  Android Phone       │
│  (Controller App)    │
└──────────────┬───────┘
               │ Bluetooth/WiFi
               │
┌──────────────▼───────┐
│   R36s Device        │
│  (Main System)       │
└──────────────────────┘
```

---

## Documentation

- [Installation Guide](INSTALLATION.md)
- [User Guide](USER_GUIDE.md)
- [API Reference](API.md)

---

## Development Requirements

- Node.js 14+
- Java 8+
- Python 3.7+
- Android SDK
- Git

---

## Building from Source

```bash
# Install dependencies
pip3 install -r requirements.txt
npm install

# Build all components
python3 BUILD_ALL.py

# Or build platforms separately
cd phone && ./gradlew build  # Android
cd ../r36s && make build    # R36s Device
```

---

## Statistics

- **Version**: 2.0.0 (R36s Controller)
- **Features**: 12+
- **Lines of Code**: 5000+
- **Files**: 20+

---

## Bug Reports

Found an issue? [Report it here](https://github.com/smi9la/androidos-rk/issues)

---

## Feature Requests

Have a great idea? [Share it here](https://github.com/smi9la/androidos-rk/discussions)

---

## License

MIT License - Available for everyone

---

## Developer

**smi9la** - Software developer and innovator

---

## Credits

Thank you for using **R36s Smart System**! 🎮✨

---

**Version**: v2.0.0
**Release Date**: 2026-06-05
**Status**: ✅ Ready for use