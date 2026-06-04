#!/bin/bash
# Install ANDROIDOS-RK to system

set -e

echo "[*] ANDROIDOS-RK Installation"
echo ""

if [ ! -d "bin" ]; then
  echo "[!] bin/ directory not found. Run ./scripts/compile.sh first"
  exit 1
fi

echo "[*] Installing binaries..."
sudo install -m 0755 bin/power-daemon /usr/local/bin/
sudo install -m 0755 bin/rom-scanner /usr/local/bin/
sudo install -m 0755 bin/androidos-launcher /usr/local/bin/
sudo install -m 0755 ui/run-launcher.sh /usr/local/bin/androidos-rk

echo "[*] Creating system directories..."
sudo mkdir -p /var/log/androidos/{power,input,roms,ui}
sudo mkdir -p /var/cache/androidos
sudo mkdir -p /roms/{psp,ps1,gba,snes,nes,md}
sudo mkdir -p /etc/androidos/controller

echo "[*] Setting up systemd service..."
sudo tee /etc/systemd/system/androidos-rk.service > /dev/null << 'EOF'
[Unit]
Description=ANDROIDOS-RK Launcher
After=network.target

[Service]
Type=simple
ExecStart=/usr/local/bin/androidos-rk
Restart=on-failure
RestartSec=5
User=root

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload

echo ""
echo "[+] Installation complete!"
echo ""
echo "Usage:"
echo "  Direct launch: androidos-rk"
echo "  Service start: systemctl start androidos-rk.service"
echo "  Service enable: systemctl enable androidos-rk.service"
echo ""
