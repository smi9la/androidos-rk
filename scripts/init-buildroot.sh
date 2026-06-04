#!/bin/bash
# Initialize Buildroot for ANDROIDOS-RK build

set -e

echo "[*] ANDROIDOS-RK Buildroot Initialization"
echo "[*] Target: RK3326 (R36S/R35S)"
echo ""

# Check prerequisites
echo "[*] Checking prerequisites..."
command -v git >/dev/null 2>&1 || { echo "❌ git not found"; exit 1; }
command -v wget >/dev/null 2>&1 || { echo "❌ wget not found"; exit 1; }
command -v tar >/dev/null 2>&1 || { echo "❌ tar not found"; exit 1; }
echo "[✓] All prerequisites found"

# Create buildroot directory
if [ ! -d "buildroot" ]; then
  mkdir -p buildroot
fi

cd buildroot

# Download Buildroot 2023.02 (LTS)
if [ ! -d "buildroot-2023.02" ]; then
  echo "[*] Downloading Buildroot 2023.02 LTS..."
  wget -q https://buildroot.org/downloads/buildroot-2023.02.tar.gz
  echo "[*] Extracting..."
  tar xzf buildroot-2023.02.tar.gz
  rm buildroot-2023.02.tar.gz
  echo "[✓] Buildroot extracted"
fi

cd buildroot-2023.02

# Create ANDROIDOS-RK configuration
echo "[*] Creating ANDROIDOS-RK configuration..."

mkdir -p board/androidos-rk
mkdir -p configs

echo "[✓] Buildroot initialized successfully"
echo ""
echo "Next steps:"
echo "  1. ./scripts/configure.sh r36s   # Configure for R36S"
echo "  2. ./scripts/build.sh r36s       # Build system"
echo ""
