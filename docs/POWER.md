# Power Management Specification

## Overview

ANDROIDOS-RK power management ensures:

1. **Battery Health**: Accurate monitoring and low-battery detection
2. **Thermal Safety**: Prevents overheating through throttling
3. **Energy Efficiency**: Extends battery life through intelligent scaling
4. **Reliability**: Graceful degradation under thermal/power stress

## Battery Monitoring

### Data Sources

All battery data sourced from `/sys/class/power_supply/`:

```
/sys/class/power_supply/
├── battery/
│   ├── capacity          (0-100, percentage)
│   ├── capacity_level    (Critical, Low, Normal, High, Full)
│   ├── voltage_now       (microvolts)
│   ├── current_now       (microamps, signed)
│   ├── temp              (°C)
│   ├── status            (Discharging, Charging, Not charging, Full)
│   ├── health            (Good, Overheat, Dead, Over voltage, etc.)
│   ├── charge_counter    (microampere-hours)
│   └── time_to_empty     (seconds, if available)
├── usb/
│   ├── present
│   └── type              (USB Power Delivery, etc.)
└── ac/
    └── present
```

### Battery Capacity Thresholds

| Level | Range | UI Color | Action |
|-------|-------|----------|--------|
| Critical | 0-5% | Red flashing | Shutdown warning |
| Low | 5-10% | Red | Brightness reduction |
| Warning | 10-20% | Orange | Normal operation |
| Normal | 20-80% | Green | Normal operation |
| High | 80-100% | Green | Charging (if plugged) |

### Battery Update Frequency

- **Normal Operation**: Every 5 seconds
- **Low Battery (<20%)**: Every 2 seconds
- **Charging**: Every 10 seconds

## Thermal Management

### Thermal Zone Monitoring

```
/sys/class/thermal/
├── thermal_zone0/     (CPU)
│   ├── temp           (millidegrees Celsius)
│   ├── trip_point_0_type
│   ├── trip_point_0_temp
│   └── trip_point_N_type
├── thermal_zone1/     (GPU, if available)
└── cooling_device0/   (CPU frequency scaling)
```

### Thermal Trip Points

| Temperature | Action | Recovery |
|-------------|--------|----------|
| >50°C | Monitor closely | Normal |
| >65°C | Reduce CPU frequency to 1.2 GHz | At 60°C |
| >75°C | Reduce CPU frequency to 800 MHz | At 70°C |
| >85°C | Reduce CPU frequency to 600 MHz | At 80°C |
| >95°C | Force shutdown (emergency) | Reboot required |

### Thermal Control Flow

```
1. Read /sys/class/thermal/thermal_zone0/temp
2. Compare against thresholds
3. Apply frequency scaling if needed
4. Log thermal event
5. Update UI (optional thermal indicator)
6. Repeat every 1 second
```

## CPU Frequency Scaling (DVFS)

### Available Frequencies

R36S typical frequencies:

```
/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies
600000 800000 1200000 1416000 1512000 1608000 1704000 1800000
```

(Units: kHz)

### Scaling Governors

**Default: `schedutil`** (kernel scheduler-based)

Alternatives:
- `powersave` — Always minimum frequency (saves power, slows performance)
- `performance` — Always maximum frequency (uses power, maximizes performance)
- `ondemand` — Reactive scaling based on CPU load
- `conservative` — Gradual scaling similar to ondemand

### Scaling Policy

```bash
# Get current policy
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

# Set to schedutil
echo schedutil > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

# Get/set min frequency
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq
echo 600000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq

# Get/set max frequency
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
echo 1800000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
```

### Scaling Profiles

**ANDROIDOS-RK uses 3 profiles:**

1. **Gaming** (during emulator execution)
   - Min: 1.2 GHz
   - Max: 1.8 GHz
   - Governor: schedutil
   - Turbo: Enabled

2. **Balanced** (normal UI usage)
   - Min: 600 MHz
   - Max: 1.5 GHz
   - Governor: schedutil
   - Turbo: Adaptive

3. **Power Saver** (battery critical)
   - Min: 600 MHz
   - Max: 1.2 GHz
   - Governor: powersave
   - Turbo: Disabled

### Profile Switching Logic

```
if (emulator_running) {
  apply_profile(GAMING)
} else if (battery_percent < 10) {
  apply_profile(POWER_SAVER)
} else if (battery_percent < 20) {
  apply_profile(POWER_SAVER)  // Force power saving
} else {
  apply_profile(BALANCED)
}
```

## Power States

### System Sleep States

- **S0**: Active (playing games, UI navigation)
- **S1**: Idle (UI visible, no user input for 2+ minutes)
- **S3**: Suspend (button press required to wake)
- **S4**: Hibernation (not implemented for simplicity)
- **S5**: Shutdown

