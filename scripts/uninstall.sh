#!/bin/bash
# Uninstall ANDROIDOS-RK

echo "[!] ANDROIDOS-RK Uninstall"
echo ""
echo "This will remove ANDROIDOS-RK from your system."
read -p "Continue? (y/N) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
  echo "Aborted."
  exit 1
fi

echo "[*] Stopping services..."
sudo systemctl stop androidos-rk.service 2>/dev/null || true
sudo systemctl disable androidos-rk.service 2>/dev/null || true

echo "[*] Killing processes..."
sudo pkill -9 power-daemon 2>/dev/null || true
sudo pkill -9 androidos-launcher 2>/dev/null || true

echo "[*] Removing binaries..."
sudo rm -f /usr/local/bin/power-daemon
sudo rm -f /usr/local/bin/rom-scanner
sudo rm -f /usr/local/bin/androidos-launcher
sudo rm -f /usr/local/bin/androidos-rk

echo "[*] Removing service file..."
sudo rm -f /etc/systemd/system/androidos-rk.service
sudo systemctl daemon-reload

echo "[*] Removing logs..."
sudo rm -rf /var/log/androidos

echo "[+] Uninstall complete"
echo "[!] ROM files and user data preserved in /roms and /etc/androidos"
