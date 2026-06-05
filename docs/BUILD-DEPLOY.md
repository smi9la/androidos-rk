# Complete Project Build & Deployment Instructions

## Quick Start (All Platforms)

### Prerequisites
- Linux host system (Ubuntu 20.04+ or similar)
- RK3326 handheld device (R36S, R35S)
- MicroSD card (32GB+ recommended)
- USB adapter for flashing

## Build System

### 1. R36S Operating System

```bash
# Clone repository
git clone https://github.com/smi9la/androidos-rk.git
cd androidos-rk

# Setup Buildroot environment
cd buildroot
make BR2_EXTERNAL=../external/ androidos_rk_r36s_defconfig

# Build base system (takes 1-2 hours)
make -j$(nproc)

# Build kernel with patches
cd ../kernel
make rk3326_defconfig
make -j$(nproc) Image dtbs

# Create SD card image
cd ../scripts
./build.sh r36s
./image.sh
./flash.sh /dev/sdX  # Replace sdX with your SD card
```

### 2. PC Desktop Application

```bash
# Windows / macOS / Linux
cd src/pc_app

# Install dependencies
npm install

# Development mode
npm start

# Build for distribution
npm run build
npm run electron-build

# Output in dist/ folder
```

### 3. Mobile Application (Flutter)

```bash
# Requires Flutter SDK
flutter --version

cd src/mobile_app

# Get dependencies
flutter pub get

# iOS
flutter build ios
# or
cd ios && pod install && cd ..
flutter run -d iPhone

# Android
flutter build apk
# or
flutter install
flutter run

# Web (bonus)
flutter build web
```

## File Structure After Build

```
androidos-rk/
├── buildroot/                    # Base filesystem
│   └── output/images/
│       ├── rootfs.tar
│       └── boot.tar
├── kernel/                       # Linux kernel
│   └── output/
│       ├── Image
│       └── dtbs/rk3326-*.dtb
├── src/
│   ├── r36s_os/                 # OS components (C/Python)
│   │   ├── system/              # Core services
│   │   ├── ui/                  # Launcher
│   │   └── api/                 # REST API server
│   ├── pc_app/                  # Electron app (Windows/Mac/Linux)
│   └── mobile_app/              # Flutter app (iOS/Android)
├── build/
│   ├── sdcard.img               # Flashable SD image
│   └── androidos-rk-v1.0.0.tar  # Release archive
└── scripts/
    ├── build.sh                 # Main build script
    ├── image.sh                 # Image creation
    └── flash.sh                 # SD card flashing
```

## Network Access

### Device Discovery

Devices broadcast on UDP port 5555:

```json
{
  "action": "device_announce",
  "device_name": "R36S",
  "ip": "192.168.1.100",
  "port": 8888,
  "model": "R36S",
  "api_version": "1.0"
}
```

### API Server

Runs on port 8888 with:
- REST API at `http://<ip>:8888/api/v1`
- WebSocket at `ws://<ip>:8888/ws`

## Device Flashing

### Linux

```bash
./scripts/flash.sh /dev/sdX
```

### Windows (balena Etcher)

1. Download: https://www.balena.io/etcher/
2. Open `build/sdcard.img`
3. Select SD card
4. Click Flash

### macOS (dd)

```bash
diskutil list
diskutil unmountDisk /dev/diskX
sudo dd if=build/sdcard.img of=/dev/rdiskX bs=4m
```

## First Boot

1. Insert flashed SD card into R36S
2. Power on
3. Wait 30-60 seconds for first boot
4. Launcher appears (Android-like interface)
5. Scan for games (Settings > Storage > Scan ROMs)
6. Enjoy!

## Network Setup

### WiFi Configuration

```bash
# On device (SSH or terminal)
wmctrl
iwconfig
# or via Settings > Network > WiFi
```

### PC App Connection

1. Open PC app
2. Click "Discover Devices"
3. Select R36S from list
4. Click Connect

### Mobile App Connection

1. Open mobile app
2. Go to "Home" tab
3. Click "Discover Devices"
4. Tap device to connect

## Troubleshooting

### SD Card Won't Boot

- Verify image was flashed correctly: `md5sum build/sdcard.img`
- Check SD card compatibility (Class 10 recommended)
- Try another SD card or computer

### Device Not Detected on Network

- Ensure WiFi is connected (check LED)
- Devices broadcast on UDP 5555
- Check firewall settings
- Try static IP assignment

### Games Won't Launch

- Verify ROM format is compatible
- Check game database: `/var/lib/androidos/games.db`
- Enable debug logging: `journalctl -u launcher -f`

### Performance Issues

- Thermal throttling: Check temperature with `cat /sys/class/thermal/thermal_zone0/temp`
- CPU frequency: `cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq`
- Memory: Free unused apps, clear cache

## Development

### Building Custom Kernel Module

```bash
cd kernel
make ARCH=arm64 modules
make ARCH=arm64 modules_install INSTALL_MOD_PATH=../buildroot/output/staging
```

### Adding Custom App

1. Create in `src/r36s_os/apps/myapp/`
2. Register in `src/r36s_os/ui/launcher/launcher.c`
3. Add entry to `/etc/androidos/apps.conf`

### Testing on PC (x86_64)

```bash
# Build for x86_64 instead of ARM
cd buildroot
make pc_defconfig
make -j$(nproc)

# Run in QEMU or directly on Linux
./output/images/vmlinux
```

## License

MIT - See LICENSE file

## Support

For issues:
- GitHub Issues: https://github.com/smi9la/androidos-rk/issues
- Documentation: https://github.com/smi9la/androidos-rk/tree/main/docs
