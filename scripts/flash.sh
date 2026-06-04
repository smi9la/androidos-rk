#!/bin/bash
# Flash ANDROIDOS-RK to SD card

set -e

if [ $# -lt 1 ]; then
  echo "Usage: $0 /dev/sdX"
  echo ""
  echo "⚠️  WARNING: This will overwrite the target device!"
  echo ""
  echo "List your devices:"
  lsblk
  exit 1
fi

DEVICE=$1
IMAGE="output/androidos-rk-r36s.img"

if [ ! -f "$IMAGE" ]; then
  echo "❌ Image not found: $IMAGE"
  echo "Run: ./scripts/image.sh r36s"
  exit 1
fi

echo "[!] ANDROIDOS-RK SD Card Flasher"
echo "[!] ⚠️  WARNING: This will OVERWRITE $DEVICE"
echo "[!] Make sure this is the correct device!"
echo ""

# Unmount device
echo "[*] Unmounting $DEVICE..."
sudo umount $DEVICE* 2>/dev/null || true

echo "[*] Flashing $IMAGE to $DEVICE (this may take 5-15 minutes)..."
sudo dd if="$IMAGE" of="$DEVICE" bs=4M status=progress

echo ""
echo "[*] Syncing..."
sudo sync

echo ""
echo "[✓] Flash completed!"
echo ""
echo "Next steps:"
echo "  1. Remove SD card from computer"
echo "  2. Insert into R36S device"
echo "  3. Power on device"
echo "  4. ANDROIDOS-RK should boot!"
echo ""
