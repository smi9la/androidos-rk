#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "battery.h"

#define POWER_SUPPLY_PATH "/sys/class/power_supply"
#define LOG_DIR "/var/log/androidos/power"
#define BATTERY_POLL_INTERVAL 5  // seconds (normal), 2 (low battery)
#define LOW_BATTERY_THRESHOLD 20
#define CRITICAL_BATTERY_THRESHOLD 10

typedef struct {
  int capacity;
  int voltage;
  int current;
  int temp;
  char status[32];
  char health[32];
  time_t timestamp;
} BatteryState;

static BatteryState g_battery_state = {0};
static FILE *g_log_file = NULL;

void battery_log(const char *format, ...) {
  char log_path[256];
  snprintf(log_path, sizeof(log_path), "%s/battery.log", LOG_DIR);
  
  g_log_file = fopen(log_path, "a");
  if (!g_log_file) return;
  
  va_list args;
  va_start(args, format);
  
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char timestamp[32];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
  
  fprintf(g_log_file, "[%s] ", timestamp);
  vfprintf(g_log_file, format, args);
  fprintf(g_log_file, "\n");
  
  va_end(args);
  fclose(g_log_file);
}

int battery_read_sysfs(const char *node, char *buffer, size_t len) {
  char path[512];
  snprintf(path, sizeof(path), "%s/battery/%s", POWER_SUPPLY_PATH, node);
  
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    battery_log("ERROR: Cannot read %s", node);
    return -1;
  }
  
  ssize_t n = read(fd, buffer, len - 1);
  close(fd);
  
  if (n > 0) {
    buffer[n] = '\0';
    // Remove trailing newline
    if (buffer[n-1] == '\n') buffer[n-1] = '\0';
    return 0;
  }
  
  return -1;
}

int battery_get_capacity(void) {
  char buffer[32];
  if (battery_read_sysfs("capacity", buffer, sizeof(buffer)) == 0) {
    return atoi(buffer);
  }
  return -1;
}

int battery_get_voltage(void) {
  char buffer[32];
  if (battery_read_sysfs("voltage_now", buffer, sizeof(buffer)) == 0) {
    return atoi(buffer) / 1000;  // Convert to mV
  }
  return -1;
}

int battery_get_current(void) {
  char buffer[32];
  if (battery_read_sysfs("current_now", buffer, sizeof(buffer)) == 0) {
    return atoi(buffer) / 1000;  // Convert to mA
  }
  return -1;
}

int battery_get_temp(void) {
  char buffer[32];
  if (battery_read_sysfs("temp", buffer, sizeof(buffer)) == 0) {
    return atoi(buffer);
  }
  return -1;
}

char *battery_get_status(void) {
  static char buffer[32];
  if (battery_read_sysfs("status", buffer, sizeof(buffer)) == 0) {
    return buffer;
  }
  return "Unknown";
}

void battery_update_state(void) {
  g_battery_state.capacity = battery_get_capacity();
  g_battery_state.voltage = battery_get_voltage();
  g_battery_state.current = battery_get_current();
  g_battery_state.temp = battery_get_temp();
  strcpy(g_battery_state.status, battery_get_status());
  g_battery_state.timestamp = time(NULL);
}

BatteryLevel battery_get_level(void) {
  int capacity = g_battery_state.capacity;
  
  if (capacity >= 80) return BATTERY_HIGH;
  if (capacity >= 50) return BATTERY_NORMAL;
  if (capacity >= 20) return BATTERY_WARNING;
  if (capacity >= 10) return BATTERY_LOW;
  return BATTERY_CRITICAL;
}

BatteryColor battery_get_color(void) {
  int capacity = g_battery_state.capacity;
  
  if (capacity >= 20) return COLOR_GREEN;
  if (capacity >= 10) return COLOR_ORANGE;
  return COLOR_RED;
}

char *battery_get_color_name(BatteryColor color) {
  switch (color) {
    case COLOR_GREEN: return "GREEN";
    case COLOR_ORANGE: return "ORANGE";
    case COLOR_RED: return "RED";
    default: return "UNKNOWN";
  }
}

void battery_print_status(void) {
  printf("Battery Status:\n");
  printf("  Capacity: %d%%\n", g_battery_state.capacity);
  printf("  Voltage: %d mV\n", g_battery_state.voltage);
  printf("  Current: %d mA\n", g_battery_state.current);
  printf("  Temp: %d°C\n", g_battery_state.temp);
  printf("  Status: %s\n", g_battery_state.status);
  printf("  Level: %s\n", battery_get_level_name(battery_get_level()));
  printf("  Color: %s\n", battery_get_color_name(battery_get_color()));
}

char *battery_get_level_name(BatteryLevel level) {
  switch (level) {
    case BATTERY_HIGH: return "HIGH";
    case BATTERY_NORMAL: return "NORMAL";
    case BATTERY_WARNING: return "WARNING";
    case BATTERY_LOW: return "LOW";
    case BATTERY_CRITICAL: return "CRITICAL";
    default: return "UNKNOWN";
  }
}

int battery_is_charging(void) {
  return strcmp(g_battery_state.status, "Charging") == 0;
}

int battery_init(void) {
  // Create log directory
  system("mkdir -p /var/log/androidos/power");
  
  battery_log("Battery service started");
  battery_update_state();
  battery_print_status();
  
  return 0;
}

void battery_shutdown(void) {
  battery_log("Battery service stopped");
}
