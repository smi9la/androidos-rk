#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "thermal.h"

#define THERMAL_ZONE_PATH "/sys/class/thermal/thermal_zone0"
#define LOG_DIR "/var/log/androidos/power"
#define THERMAL_POLL_INTERVAL 1  // second

#define TEMP_CRITICAL 95000   // 95°C in millidegrees
#define TEMP_THROTTLE_HIGH 85000  // 85°C
#define TEMP_THROTTLE_MID 75000   // 75°C
#define TEMP_THROTTLE_LOW 65000   // 65°C
#define TEMP_MONITOR 50000    // 50°C

typedef struct {
  int current_temp;
  ThermalLevel level;
  time_t last_update;
  int throttle_active;
  int shutdown_triggered;
} ThermalState;

static ThermalState g_thermal_state = {0};

void thermal_log(const char *format, ...) {
  char log_path[256];
  snprintf(log_path, sizeof(log_path), "%s/thermal.log", LOG_DIR);
  
  FILE *f = fopen(log_path, "a");
  if (!f) return;
  
  va_list args;
  va_start(args, format);
  
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char timestamp[32];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
  
  fprintf(f, "[%s] ", timestamp);
  vfprintf(f, format, args);
  fprintf(f, "\n");
  
  va_end(args);
  fclose(f);
}

int thermal_read_temp(void) {
  char path[256];
  snprintf(path, sizeof(path), "%s/temp", THERMAL_ZONE_PATH);
  
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    thermal_log("ERROR: Cannot read thermal zone");
    return -1;
  }
  
  char buffer[32];
  ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
  close(fd);
  
  if (n > 0) {
    buffer[n] = '\0';
    return atoi(buffer);
  }
  
  return -1;
}

ThermalLevel thermal_get_level(int temp_millidegrees) {
  if (temp_millidegrees >= TEMP_CRITICAL) return THERMAL_CRITICAL;
  if (temp_millidegrees >= TEMP_THROTTLE_HIGH) return THERMAL_HIGH;
  if (temp_millidegrees >= TEMP_THROTTLE_MID) return THERMAL_MID;
  if (temp_millidegrees >= TEMP_THROTTLE_LOW) return THERMAL_LOW;
  if (temp_millidegrees >= TEMP_MONITOR) return THERMAL_MONITOR;
  return THERMAL_NORMAL;
}

char *thermal_get_level_name(ThermalLevel level) {
  switch (level) {
    case THERMAL_NORMAL: return "NORMAL";
    case THERMAL_MONITOR: return "MONITOR";
    case THERMAL_LOW: return "LOW";
    case THERMAL_MID: return "MID";
    case THERMAL_HIGH: return "HIGH";
    case THERMAL_CRITICAL: return "CRITICAL";
    default: return "UNKNOWN";
  }
}

void thermal_apply_throttle(ThermalLevel level) {
  switch (level) {
    case THERMAL_CRITICAL:
      thermal_log("EMERGENCY: Critical temperature! Forcing shutdown.");
      system("shutdown -h now");
      g_thermal_state.shutdown_triggered = 1;
      break;
      
    case THERMAL_HIGH:
      if (!g_thermal_state.throttle_active || g_thermal_state.level != THERMAL_HIGH) {
        thermal_log("THROTTLE: High temp (%.1f°C) - scaling CPU to 600 MHz",
                   g_thermal_state.current_temp / 1000.0);
        system("echo 600000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
        g_thermal_state.throttle_active = 1;
      }
      break;
      
    case THERMAL_MID:
      if (!g_thermal_state.throttle_active || g_thermal_state.level != THERMAL_MID) {
        thermal_log("THROTTLE: Mid temp (%.1f°C) - scaling CPU to 800 MHz",
                   g_thermal_state.current_temp / 1000.0);
        system("echo 800000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
        g_thermal_state.throttle_active = 1;
      }
      break;
      
    case THERMAL_LOW:
      if (!g_thermal_state.throttle_active || g_thermal_state.level != THERMAL_LOW) {
        thermal_log("THROTTLE: Low temp (%.1f°C) - scaling CPU to 1200 MHz",
                   g_thermal_state.current_temp / 1000.0);
        system("echo 1200000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
        g_thermal_state.throttle_active = 1;
      }
      break;
      
    case THERMAL_MONITOR:
    case THERMAL_NORMAL:
      if (g_thermal_state.throttle_active) {
        thermal_log("RECOVERY: Temperature normal (%.1f°C) - restoring CPU to 1800 MHz",
                   g_thermal_state.current_temp / 1000.0);
        system("echo 1800000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
        g_thermal_state.throttle_active = 0;
      }
      break;
  }
}

void thermal_update_state(void) {
  int temp = thermal_read_temp();
  if (temp < 0) return;
  
  g_thermal_state.current_temp = temp;
  g_thermal_state.level = thermal_get_level(temp);
  g_thermal_state.last_update = time(NULL);
  
  thermal_apply_throttle(g_thermal_state.level);
}

int thermal_get_current_temp(void) {
  return g_thermal_state.current_temp / 1000;  // Return in °C
}

int thermal_get_level_int(void) {
  return (int)g_thermal_state.level;
}

void thermal_print_status(void) {
  printf("Thermal Status:\n");
  printf("  Current Temp: %.1f°C\n", g_thermal_state.current_temp / 1000.0);
  printf("  Level: %s\n", thermal_get_level_name(g_thermal_state.level));
  printf("  Throttle Active: %s\n", g_thermal_state.throttle_active ? "YES" : "NO");
  printf("  Last Update: %ld\n", g_thermal_state.last_update);
}

int thermal_init(void) {
  system("mkdir -p /var/log/androidos/power");
  thermal_log("Thermal monitoring service started");
  thermal_update_state();
  thermal_print_status();
  return 0;
}

void thermal_shutdown(void) {
  thermal_log("Thermal monitoring service stopped");
}
