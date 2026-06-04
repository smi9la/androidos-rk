#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include "rom-launcher.h"

#define LOG_DIR "/var/log/androidos/roms"

void launcher_log(const char *format, ...) {
  char log_path[256];
  snprintf(log_path, sizeof(log_path), "%s/launcher.log", LOG_DIR);
  
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

EmulatorType rom_detect_emulator(const char *system) {
  if (strcmp(system, "psp") == 0) return EMULATOR_PPSSPP;
  if (strcmp(system, "ps1") == 0) return EMULATOR_RETROARCH;
  if (strcmp(system, "gba") == 0) return EMULATOR_RETROARCH;
  if (strcmp(system, "snes") == 0) return EMULATOR_RETROARCH;
  if (strcmp(system, "nes") == 0) return EMULATOR_RETROARCH;
  if (strcmp(system, "md") == 0) return EMULATOR_RETROARCH;
  return EMULATOR_UNKNOWN;
}

int rom_launch(const char *rom_path, const char *system) {
  launcher_log("Attempting to launch ROM: %s (system: %s)", rom_path, system);
  
  // Verify file exists
  if (access(rom_path, F_OK) != 0) {
    launcher_log("ERROR: ROM file not found: %s", rom_path);
    return -1;
  }
  
  EmulatorType emulator = rom_detect_emulator(system);
  if (emulator == EMULATOR_UNKNOWN) {
    launcher_log("ERROR: Unknown system: %s", system);
    return -1;
  }
  
  // Set emulator running flag
  system("touch /tmp/emulator-running");
  
  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    if (emulator == EMULATOR_PPSSPP) {
      launcher_log("Launching PPSSPP with ROM: %s", rom_path);
      execlp("ppsspp", "ppsspp", rom_path, NULL);
    } else if (emulator == EMULATOR_RETROARCH) {
      launcher_log("Launching RetroArch with ROM: %s", rom_path);
      execlp("retroarch", "retroarch", "-L", "auto", rom_path, NULL);
    }
    launcher_log("ERROR: Failed to execute emulator");
    exit(1);
  } else if (pid > 0) {
    // Parent process - wait for emulator
    launcher_log("Emulator started with PID %d", pid);
    int status;
    waitpid(pid, &status, 0);
    launcher_log("Emulator exited with status %d", status);
    
    // Clear emulator running flag
    system("rm -f /tmp/emulator-running");
    
    return 0;
  } else {
    launcher_log("ERROR: Failed to fork");
    system("rm -f /tmp/emulator-running");
    return -1;
  }
}

int launcher_init(void) {
  system("mkdir -p /var/log/androidos/roms");
  launcher_log("ROM launcher initialized");
  return 0;
}

void launcher_shutdown(void) {
  launcher_log("ROM launcher shutdown");
}
