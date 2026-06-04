#!/bin/bash
# Compile ANDROIDOS-RK System

set -e

echo "[*] ANDROIDOS-RK Build"
echo "[*] Compiling all modules..."
echo ""

COMPILE_FLAGS="-Wall -Wextra -O2 -fPIC"
LINK_FLAGS="-lsqlite3 -lSDL2"

# Create output directories
mkdir -p bin

# Compile Power Management
echo "[*] Building power management..."
gcc $COMPILE_FLAGS -c power/battery.c -o power/battery.o
gcc $COMPILE_FLAGS -c power/thermal.c -o power/thermal.o
gcc $COMPILE_FLAGS -c power/cpufreq.c -o power/cpufreq.o
gcc $COMPILE_FLAGS -c power/power-daemon.c -o power/power-daemon.o
gcc power/battery.o power/thermal.o power/cpufreq.o power/power-daemon.o -o bin/power-daemon $LINK_FLAGS
echo "[+] Power daemon compiled"

# Compile Input System
echo "[*] Building input system..."
gcc $COMPILE_FLAGS -c input/input-manager.c -o input/input-manager.o
gcc $COMPILE_FLAGS -c input/controller-map.c -o input/controller-map.o
gcc $COMPILE_FLAGS -c input/hotkey-handler.c -o input/hotkey-handler.o
echo "[+] Input system compiled"

# Compile ROM System
echo "[*] Building ROM system..."
gcc $COMPILE_FLAGS -c roms/rom-scanner.c -o roms/rom-scanner.o
gcc $COMPILE_FLAGS -c roms/rom-launcher.c -o roms/rom-launcher.o
gcc roms/rom-scanner.o roms/rom-launcher.o -o bin/rom-scanner $LINK_FLAGS
echo "[+] ROM system compiled"

# Compile UI
echo "[*] Building launcher UI..."
gcc $COMPILE_FLAGS -c ui/launcher.c -o ui/launcher.o
gcc ui/launcher.o power/battery.o power/thermal.o power/cpufreq.o input/input-manager.o roms/rom-scanner.o roms/rom-launcher.o -o bin/androidos-launcher $LINK_FLAGS
echo "[+] Launcher UI compiled"

echo ""
echo "[+] Build complete!"
echo ""
echo "Output files:"
ls -lh bin/
echo ""
