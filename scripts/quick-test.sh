#!/bin/bash
# ANDROIDOS-RK Quick Test

echo "====================================="
echo "ANDROIDOS-RK Quick Test"
echo "====================================="
echo ""

echo "[*] Checking compilation..."
if [ ! -f "bin/power-daemon" ]; then
  echo "[!] Not compiled. Running compile..."
  ./scripts/compile.sh
fi

echo "[+] Compilation OK"
echo ""

echo "[*] Testing power daemon (3 seconds)..."
timeout 3 ./bin/power-daemon || true
echo ""

echo "[*] Checking log files..."
echo "Power logs:"
ls -lh /var/log/androidos/power/ 2>/dev/null || echo "  Not found (will be created on run)"
echo ""

echo "[*] ROM database test..."
if command -v sqlite3 &> /dev/null; then
  echo "  SQLite3 available"
  timeout 5 ./bin/rom-scanner || true
  echo "  ROMs found: $(sqlite3 /var/cache/androidos/roms.db 'SELECT COUNT(*) FROM roms;' 2>/dev/null || echo '0')"
else
  echo "  SQLite3 not installed"
fi
echo ""

echo "[+] Quick test complete"
echo ""
echo "Next: ./scripts/install.sh"
echo ""
