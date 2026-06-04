#ifndef CPUFREQ_H
#define CPUFREQ_H

#include <time.h>
#include <stdarg.h>

typedef enum {
  CPU_PROFILE_GAMING,
  CPU_PROFILE_BALANCED,
  CPU_PROFILE_POWER_SAVER,
  CPU_PROFILE_COUNT
} CPUProfile;

typedef struct {
  const char *name;
  int min_freq;        // kHz
  int max_freq;        // kHz
  const char *governor;
} CPUProfileConfig;

// Reading functions
int cpufreq_get_current_freq(void);  // kHz
int cpufreq_get_min_freq(void);      // kHz
int cpufreq_get_max_freq(void);      // kHz
char *cpufreq_get_governor(void);

// Control functions
int cpufreq_set_min_freq(int freq_khz);
int cpufreq_set_max_freq(int freq_khz);
int cpufreq_set_governor(const char *governor);

// Profile functions
int cpufreq_apply_profile(CPUProfile profile);
char *cpufreq_get_profile_name(CPUProfile profile);

// State functions
void cpufreq_update_state(void);
void cpufreq_print_status(void);
void cpufreq_log(const char *format, ...);

// Lifecycle
int cpufreq_init(void);
void cpufreq_shutdown(void);

#endif  // CPUFREQ_H