### Sleep Timeout Configuration

```
UI Idle: 2 minutes → Dim display
Display Idle: 4 minutes → Turn off display
Deep Idle: 6 minutes → Enter S3 suspend
```

## Power Efficiency Techniques

### 1. Clock Gating

Unused peripherals disabled:
- WiFi off when not needed
- Bluetooth off when not needed
- USB off when not charging

### 2. Power Domains

Managed via PMU:

```
/sys/module/rockchip_pm_domains/parameters/
```

### 3. Backlight Control

```
/sys/class/backlight/*/brightness

Brightness levels: 0 (off) to 255 (max)

Policy:
- Gaming: 200 (80% brightness for visibility)
- Normal: 150 (60% default)
- Low battery: 100 (40%)
- Dim: 50 (20%)
```

### 4. Dynamic Voltage Scaling (DVS)

CPU voltage adjusted with frequency:

```
600 MHz → 0.9V
1.0 GHz → 1.0V
1.5 GHz → 1.1V
1.8 GHz → 1.2V
```

## Low Battery Handling

### Stages

**Stage 1: Warning (20% → 15%)**
- Show low battery notification
- Suggest reducing brightness
- Continue normal operation

**Stage 2: Critical (15% → 10%)**
- Enable power saver mode automatically
- Reduce brightness to 40%
- Limit CPU to 1.2 GHz
- Show persistent low battery indicator

**Stage 3: Emergency (10% → 5%)**
- Show emergency shutdown warning
- Save all game state
- Prepare for immediate shutdown

**Stage 4: Fatal (<5%)**
- Force shutdown immediately
- Preserve data integrity

### Low Battery Code Example

```c
void check_battery() {
  int capacity = read_battery_capacity();
  
  if (capacity <= 5) {
    save_all_state();
    system("shutdown -h now");
  }
  else if (capacity <= 10) {
    show_emergency_warning();
    force_power_saver_mode();
  }
  else if (capacity <= 20) {
    enable_power_saver_mode();
    show_low_battery_warning();
  }
}
```

## Charging Behavior

### Detection

Charging detected via:

```bash
cat /sys/class/power_supply/usb/present
# or
cat /sys/class/power_supply/ac/present
# or check battery status
cat /sys/class/power_supply/battery/status  # "Charging"
```

### Charging Profiles

| Battery Level | CPU Max | GPU Max | Charging Current |
|----------------|---------|---------|------------------|
| 0-20% | 1.5 GHz | 400 MHz | 2.0A (fast charge) |
| 20-80% | 1.5 GHz | 400 MHz | 2.0A (normal) |
| 80-95% | 1.2 GHz | 300 MHz | 1.0A (trickle) |
| 95-100% | 600 MHz | 200 MHz | 0.5A (maintenance) |

### Full Charge Behavior

When battery reaches 100%:

```
1. Switch to 0.5A maintenance charging
2. Limit to 1.2 GHz CPU (to reduce heat)
3. Stop at 100% (prevent overcharging)
4. Display "Fully charged" notification
```

## Logging

All power events logged to `/var/log/power/`:

```
/var/log/power/
├── battery.log       (capacity, voltage, current, temp changes)
├── thermal.log       (temperature changes, throttling events)
├── charging.log      (charging start/stop, events)
├── frequency.log     (frequency scaling events)
└── power-events.log  (summary of all power events)
```

### Log Entry Format

```
[2026-06-04 14:32:15] BATTERY: capacity=45%, voltage=3.8V, temp=35°C
[2026-06-04 14:32:20] THERMAL: temp=72°C, scaled_freq=1200MHz (high temp throttle)
[2026-06-04 14:33:00] CHARGING: started (2.0A detected)
```

## Performance Impact

Estimated impact of power management:

- **Battery Life**: +30-50% vs. no power management
- **Gaming Performance**: <5% impact (governor adapts)
- **Thermal**: Prevents throttling in most scenarios
- **Responsiveness**: Imperceptible (<100ms overhead)

## Testing

### Battery Simulation

```bash
# Simulate low battery
echo 15 > /sys/class/power_supply/battery/capacity

# Simulate charging
echo Charging > /sys/class/power_supply/battery/status
```

### Thermal Testing

```bash
# Run stress test
stress-ng --cpu 4 --timeout 600s

# Monitor temperature
while true; do
  echo "$(date): $(cat /sys/class/thermal/thermal_zone0/temp | awk '{print $1/1000}'°C)"
  sleep 1
done
```

### Frequency Scaling Verification

```bash
# Monitor frequency changes
watch -n 0.5 'cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq | awk "{sum+=$1} END {print \"Avg: \" sum/NR/1000000 \" GHz\"}'
```
