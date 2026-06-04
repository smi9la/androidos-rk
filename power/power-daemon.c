#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "battery.h"
#include "thermal.h"
#include "cpufreq.h"

#define DAEMON_LOG "/var/log/androidos/power/daemon.log"
#define BATTERY_POLL_INTERVAL 5  // seconds (normal)
#define BATTERY_CRITICAL_POLL 2  // seconds (low battery)
#define THERMAL_POLL_INTERVAL 1  // second

static volatile int g_running = 1;

void signal_handler(int sig) {
  if (sig == SIGTERM || sig == SIGINT) {
    g_running = 0;
  }
}

void daemon_log(const char *format, ...) {
  FILE *f = fopen(DAEMON_LOG, "a");
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

void check_battery_status(void) {
  battery_update_state();
  int capacity = battery_get_capacity();
  
  if (capacity <= 5) {
    daemon_log("CRITICAL: Battery at %d%% - Emergency shutdown!", capacity);
    system("shutdown -h now");
  }
  else if (capacity <= 10) {
    daemon_log("WARNING: Battery critical at %d%%", capacity);
    // Trigger emergency UI notification
    system("touch /tmp/battery-critical");
  }
  else if (capacity <= 20) {
    daemon_log("NOTICE: Battery low at %d%% - enabling power saver", capacity);
    cpufreq_apply_profile(CPU_PROFILE_POWER_SAVER);
    system("touch /tmp/battery-low");
  }
}

void check_thermal_status(void) {
  thermal_update_state();
  int temp = thermal_get_current_temp();
  ThermalLevel level = (ThermalLevel)thermal_get_level_int();
  
  if (level >= THERMAL_HIGH) {
    daemon_log("WARNING: High temperature: %d°C - applying thermal throttle", temp);
  }
}

void manage_power_profiles(void) {
  // Determine best profile based on current conditions
  static int last_profile = -1;
  
  battery_update_state();
  int capacity = battery_get_capacity();
  
  int emulator_running = (access("/tmp/emulator-running", F_OK) == 0);
  
  CPUProfile desired_profile;
  
  if (emulator_running) {
    desired_profile = CPU_PROFILE_GAMING;
  }
  else if (capacity < 10) {
    desired_profile = CPU_PROFILE_POWER_SAVER;
  }
  else if (capacity < 20) {
    desired_profile = CPU_PROFILE_POWER_SAVER;
  }
  else {
    desired_profile = CPU_PROFILE_BALANCED;
  }
  
  if (desired_profile != last_profile) {
    daemon_log("Switching to profile: %s", cpufreq_get_profile_name(desired_profile));
    cpufreq_apply_profile(desired_profile);
    last_profile = desired_profile;
  }
}

int main(int argc, char *argv[]) {
  // Setup signal handlers
  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);
  
  // Create log directory
  system("mkdir -p /var/log/androidos/power");
  
  daemon_log("Power management daemon started (PID: %d)", getpid());
  
  // Initialize subsystems
  battery_init();
  thermal_init();
  cpufreq_init();
  
  daemon_log("All subsystems initialized");
  
  // Main loop
  time_t last_battery_check = 0;
  time_t last_thermal_check = 0;
  time_t last_profile_check = 0;
  
  while (g_running) {
    time_t now = time(NULL);
    
    // Battery check (every 5 seconds, or 2 seconds if low)
    battery_update_state();
    int capacity = battery_get_capacity();
    int poll_interval = (capacity < 20) ? BATTERY_CRITICAL_POLL : BATTERY_POLL_INTERVAL;
    
    if (now - last_battery_check >= poll_interval) {
      check_battery_status();
      last_battery_check = now;
    }
    
    // Thermal check (every 1 second)
    if (now - last_thermal_check >= THERMAL_POLL_INTERVAL) {
      check_thermal_status();
      last_thermal_check = now;
    }
    
    // Profile management (every 2 seconds)
    if (now - last_profile_check >= 2) {
      manage_power_profiles();
      last_profile_check = now;
    }
    
    sleep(1);
  }
  
  daemon_log("Shutting down...");
  
  // Cleanup
  battery_shutdown();
  thermal_shutdown();
  cpufreq_shutdown();
  
  daemon_log("Power management daemon stopped");
  
  return 0;
}
