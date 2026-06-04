# RK3326 Hardware Reference

## Processor

- **Chipset**: Rockchip RK3326
- **CPU**: ARM Cortex-A35 (4 cores)
- **Frequency**: 1.5 GHz (typical), up to 1.8 GHz boost
- **Architecture**: ARMv8-A 32-bit
- **L1 Cache**: 32KB per core
- **L2 Cache**: 512KB shared

## Graphics

- **GPU**: Mali-400MP2
- **API Support**: OpenGL ES 2.0, OpenGL ES 3.0
- **Memory**: Shared system memory (no dedicated VRAM)
- **Clock**: 400 MHz typical

## Memory

- **RAM**: 1GB or 2GB LPDDR3
- **Storage**: MicroSD (up to 1TB)
- **Boot**: 8MB SPI NOR Flash (optional)

## Power Management

- **Battery**: Li-Po 3.7V typical
- **Capacity**: 1200-3500 mAh (device-dependent)
- **Charging**: USB Type-C (5V/2A typical)
- **Power Monitoring**: PMU (Power Management Unit) via I2C

### Power Consumption

- **Idle**: 50-100 mW
- **Gaming**: 1-3 W
- **Max**: 5-7 W

### Battery Voltage Monitoring

Read from `/sys/class/power_supply/battery/`:

```
voltage_now - Current voltage (microvolts)
current_now - Current draw (microamps)
capacity - Battery percentage (0-100)
status - Charging state (Charging, Discharging, etc.)
temp - Temperature (°C)
```

## Audio

- **Codec**: Internal audio codec (Rockchip RK3026 or similar)
- **Output**: 3.5mm headphone jack (mono/stereo)
- **Speaker**: Mono speaker (typical)
- **Recording**: Internal microphone (optional)

## Input Devices

### Game Controller

- **Type**: Analog joysticks + digital buttons
- **Interface**: GPIO-based or SPI
- **Mappings**:
  - D-Pad: GPIO pins
  - Analog Sticks: ADC inputs
  - Buttons: GPIO pins
  - Shoulders: GPIO pins

### Touchscreen (if present)

- **Type**: Capacitive or resistive
- **Interface**: I2C or SPI
- **Resolution**: Device-dependent
- **Driver**: Kernel input subsystem

## Display

- **Type**: IPS LCD or OLED
- **Resolution**: 640x480 (typical)
- **Size**: 3.5"-3.7" diagonal
- **Color Depth**: 24-bit (8-8-8)
- **Interface**: Parallel RGB or LVDS
- **Refresh**: 60 Hz

### Display Brightness Control

Via PWM on GPIO or dedicated backlight controller:

```bash
echo N > /sys/class/backlight/*/brightness
```

## Connectivity

### WiFi/Bluetooth

- **Module**: Realtek RTL8723/RTL8821 or MediaTek
- **Interface**: SDIO (for WiFi), UART (for Bluetooth)
- **Standards**:
  - WiFi: 802.11b/g/n
  - Bluetooth: 4.2

### USB

- **Type**: USB 2.0
- **Ports**: 1x Micro USB or USB Type-C
- **Functions**: Mass storage, ADB (Android Debug Bridge)

## Thermal Management

- **Thermal Zones**: CPU zone monitored
- **Throttling**: Automatic frequency reduction above 80°C
- **Shutdown**: Forced shutdown above 95°C
- **Cooling**: Passive (no active fan)

### Temperature Monitoring

```bash
cat /sys/class/thermal/thermal_zone0/temp
# Returns temperature in millidegrees Celsius
```

## Boot Process

1. **Mask ROM** (in chip) → Boots DDR initialization
2. **DDR Init** → Initializes RAM
3. **U-Boot** → Secondary bootloader
4. **Kernel** → Linux kernel loads
5. **Rootfs** → Root filesystem mounted
6. **Init System** → systemd or OpenRC starts
7. **User Space** → ANDROIDOS-RK services start

## Device Tree

RK3326 device tree source: `kernel/dts/rk3326.dtsi`

Key nodes:

```
/ {
  cpus { /* CPU cores */ }
  gpu { /* Mali GPU */ }
  soc { /* SoC peripherals */ }
  memory { /* Memory layout */ }
  chosen { /* Boot parameters */ }
}
```

## Pin Multiplexing (Pinctrl)

Configured in device tree. Key pins for R36S:

| Function | GPIO | Interface |
|----------|------|----------|
| D-Pad Up | GPIO0_A2 | GPIO |
| D-Pad Down | GPIO0_A3 | GPIO |
| D-Pad Left | GPIO0_B0 | GPIO |
| D-Pad Right | GPIO0_B1 | GPIO |
| Button A | GPIO1_A2 | GPIO |
| Button B | GPIO1_A3 | GPIO |
| Button X | GPIO1_B0 | GPIO |
| Button Y | GPIO1_B1 | GPIO |
| L Shoulder | GPIO0_C2 | GPIO |
| R Shoulder | GPIO0_C3 | GPIO |
| Start | GPIO1_C0 | GPIO |
| Select | GPIO1_C1 | GPIO |
| Left Analog X | ADC0_IN0 | ADC |
| Left Analog Y | ADC0_IN1 | ADC |
| Right Analog X | ADC1_IN0 | ADC |
| Right Analog Y | ADC1_IN1 | ADC |

*(Exact pins vary by device; verify with device tree)*

## Interrupt Handling

Key interrupt sources:

- **GPIO Interrupts**: Button presses
- **ADC Interrupts**: Analog stick changes
- **I2C Interrupts**: Touchscreen/PMU events
- **Timer Interrupts**: System tick (100 Hz)

## Clock Tree

Main clocks managed by CCU (Clock Control Unit):

- **ARM Clock**: Up to 1.8 GHz
- **GPU Clock**: Up to 400 MHz
- **DDR Clock**: Up to 800 MHz
- **Peripheral Clocks**: Various

Frequency scaling managed by DVFS (Dynamic Voltage and Frequency Scaling).

## Voltage Domains

- **CPU**: 0.9-1.2V
- **GPU**: 0.9-1.1V
- **DDR**: 1.5V fixed
- **IO**: 1.8V/3.3V

## Debugging Interfaces

### UART

- **Port**: UART2 (typical)
- **Baud**: 1500000
- **Parity**: None
- **Flow Control**: None

Connect via TTL-USB adapter to debug serial console.

### JTAG

Optional JTAG interface for low-level debugging (if populated on device).

## Performance Benchmarks

### CPU

- **Dhrystone**: ~800 DMIPS
- **SpecINT**: ~1.2x baseline
- **Coremark**: ~8000

### GPU

- **GFXBench**: ~200 FPS (Manhattan)
- **GLBenchmark**: ~500 FPS (Simple)

### Memory

- **Read Bandwidth**: 4-5 GB/s (LPDDR3)
- **Write Bandwidth**: 2-3 GB/s
- **Latency**: 50-100 ns

## Known Quirks

1. **Thermal**: Device can throttle under sustained load
2. **WiFi**: May require firmware loading at boot
3. **Bluetooth**: Some devices use Bluetooth Low Energy (BLE) only
4. **USB**: May need special handling for fast charging detection
5. **Display**: Some panels require timing tuning in device tree

## Further Reading

- Rockchip RK3326 TRM (Technical Reference Manual)
- ARM Cortex-A35 Technical Reference Manual
- Mali-400 Graphics Processor Technical Reference Manual
