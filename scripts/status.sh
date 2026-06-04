#!/bin/bash
# ANDROIDOS-RK System Status

echo "====================================="
echo "ANDROIDOS-RK System Status"
echo "====================================="
echo ""

echo "[*] Power Management"
sudo /usr/local/bin/power-daemon 2>&1 | head -20 || echo "  Not running"
echo ""

echo "[*] Process Status"
ps aux | grep -E 'power-daemon|androidos-launcher' | grep -v grep || echo "  No processes running"
echo ""

echo "[*] Log Files"
echo "Power logs:"
ls -lh /var/log/androidos/power/ 2>/dev/null || echo "  Not found"
echo ""
echo "Input logs:"
ls -lh /var/log/androidos/input/ 2>/dev/null || echo "  Not found"
echo ""
echo "ROM logs:"
ls -lh /var/log/androidos/roms/ 2>/dev/null || echo "  Not found"
echo ""
echo "UI logs:"
ls -lh /var/log/androidos/ui/ 2>/dev/null || echo "  Not found"
echo ""

echo "[*] ROM Database"
echo "Total ROMs:"
ls -1 /roms/*/ 2>/dev/null | wc -l || echo "  0"
echo ""

echo "[*] Hardware Status"
echo "Battery: $(cat /sys/class/power_supply/battery/capacity 2>/dev/null || echo 'N/A')%"
echo "Temperature: $(cat /sys/class/thermal/thermal_zone0/temp 2>/dev/null | awk '{print $1/1000}')°C"
echo "CPU Freq: $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq 2>/dev/null | awk '{print $1/1000}') MHz"
echo ""

echo "====================================="
