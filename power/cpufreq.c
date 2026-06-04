#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cpufreq.h"

#define CPUFREQ_PATH "/sys/devices/system/cpu/cpu0/cpufreq"
#define LOG_DIR "/var/log/androidos/power"

typedef struct {
  int current_freq;      // kHz
  int min_freq;          // kHz
  int max_freq;          // kHz
  char governor[64];
  CPUProfile current_profile;
  time_t last_profile_change;
} CPUFreqState;

static CPUFreqState g_cpufreq_state = {0};

const CPUProfileConfig g_profiles[CPU_PROFILE_COUNT] = {
  // GAMING
  {
    .name = "GAMING",
    .min_freq = 1200000,  // 1.2 GHz
    .max_freq = 1800000,  // 1.8 GHz
    .governor = "schedutil",
  },
  // BALANCED
  {
    .name = "BALANCED",
    .min_freq = 600000,   // 600 MHz
    .max_freq = 1500000,  // 1.5 GHz
    .governor = "schedutil",
  },
  // POWER_SAVER
  {
    .name = "POWER_SAVER",
    .min_freq = 600000,   // 600 MHz
    .max_freq = 1200000,  // 1.2 GHz
    .governor = "powersave",
  }
};

void cpufreq_log(const char *format, ...) {
  char log_path[256];
  snprintf(log_path, sizeof(log_path), "%s/frequency.log", LOG_DIR);
  
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

int cpufreq_read_file(const char *filename, char *buffer, size_t len) {
  char path[256];
  snprintf(path, sizeof(path), "%s/%s", CPUFREQ_PATH, filename);
  
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    return -1;
  }
  
  ssize_t n = read(fd, buffer, len - 1);
  close(fd);
  
  if (n > 0) {
    buffer[n] = '\0';
    if (buffer[n-1] == '\n') buffer[n-1] = '\0';
    return 0;
  }
  
  return -1;
}

int cpufreq_write_file(const char *filename, const char *value) {
  char path[256];
  snprintf(path, sizeof(path), "%s/%s", CPUFREQ_PATH, filename);
  
  int fd = open(path, O_WRONLY);
  if (fd < 0) {
    cpufreq_log("ERROR: Cannot write to %s", filename);
    return -1;
  }
  
  ssize_t n = write(fd, value, strlen(value));
  close(fd);
  
  return (n > 0) ? 0 : -1;
}

int cpufreq_get_current_freq(void) {
  char buffer[32];
  if (cpufreq_read_file("scaling_cur_freq", buffer, sizeof(buffer)) == 0) {
    return atoi(buffer);
  }
  return -1;
}

int cpufreq_get_min_freq(void) {
  char buffer[32];
  if (cpufreq_read_file("scaling_min_freq", buffer, sizeof(buffer)) == 0) {
    return atoi(buffer);
  }
  return -1;
}

int cpufreq_get_max_freq(void) {
  char buffer[32];
  if (cpufreq_read_file("scaling_max_freq", buffer, sizeof(buffer)) == 0) {
    return atoi(buffer);
  }
  return -1;
}

char *cpufreq_get_governor(void) {
  static char buffer[64];
  if (cpufreq_read_file("scaling_governor", buffer, sizeof(buffer)) == 0) {
    return buffer;
  }
  return "unknown";
}

int cpufreq_set_min_freq(int freq_khz) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%d", freq_khz);
  return cpufreq_write_file("scaling_min_freq", buffer);
}

int cpufreq_set_max_freq(int freq_khz) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%d", freq_khz);
  return cpufreq_write_file("scaling_max_freq", buffer);
}

int cpufreq_set_governor(const char *governor) {
  return cpufreq_write_file("scaling_governor", governor);
}

int cpufreq_apply_profile(CPUProfile profile) {
  if (profile >= CPU_PROFILE_COUNT) {
    cpufreq_log("ERROR: Invalid profile %d", profile);
    return -1;
  }
  
  const CPUProfileConfig *cfg = &g_profiles[profile];
  
  cpufreq_log("Applying profile: %s (min=%d kHz, max=%d kHz, gov=%s)",
             cfg->name, cfg->min_freq, cfg->max_freq, cfg->governor);
  
  // Set governor first
  if (cpufreq_set_governor(cfg->governor) != 0) {
    cpufreq_log("WARNING: Failed to set governor to %s", cfg->governor);
  }
  
  // Set frequencies
  if (cpufreq_set_min_freq(cfg->min_freq) != 0) {
    cpufreq_log("WARNING: Failed to set min freq to %d", cfg->min_freq);
  }
  
  if (cpufreq_set_max_freq(cfg->max_freq) != 0) {
    cpufreq_log("WARNING: Failed to set max freq to %d", cfg->max_freq);
  }
  
  g_cpufreq_state.current_profile = profile;
  g_cpufreq_state.last_profile_change = time(NULL);
  
  return 0;
}

void cpufreq_update_state(void) {
  g_cpufreq_state.current_freq = cpufreq_get_current_freq();
  g_cpufreq_state.min_freq = cpufreq_get_min_freq();
  g_cpufreq_state.max_freq = cpufreq_get_max_freq();
  strcpy(g_cpufreq_state.governor, cpufreq_get_governor());
}

void cpufreq_print_status(void) {
  printf("CPU Frequency Status:\n");
  printf("  Current: %.2f GHz\n", g_cpufreq_state.current_freq / 1000000.0);
  printf("  Min: %.2f GHz\n", g_cpufreq_state.min_freq / 1000000.0);
  printf("  Max: %.2f GHz\n", g_cpufreq_state.max_freq / 1000000.0);
  printf("  Governor: %s\n", g_cpufreq_state.governor);
  printf("  Profile: %s\n", g_profiles[g_cpufreq_state.current_profile].name);
}

char *cpufreq_get_profile_name(CPUProfile profile) {
  if (profile >= CPU_PROFILE_COUNT) return "UNKNOWN";
  return (char *)g_profiles[profile].name;
}

int cpufreq_init(void) {
  system("mkdir -p /var/log/androidos/power");
  cpufreq_log("CPU frequency scaling service started");
  cpufreq_update_state();
  cpufreq_apply_profile(CPU_PROFILE_BALANCED);
  cpufreq_print_status();
  return 0;
}

void cpufreq_shutdown(void) {
  cpufreq_log("CPU frequency scaling service stopped");
}
