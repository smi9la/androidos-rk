#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "hotkey-handler.h"

#define LOG_DIR "/var/log/androidos/input"

void hotkey_log(const char *format, ...) {
  char log_path[256];
  snprintf(log_path, sizeof(log_path), "%s/hotkey.log", LOG_DIR);
  
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

int hotkey_handle_combination(int button1, int button2) {
  // SELECT + START = Return to launcher
  if ((button1 == BUTTON_SELECT && button2 == BUTTON_START) ||
      (button1 == BUTTON_START && button2 == BUTTON_SELECT)) {
    hotkey_log("Hotkey: SELECT + START = Return to launcher");
    system("pkill -9 ppsspp retroarch");
    system("touch /tmp/return-to-launcher");
    return 1;
  }
  
  // SELECT + A = Save state
  if ((button1 == BUTTON_SELECT && button2 == BUTTON_A) ||
      (button1 == BUTTON_A && button2 == BUTTON_SELECT)) {
    hotkey_log("Hotkey: SELECT + A = Save state");
    system("touch /tmp/save-state");
    return 1;
  }
  
  // SELECT + B = Load state
  if ((button1 == BUTTON_SELECT && button2 == BUTTON_B) ||
      (button1 == BUTTON_B && button2 == BUTTON_SELECT)) {
    hotkey_log("Hotkey: SELECT + B = Load state");
    system("touch /tmp/load-state");
    return 1;
  }
  
  // SELECT + L1 = Screenshot
  if ((button1 == BUTTON_SELECT && button2 == BUTTON_L1) ||
      (button1 == BUTTON_L1 && button2 == BUTTON_SELECT)) {
    hotkey_log("Hotkey: SELECT + L1 = Screenshot");
    system("screenshot");
    return 1;
  }
  
  return 0;
}

int hotkey_init(void) {
  system("mkdir -p /var/log/androidos/input");
  hotkey_log("Hotkey handler initialized");
  return 0;
}

void hotkey_shutdown(void) {
  hotkey_log("Hotkey handler shutdown");
}
