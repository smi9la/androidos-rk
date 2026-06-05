# R36s Smart System - User Guide

## Controller App Overview

The R36s Smart Controller is your gateway to managing and monitoring your R36s device from your Android phone.

---

## Getting Started

### First Launch
1. Open **R36s Smart Controller** app
2. Accept permission requests when prompted
3. App displays: "Ready to connect to R36s"

### Connecting to R36s
1. Tap **"Scan for R36s Devices"**
2. Wait 10 seconds for scan to complete
3. Select your device from the list
4. Confirm pairing on both devices
5. Status changes to **"Connected"**

---

## Dashboard Overview

The main screen displays:

```
┌────────────────────────────┐
│ 🎮 R36s Smart              │ ← Title
├────────────────────────────┤
│ Status: Connected          │ ← Connection status
│ Battery: 85%               │ ← Battery level
│ Uptime: 2h 34m             │ ← Device uptime
├────────────────────────────┤
│ [Refresh]                  │ ← Manual refresh
│ [Settings]                 │ ← Open settings
│ [Help]                     │ ← Get help
└────────────────────────────┘
```

---

## Main Features

### 1. Status Monitoring
- **Battery Level** - Current charge percentage (0-100%)
- **Connection** - Signal strength and stability
- **Uptime** - How long device has been running
- **Temperature** - Device CPU temperature

**How to use:**
1. Tap "Refresh" to update information immediately
2. Status automatically updates every 30 seconds

### 2. Device Control
- **Power On/Off** - Control device power state
- **Reboot** - Restart R36s device
- **Reset** - Perform factory reset

**How to use:**
1. Tap the device name in the header
2. Choose desired action from menu
3. Confirm action on R36s display

### 3. Settings Management
- **Bluetooth Name** - Change device Bluetooth name
- **Display Brightness** - Adjust screen brightness (1-100%)
- **Update Firmware** - Install new firmware versions

**How to use:**
1. Tap "Settings" button
2. Adjust desired options
3. Changes apply immediately

---

## Advanced Features

### Firmware Updates
```
Settings → Firmware → Check for Updates
```

- App automatically checks for new versions
- Download and install with one tap
- Device reboots after update completes
- No data loss during update

### Display Customization
```
Settings → Display → Brightness
```

- Adjust brightness: 1-100%
- Change color theme
- Enable/disable animations
- Customize update intervals

### Data Synchronization
```
Settings → Sync → Enable Cloud Backup
```

- Sync settings across devices
- Automatic backup schedule
- Restore from previous backups
- Local storage only option

---

## Menu Navigation

### Main Menu
- 🏠 Home - Dashboard and status view
- 🔧 Settings - Configure app and device
- 📊 Stats - View device statistics
- ℹ️ About - App information and credits

### Settings Submenu
- 🔗 Bluetooth - Connection and pairing options
- 🎨 Display - Visual settings and themes
- 🔔 Notifications - Alert and notification preferences
- 📦 Storage - Manage app data and cache

---

## Security

### Device Pairing
- Device generates random PIN for pairing
- PIN valid for 5 minutes only
- Pairing cannot be replayed or intercepted

### Data Encryption
- All communications encrypted with AES-256
- No data stored unencrypted on device
- Local storage only (no cloud sync)
- Automatic data cleanup on disconnect

### Permissions
The app requests:
- **Bluetooth** - To connect to R36s device
- **Location** - For Bluetooth device scanning
- **Storage** - To save configuration and data

---

## Troubleshooting

### "Device Not Found"
1. Ensure R36s Bluetooth is enabled
2. Bring phone closer to R36s
3. Restart both devices
4. Clear app cache: Settings → Apps → R36s Controller → Storage → Clear Cache

### "Connection Lost"
1. Move phone closer to R36s (better signal)
2. Remove obstacles between devices
3. Restart Bluetooth on phone
4. Charge R36s battery (low power may cause disconnection)
5. Update both app and firmware

### "Slow Response"
1. Move closer to R36s for better signal
2. Close other Bluetooth apps
3. Restart the R36s Smart Controller app
4. Update firmware to latest version
5. Check for interference from other devices

### "App Crashes"
1. Update to latest version from Play Store
2. Clear app data: Settings → Apps → R36s Controller → Storage → Clear Data
3. Reinstall app if issue persists
4. Report crash details to GitHub Issues

---

## Background Service

The app can run in the background:

**Features:**
- Continuous connection monitoring
- Automatic reconnection if disconnected
- Battery status notifications
- Firmware update reminders

**To enable:**
```
Settings → Background Service → Enable
```

**Battery Impact:**
- Minimal battery drain (1-2% per hour)
- Syncs data every 5 minutes
- Automatically stops when phone battery is low

---

## Tips & Tricks

### Quick Connect
- App remembers last connected device
- Next launch auto-connects if available
- One-tap reconnect to favorite devices

### Battery Optimization
- Close other Bluetooth apps before connecting
- Reduce notification frequency
- Disable background sync when not needed
- Use WiFi instead of Bluetooth for faster sync

### Performance
- Update app regularly for improvements
- Keep device firmware current
- Restart app weekly for best performance
- Clear cache monthly

---

## Getting Help

### In-App Help
1. Tap "Help" button
2. Browse common questions
3. View video tutorials
4. Contact support team

### Online Resources
- **GitHub Issues**: https://github.com/smi9la/androidos-rk/issues
- **Documentation**: https://github.com/smi9la/androidos-rk/wiki
- **Forum**: https://github.com/smi9la/androidos-rk/discussions

### Report Problems
1. Tap Menu → About → Report Issue
2. Describe the problem clearly
3. Include logs and device information
4. Submit to GitHub

---

## Device Buttons (R36s)

- **Power** - Hold 3 seconds to power off
- **Reset** - Hold 5 seconds to reboot
- **Menu** - Short press to open menu
- **Back** - Return to previous screen

---

## System Requirements

**Minimum:**
- Android 6.0 or higher
- Bluetooth 4.2
- 100 MB free storage

**Recommended:**
- Android 10 or higher
- Bluetooth 5.0
- 500 MB free storage
- 2GB+ RAM

---

**Version**: 2.0.0
**Last Updated**: 2026-06-05