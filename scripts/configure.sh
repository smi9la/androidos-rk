#!/bin/bash
# Configure ANDROIDOS-RK build for target device

set -e

DEVICE=${1:-r36s}

echo "[*] ANDROIDOS-RK Configuration"
echo "[*] Target Device: $DEVICE"
echo ""

if [ "$DEVICE" != "r36s" ] && [ "$DEVICE" != "r35s" ]; then
  echo "❌ Unknown device: $DEVICE"
  echo "Supported: r36s, r35s"
  exit 1
fi

# Create device-specific configuration
echo "[*] Generating device configuration for $DEVICE..."

cat > "configs/$DEVICE.config" << 'EOF'
# ANDROIDOS-RK Buildroot Configuration

# Target architecture
BR2_arm=y
BR2_cortex_a35=y
BR2_ARM_ENABLE_NEON=y
BR2_ARM_INSTRUCTIONS_THUMB2=y
BR2_ARM_INSTRUCTIONS_THUMB=y

# Toolchain
BR2_TOOLCHAIN_BUILDROOT=y
BR2_GCC_VERSION_12_X=y
BR2_BINUTILS_VERSION_2_38_X=y
BR2_GLIBC_VERSION_2_36_X=y

# Kernel
BR2_LINUX_KERNEL=y
BR2_LINUX_KERNEL_CUSTOM_VERSION=y
BR2_LINUX_KERNEL_CUSTOM_VERSION_VALUE="5.15.108"
BR2_LINUX_KERNEL_USE_CUSTOM_CONFIG=y
BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE="../../kernel/rk3326.config"

# Root filesystem
BR2_TARGET_ROOTFS_EXT4=y
BR2_TARGET_ROOTFS_EXT4_SIZE="512M"

# Packages
BR2_PACKAGE_OPENSSL=y
BR2_PACKAGE_ZLIB=y
BR2_PACKAGE_SDL2=y
BR2_PACKAGE_SDL2_OPENGLES=y
BR2_PACKAGE_FFMPEG=y
BR2_PACKAGE_ALSA_LIB=y
BR2_PACKAGE_SQLITE=y
BR2_PACKAGE_CURL=y
BR2_PACKAGE_BUSYBOX=y
BR2_PACKAGE_MTD=y
BR2_PACKAGE_UTIL_LINUX=y

# Init system
BR2_INIT_SYSTEMD=y
BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_EUDEV=y

# System configuration
BR2_TARGET_GENERIC_HOSTNAME="androidos-rk"
BR2_TARGET_GENERIC_ISSUE="ANDROIDOS-RK"
BR2_ROOTFS_POST_IMAGE_SCRIPT="support/scripts/generic-post-process.sh"

# Bootloader
BR2_TARGET_UBOOT=y
BR2_TARGET_UBOOT_BUILD_SYSTEM_KCONFIG=y
BR2_TARGET_UBOOT_CUSTOM_VERSION=y
BR2_TARGET_UBOOT_CUSTOM_VERSION_VALUE="2023.04"
BR2_TARGET_UBOOT_FORMAT_IMG=y
EOF

echo "[✓] Configuration generated: configs/$DEVICE.config"
echo ""
echo "Next step:"
echo "  ./scripts/build.sh $DEVICE"
echo ""
