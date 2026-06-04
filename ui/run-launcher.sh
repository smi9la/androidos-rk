#!/bin/bash
# ANDROIDOS-RK Main Launcher - Entry point

set -e

echo "[*] ANDROIDOS-RK Launcher Starting"
echo "[*] Version 1.0.0"
echo ""

# Start power management daemon
echo "[*] Starting power management daemon..."
/usr/local/bin/power-daemon &
POWER_PID=$!
echo "[+] Power daemon started (PID: $POWER_PID)"

# Start ROM scanner (background)
echo "[*] Scanning ROMs..."
/usr/local/bin/rom-scanner &
SCANNER_PID=$!

# Wait for ROM scanner to finish
wait $SCANNER_PID
echo "[+] ROM scan complete"

# Start main UI
echo "[*] Starting launcher UI..."
/usr/local/bin/androidos-launcher

# Cleanup
echo "[*] Shutting down..."
kill $POWER_PID 2>/dev/null || true

echo "[+] ANDROIDOS-RK exited"
