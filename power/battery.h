#ifndef BATTERY_H
#define BATTERY_H

#include <time.h>
#include <stdarg.h>

typedef enum {
  BATTERY_HIGH,
  BATTERY_NORMAL,
  BATTERY_WARNING,
  BATTERY_LOW,
  BATTERY_CRITICAL
} BatteryLevel;

typedef enum {
  COLOR_GREEN,
  COLOR_ORANGE,
  COLOR_RED
} BatteryColor;

// Battery reading functions
int battery_get_capacity(void);      // 0-100%
int battery_get_voltage(void);       // mV
int battery_get_current(void);       // mA
int battery_get_temp(void);          // °C
char *battery_get_status(void);      // Charging, Discharging, etc.

// State functions
BatteryLevel battery_get_level(void);
BatteryColor battery_get_color(void);
int battery_is_charging(void);

// Utility functions
void battery_update_state(void);
void battery_print_status(void);
char *battery_get_level_name(BatteryLevel level);
char *battery_get_color_name(BatteryColor color);
void battery_log(const char *format, ...);

// Lifecycle
int battery_init(void);
void battery_shutdown(void);

#endif  // BATTERY_H
