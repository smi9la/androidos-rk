#ifndef ROM_LAUNCHER_H
#define ROM_LAUNCHER_H

#include <stdarg.h>

typedef enum {
  EMULATOR_UNKNOWN,
  EMULATOR_PPSSPP,
  EMULATOR_RETROARCH
} EmulatorType;

// Launching
int rom_launch(const char *rom_path, const char *system);
EmulatorType rom_detect_emulator(const char *system);

// Lifecycle
int launcher_init(void);
void launcher_shutdown(void);
void launcher_log(const char *format, ...);

#endif  // ROM_LAUNCHER_H
