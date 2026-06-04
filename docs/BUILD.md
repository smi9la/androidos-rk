# ANDROIDOS-RK Build Guide

## Prerequisites

### Required Tools

```bash
# Ubuntu/Debian
sudo apt-get install -y \
  build-essential \
  git \
  wget \
  curl \
  python3 \
  python3-dev \
  perl \
  rsync \
  bc \
  bison \
  flex \
  libssl-dev \
  libncurses-dev \
  device-tree-compiler \
  u-boot-tools \
  qemu-user-static \
  debootstrap

# Arch Linux
sudo pacman -S base-devel git wget curl python3 rsync bc bison flex libelf uboot-tools
```

### System Requirements

- **Disk Space**: 50+ GB free (for Buildroot build)
- **RAM**: 4+ GB minimum (8+ GB recommended)
- **Processor**: Multi-core processor (build parallelization)
- **OS**: Linux (Ubuntu 20.04 LTS, Debian 11+, or Arch)

---

## Build Steps

### Step 1: Clone Repository

```bash
git clone https://github.com/smi9la/androidos-rk.git
cd androidos-rk
```

### Step 2: Initialize Buildroot

```bash
./scripts/init-buildroot.sh
```

This script:
- Downloads Buildroot 2023.02 (LTS)
- Applies ANDROIDOS-RK patches
- Sets up cross-compilation toolchain
- Downloads kernel sources

### Step 3: Configure Build

```bash
# For R36S (default)
./scripts/configure.sh r36s

# For R35S
./scripts/configure.sh r35s
```

This creates device-specific configurations.

### Step 4: Build Everything

```bash
./scripts/build.sh r36s
```

**Build Time**: 60-120 minutes (first build), 5-15 minutes (incremental)

**Output:**
- `buildroot/output/images/sdcard.img` — Bootable SD card image
- `buildroot/output/images/uImage` — Kernel image
- `buildroot/output/rootfs/` — Root filesystem

### Step 5: Create SD Card Image

```bash
./scripts/image.sh r36s
```

Creates final bootable SD card image at:
```
output/androidos-rk-r36s.img
```

### Step 6: Flash to SD Card

```bash
# IMPORTANT: Replace sdX with your actual SD card device!
# Use 'lsblk' to identify the correct device

sudo ./scripts/flash.sh /dev/sdX
```

**Warning**: This will overwrite the SD card. Verify device carefully!

---

## Build Customization

### Modify Buildroot Configuration

```bash
cd buildroot
make menuconfig
```

Navigate to:
- **System Configuration** → Device settings
- **Kernel** → Kernel options
- **Target Packages** → Emulators, utilities

### Add Custom Packages

Edit `buildroot/local.mk`:

```makefile
# Enable additional packages
BRAPP_PPSSPP=y
BRAPP_RETROARCH=y
BRAPP_MPVC=y
```

### Patch Kernel

Place patches in `kernel/patches/`:

```bash
kernel/patches/
├── 0001-fix-something.patch
├── 0002-optimize-gpu.patch
└── ...
```

Patch order is numerical.

---

## Verification

### Test Build Artifacts

```bash
# Verify kernel
file buildroot/output/images/uImage

# Verify rootfs
ls -lh buildroot/output/images/rootfs.*

# Verify image
file output/androidos-rk-r36s.img
```

### Calculate Image Checksums

```bash
sha256sum output/androidos-rk-r36s.img > output/androidos-rk-r36s.img.sha256
md5sum output/androidos-rk-r36s.img > output/androidos-rk-r36s.img.md5
```

---

## Troubleshooting

### Build Fails with Permission Error

```bash
# Ensure user is in docker group (if using Docker)
sudo usermod -aG docker $USER
newgrp docker
```

### Out of Disk Space

```bash
# Clean build artifacts
cd buildroot
make clean

# Or full distclean
make distclean
```

### Download Failures

```bash
# Retry downloads
cd buildroot
make source
```

### Kernel Compilation Fails

```bash
# Check kernel config
cd buildroot
make linux-reconfigure

# View errors
make linux-show-build-log
```

---

## Incremental Builds

### Rebuild Just the Rootfs

```bash
./scripts/build.sh r36s --rootfs-only
```

### Rebuild Just the Kernel

```bash
./scripts/build.sh r36s --kernel-only
```

### Rebuild Specific Package

```bash
cd buildroot
make ppsspp-rebuild
```

---

## Cross-Compilation

The build system automatically handles ARM cross-compilation for RK3326 (ARMv7).

**Toolchain Details:**
- Target: `arm-buildroot-linux-gnueabihf`
- Kernel: 5.15 LTS
- C Library: glibc
- C Compiler: GCC 12.x

---

## Binary Package Generation

For distribution:

```bash
./scripts/package.sh
```

Creates:
- `dist/androidos-rk-r36s.img.zip` — Compressed image
- `dist/androidos-rk-r36s.img.sha256` — Checksum
- `dist/INSTALL.md` — Installation guide

---

## Development Builds

### Debug Symbols

```bash
./scripts/build.sh r36s --debug
```

Includes debug symbols for GDB, valgrind, etc.

### Verbose Output

```bash
cd buildroot
make BR2_VERBOSE=1
```

### Timing Analysis

```bash
cd buildroot
make BR2_INSTRUMENTATION=y
```

---

## CI/CD Integration

See `.github/workflows/` for automated builds on commit.

---

## Next Steps

1. **First Boot**: See `docs/FIRST-BOOT.md`
2. **Hardware Setup**: See `docs/HARDWARE.md`
3. **Configuration**: See `docs/CONFIGURATION.md`
4. **ROM Setup**: See `docs/ROM-SETUP.md`
