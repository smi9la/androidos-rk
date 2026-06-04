#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "controller-map.h"

#define LOG_DIR "/var/log/androidos/input"
#define CONFIG_DIR "/etc/androidos/controller"

static ControllerProfile g_active_profile = {0};
static char g_profile_name[256] = "default";

void controller_log(const char *format, ...) {
  char log_path[256];
  snprintf(log_path, sizeof(log_path), "%s/controller.log", LOG_DIR);
  
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

int controller_load_profile(const char *game_id) {
  char profile_path[512];
  snprintf(profile_path, sizeof(profile_path), "%s/%s.profile", CONFIG_DIR, game_id);
  
  FILE *f = fopen(profile_path, "r");
  if (!f) {
    controller_log("No profile found for %s, using default", game_id);
    controller_load_default_profile();
    return 0;
  }
  
  memset(&g_active_profile, 0, sizeof(g_active_profile));
  strcpy(g_profile_name, game_id);
  
  char line[256];
  while (fgets(line, sizeof(line), f)) {
    // Parse configuration
    // Format: button=gpio_pin or axis=adc_channel
  }
  
  fclose(f);
  controller_log("Loaded profile: %s", game_id);
  return 0;
}

int controller_load_default_profile(void) {
  // Load standard R36S button mapping
  memset(&g_active_profile, 0, sizeof(g_active_profile));
  strcpy(g_profile_name, "default");
  
  // Standard R36S GPIO pins
  g_active_profile.button_up = 1;      // GPIO0_A2
  g_active_profile.button_down = 2;    // GPIO0_A3
  g_active_profile.button_left = 3;    // GPIO0_B0
  g_active_profile.button_right = 4;   // GPIO0_B1
  g_active_profile.button_a = 5;       // GPIO1_A2
  g_active_profile.button_b = 6;       // GPIO1_A3
  g_active_profile.button_x = 7;       // GPIO1_B0
  g_active_profile.button_y = 8;       // GPIO1_B1
  g_active_profile.button_l1 = 9;      // GPIO0_C2
  g_active_profile.button_r1 = 10;     // GPIO0_C3
  g_active_profile.button_start = 11;  // GPIO1_C0
  g_active_profile.button_select = 12; // GPIO1_C1
  
  controller_log("Loaded default profile");
  return 0;
}

char *controller_get_profile_name(void) {
  return g_profile_name;
}

int controller_map_button(ButtonCode button) {
  switch (button) {
    case BUTTON_UP: return g_active_profile.button_up;
    case BUTTON_DOWN: return g_active_profile.button_down;
    case BUTTON_LEFT: return g_active_profile.button_left;
    case BUTTON_RIGHT: return g_active_profile.button_right;
    case BUTTON_A: return g_active_profile.button_a;
    case BUTTON_B: return g_active_profile.button_b;
    case BUTTON_X: return g_active_profile.button_x;
    case BUTTON_Y: return g_active_profile.button_y;
    case BUTTON_L1: return g_active_profile.button_l1;
    case BUTTON_R1: return g_active_profile.button_r1;
    case BUTTON_START: return g_active_profile.button_start;
    case BUTTON_SELECT: return g_active_profile.button_select;
    default: return -1;
  }
}

int controller_init(void) {
  system("mkdir -p /var/log/androidos/input");
  system("mkdir -p /etc/androidos/controller");
  controller_log("Controller mapper initialized");
  controller_load_default_profile();
  return 0;
}

void controller_shutdown(void) {
  controller_log("Controller mapper shutdown");
}
