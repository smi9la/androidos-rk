#ifndef THERMAL_H
#define THERMAL_H

#include <time.h>
#include <stdarg.h>

typedef enum {
  THERMAL_NORMAL,
  THERMAL_MONITOR,
  THERMAL_LOW,
  THERMAL_MID,
  THERMAL_HIGH,
  THERMAL_CRITICAL
} ThermalLevel;

// Reading functions
int thermal_read_temp(void);  // Returns millidegrees Celsius
int thermal_get_current_temp(void);  // Returns degrees Celsius

// State functions
ThermalLevel thermal_get_level(int temp_millidegrees);
int thermal_get_level_int(void);

// Utility functions
void thermal_update_state(void);
void thermal_apply_throttle(ThermalLevel level);
void thermal_print_status(void);
char *thermal_get_level_name(ThermalLevel level);
void thermal_log(const char *format, ...);

// Lifecycle
int thermal_init(void);
void thermal_shutdown(void);

#endif  // THERMAL_H
