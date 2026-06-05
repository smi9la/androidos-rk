# R36s Smart System - Installation Guide

## Overview

This guide covers installation of:
- **Android Phone Controller** - Mobile app to manage R36s
- **R36s Device Firmware** - Core system for R36s portable device

---

## Android Phone Controller Installation

### Requirements
- Android 6.0 (API 23) or higher
- Bluetooth 4.2+ support
- 100 MB free storage space

### Installation Steps

1. **Download APK**
   ```bash
   wget https://github.com/smi9la/androidos-rk/releases/download/v2.0.0/r36s-controller-2.0.0.apk
   ```

2. **Transfer to Phone**
   - Via USB cable or email attachment

3. **Enable Unknown Sources**
   - Settings → Security → Unknown Sources → Enable

4. **Install APK**
   - Open file manager
   - Find `r36s-controller-2.0.0.apk`
   - Tap to install

5. **Grant Permissions**
   - Bluetooth permission
   - Location permission (for Bluetooth scanning)

6. **Launch Application**
   - Find in App Drawer
   - Tap R36s Smart Controller to launch

---

## R36s Device Firmware Installation

### Requirements
- R36s Device V1.0 or later
- USB cable (USB-C or Micro-USB)
- Computer with Python 3.7+

### Pre-Installation

1. **Download Firmware**
   ```bash
   wget https://github.com/smi9la/androidos-rk/releases/download/v2.0.0/r36s-firmware-2.0.0.bin
   ```

2. **Install Python Dependencies**
   ```bash
   pip3 install -r requirements.txt
   ```

### Installation Process

1. **Connect R36s to Computer**
   ```bash
   # Linux/Mac
   ls /dev/tty.* | grep -i usb
   
   # Windows
   Device Manager → USB Devices
   ```

2. **Put R36s in DFU Mode**
   - Hold Power button + Reset button
   - Hold for 5 seconds until LED flashes
   - Device enters firmware update mode

3. **Flash Firmware**
   ```bash
   python3 flash_firmware.py r36s-firmware-2.0.0.bin
   ```

4. **Monitor Progress**
   ```
   Flashing... [████████████████] 100%
   Verifying... Done
   Rebooting device...
   ```

5. **Verify Installation**
   - Device reboots automatically
   - Display shows "R36s Smart v2.0.0"
   - Ready for pairing

---

## Pairing Phone to R36s

### First Connection

1. **Enable Bluetooth on Phone**
   - Settings → Bluetooth → On

2. **Enable Bluetooth on R36s**
   - Hold Bluetooth button for 3 seconds
   - LED starts blinking

3. **Open Controller App**
   - Tap "Scan for R36s Devices"
   - Wait 10 seconds for scan to complete

4. **Select R36s Device**
   - Tap device from the list
   - Confirm pairing on both devices
   - Enter PIN if prompted

5. **Successful Connection**
   - Status displays "Connected"
   - LED turns solid blue
   - Ready to use

---

## Testing Installation

### Phone App Test
```bash
Verify:
✓ Bluetooth scan works
✓ Device discovered in list
✓ Connection established
✓ Status information displays
```

### R36s Device Test
```bash
Check:
✓ Device boots normally
✓ Main menu displays
✓ Bluetooth advertises
✓ Commands received from phone
```

---

## Troubleshooting

### App Won't Install
**Problem**: Installation fails with permission error
**Solution**:
1. Go to Settings → Apps
2. Uninstall any previous version
3. Clear cache for Play Store
4. Retry installation

### Bluetooth Won't Connect
**Problem**: Devices won't pair or connect
**Solution**:
1. Restart both devices
2. Forget device from Bluetooth settings
3. Try pairing again
4. Check signal strength (move closer)
5. Update firmware to latest version

### Firmware Flash Failed
**Problem**: Flashing stops midway
**Solution**:
```bash
# Try with verbose output
python3 flash_firmware.py -v r36s-firmware-2.0.0.bin

# If still fails, use recovery mode
python3 flash_firmware.py -r --recovery r36s-firmware-2.0.0.bin
```

### Status Shows Disconnected
**Problem**: Device connects then disconnects
**Solution**:
1. Check battery level (R36s needs >10%)
2. Move phone closer to R36s
3. Restart Bluetooth on phone
4. Check firmware version compatibility
5. Reduce interference from other devices

---

## Updating

### Update Phone App
```bash
# Download new version
wget https://github.com/smi9la/androidos-rk/releases/download/v2.x.x/r36s-controller-2.x.x.apk

# Install over existing (keeps settings)
adb install -r r36s-controller-2.x.x.apk
```

### Update R36s Firmware
```bash
# Download new firmware
wget https://github.com/smi9la/androidos-rk/releases/download/v2.x.x/r36s-firmware-2.x.x.bin

# Flash new version
python3 flash_firmware.py r36s-firmware-2.x.x.bin
```

---

## Installation Complete

Your R36s Smart System is now ready to use!

**Next Steps:**
1. [Read User Guide](USER_GUIDE.md)
2. [Check API Documentation](API.md)
3. [Visit Support Forum](https://github.com/smi9la/androidos-rk/issues)

---

**Version**: 2.0.0
**Last Updated**: 2026-06-05