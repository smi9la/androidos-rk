# R36s Smart System - Installation Guide

## ✨ Overview

This guide covers installation of:
- **Android Phone Controller** - Mobile app to manage R36s
- **R36s Device Firmware** - Core system for R36s device

---

## 📱 Android Phone Controller Installation

### Requirements
- Android 6.0 (API 23) or higher
- Bluetooth 4.2+ support
- 100 MB free storage

### Installation Steps

1. **Download APK**
   ```bash
   wget https://github.com/smi9la/androidos-rk/releases/download/v2.0.0/r36s-controller-2.0.0.apk
   ```

2. **Transfer to Phone** (via USB or email)

3. **Allow Unknown Sources**
   - Settings → Security → Unknown Sources → Enable

4. **Install APK**
   - Open file manager
   - Locate `r36s-controller-2.0.0.apk`
   - Tap to install

5. **Grant Permissions**
   - Bluetooth
   - Location (for Bluetooth scanning)

6. **Launch Application**
   - Find in App Drawer
   - Tap to launch

---

## 🎮 R36s Device Firmware Installation

### Requirements
- R36s Device V1.0 or later
- USB cable
- Computer with Python 3.7+

### Installation Process

1. **Connect R36s to Computer**
   ```bash
   ls /dev/tty.* | grep -i usb
   ```

2. **Put R36s in DFU Mode**
   - Hold Power + Reset buttons
   - Hold for 5 seconds until LED flashes

3. **Flash Firmware**
   ```bash
   python3 flash_firmware.py r36s-firmware-2.0.0.bin
   ```

---

## 🔗 Pairing Phone to R36s

1. Enable Bluetooth on Phone
2. Enable Bluetooth on R36s
3. Open Controller App
4. Tap "Scan for R36s Devices"
5. Select device and confirm pairing

---

**Version**: 2.0.0
**Last Updated**: 2026-06-05
