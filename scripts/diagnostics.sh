#!/bin/bash
# ANDROIDOS-RK System Diagnostics

echo "====================================="
echo "ANDROIDOS-RK System Diagnostics"
echo "====================================="
echo ""

echo "[*] System Information"
uname -a
echo ""

echo "[*] Installed Components"
echo "Binaries:"
ls -lh /usr/local/bin/androidos-* /usr/local/bin/power-daemon /usr/local/bin/rom-scanner 2>/dev/null || echo "  Not installed"
echo ""

echo "[*] Dependencies"
echo "SDL2: $(pkg-config --modversion sdl2 2>/dev/null || echo 'Not found')"
echo "SQLite: $(sqlite3 --version 2>/dev/null || echo 'Not found')"
echo "GCC: $(gcc --version | head -1)"
echo ""

echo "[*] File Structure"
echo "Log directories:"
find /var/log/androidos -type d 2>/dev/null || echo "  Not found"
echo ""
echo "ROM directories:"
find /roms -type d 2>/dev/null | head -10 || echo "  Not found"
echo ""

echo "[*] Latest Logs"
echo "Power daemon:"
tail -5 /var/log/androidos/power/daemon.log 2>/dev/null || echo "  Not found"
echo ""
echo "Launcher UI:"
tail -5 /var/log/androidos/ui/launcher.log 2>/dev/null || echo "  Not found"
echo ""

echo "[*] Hardware Check"
echo "Input devices:"
ls -la /dev/input/event* 2>/dev/null | head -5
echo ""
echo "Power supply:"
ls -la /sys/class/power_supply/ 2>/dev/null
echo ""
echo "Thermal zones:"
ls -la /sys/class/thermal/ 2>/dev/null
echo ""

echo "====================================="
