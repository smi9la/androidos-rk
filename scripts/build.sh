#!/bin/bash
# Build ANDROIDOS-RK system

set -e

DEVICE=${1:-r36s}
MODE=${2:-full}

echo "[*] ANDROIDOS-RK Build System"
echo "[*] Target Device: $DEVICE"
echo "[*] Build Mode: $MODE"
echo ""

if [ ! -f "configs/$DEVICE.config" ]; then
  echo "❌ Configuration not found for $DEVICE"
  echo "Run: ./scripts/configure.sh $DEVICE"
  exit 1
fi

echo "[*] Starting build..."
echo "[*] Build directory: buildroot/buildroot-2023.02"
echo ""

# Trigger Buildroot build
cd buildroot/buildroot-2023.02

echo "[*] Loading configuration..."
make defconfig BR2_DEFCONFIG="../../configs/$DEVICE.config"

echo "[*] Building (this may take 60-120 minutes)..."
make -j$(nproc)

echo ""
echo "[✓] Build completed successfully!"
echo ""
echo "Output artifacts:"
echo "  - Kernel: output/images/uImage"
echo "  - Root filesystem: output/images/rootfs.ext4"
echo "  - Bootloader: output/images/u-boot.img"
echo ""
echo "Next step:"
echo "  ./scripts/image.sh $DEVICE"
echo ""
