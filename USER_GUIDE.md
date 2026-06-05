# R36s Smart System - User Guide

## 🎮 Controller App Overview

The R36s Smart Controller is your gateway to managing and monitoring your R36s device from your Android phone.

---

## 🚀 Getting Started

### First Launch
1. Open **R36s Smart Controller** app
2. Accept permission requests
3. App shows: "Ready to connect to R36s"

### Connecting to R36s
1. Tap **"Scan for R36s Devices"**
2. Wait 10 seconds for scan to complete
3. Select your device from the list
4. Confirm pairing on both devices
5. Status changes to **"Connected"**

---

## 📊 Dashboard

The main screen shows:

```
┌─────────────────────┐
│ 🎮 R36s Smart       │ ← Title
├─────────────────────┤
│ Status: Connected   │ ← Connection status
│ Battery: 85%        │ ← Battery level
│ Uptime: 2h 34m      │ ← Device uptime
├─────────────────────┤
│ [Refresh]           │ ← Manual refresh
│ [Settings]          │ ← Open settings
│ [Help]              │ ← Get help
└─────────────────────┘
```

---

## ⚙️ Main Features

### 1. Status Monitoring
- **Battery Level** - Current charge percentage
- **Connection** - Signal strength and stability
- **Uptime** - How long device has been running
- **Temperature** - Device CPU temperature

**How to use:**
1. Tap "Refresh" to update information
2. Status updates every 30 seconds automatically

### 2. Device Control
- **Power On/Off** - Control device power
- **Reboot** - Restart R36s
- **Reset** - Factory reset

**How to use:**
1. Tap the device name
2. Choose desired action
3. Confirm on R36s display

### 3. Settings Management
- **Bluetooth Name** - Change device Bluetooth name
- **Display Brightness** - Adjust screen brightness
- **Update Firmware** - Install new versions

**How to use:**
1. Tap "Settings"
2. Adjust desired options
3. Changes apply immediately

---

## 🔧 Advanced Features

### Firmware Updates
```
Settings → Firmware → Check for Updates
```

- App automatically checks for new versions
- Download and install with one tap
- Device reboots after update completes

### Display Customization
```
Settings → Display → Brightness
```

- Adjust brightness: 1-100%
- Change color scheme
- Enable/disable animations

### Data Sync
```
Settings → Sync → Enable Cloud Backup
```

- Sync settings across devices
- Automatic backup schedule
- Restore from backup

---

## 📱 Menu Navigation

### Main Menu
- 🏠 Home - Dashboard view
- 🔧 Settings - Configure app
- 📊 Stats - View statistics
- ℹ️ About - App information

### Settings Submenu
- 🔗 Bluetooth - Connection options
- 🎨 Display - Visual settings
- 🔔 Notifications - Alert preferences
- 📦 Storage - Manage data

---

## 🔐 Security

### Pairing
- Device generates random PIN for pairing
- PIN valid for 5 minutes
- Pairing cannot be replayed

### Data Encryption
- All communications encrypted with AES-256
- No data stored unencrypted on device
- Local storage only (no cloud)

### Permissions
App requests:
- **Bluetooth** - To connect to R36s
- **Location** - For Bluetooth scanning
- **Storage** - To save configuration

---

## 🐛 Troubleshooting

### "Device Not Found"
1. Ensure R36s Bluetooth is enabled
2. Bring phone closer to R36s
3. Restart both devices
4. Clear app cache: Settings → Apps → R36s Controller → Storage → Clear Cache

### "Connection Lost"
1. Move closer to R36s (better signal)
2. Remove obstacles between phone and device
3. Restart Bluetooth on phone
4. Charge R36s battery (low power may cause disconnection)

### "Slow Response"
1. Move closer to R36s (better signal)
2. Close other Bluetooth devices nearby
3. Restart the app
4. Update firmware to latest version

### "App Crashes"
1. Update to latest version
2. Clear app data: Settings → Apps → R36s Controller → Storage → Clear Data
3. Reinstall app if issue persists

---

## 🔄 Background Service

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
- Stops automatically when phone battery low

---

## 💡 Tips & Tricks

### Quick Connect
- App remembers last connected device
- Next launch auto-connects if available

### Battery Optimization
- Close other Bluetooth apps
- Reduce notification frequency
- Disable background sync when not needed

### Performance
- Update app regularly for improvements
- Keep device firmware current
- Restart app weekly for best performance

---

## 📞 Getting Help

### In-App Help
1. Tap "Help" button
2. Browse common questions
3. View video tutorials

### Online Resources
- **GitHub Issues**: https://github.com/smi9la/androidos-rk/issues
- **Documentation**: https://github.com/smi9la/androidos-rk/wiki
- **Forum**: https://github.com/smi9la/androidos-rk/discussions

### Report Problems
1. Tap Menu → About → Report Issue
2. Describe problem
3. Include logs and device info
4. Submit to GitHub

---

## 📋 Keyboard Shortcuts (R36s Device)

- **Power** - Hold 3s to power off
- **Reset** - Hold 5s to reboot
- **Menu** - Short press to open menu
- **Back** - Return to previous screen

---

## 🎯 System Requirements

**Minimum:**
- Android 6.0 or higher
- Bluetooth 4.2
- 100 MB free storage

**Recommended:**
- Android 10 or higher
- Bluetooth 5.0
- 500 MB free storage

---

**Version**: 2.0.0
**Last Updated**: 2026-06-05
