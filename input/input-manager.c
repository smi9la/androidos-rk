#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "input-manager.h"

#define INPUT_DEVICES_PATH "/dev/input"
#define LOG_DIR "/var/log/androidos/input"
#define MAX_INPUT_DEVICES 16

typedef struct {
  int fd;
  char name[256];
  char path[256];
  InputDeviceType type;
} InputDevice;

static InputDevice g_devices[MAX_INPUT_DEVICES];
static int g_device_count = 0;

void input_log(const char *format, ...) {
  char log_path[256];
  snprintf(log_path, sizeof(log_path), "%s/input.log", LOG_DIR);
  
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

int input_detect_device_type(const char *name) {
  if (strstr(name, "Gamepad") || strstr(name, "Joystick")) {
    return INPUT_DEVICE_GAMEPAD;
  }
  if (strstr(name, "Touchscreen") || strstr(name, "Touch")) {
    return INPUT_DEVICE_TOUCHSCREEN;
  }
  if (strstr(name, "Keyboard")) {
    return INPUT_DEVICE_KEYBOARD;
  }
  return INPUT_DEVICE_UNKNOWN;
}

int input_scan_devices(void) {
  DIR *dir = opendir(INPUT_DEVICES_PATH);
  if (!dir) {
    input_log("ERROR: Cannot open %s", INPUT_DEVICES_PATH);
    return -1;
  }
  
  g_device_count = 0;
  struct dirent *entry;
  
  while ((entry = readdir(dir)) != NULL && g_device_count < MAX_INPUT_DEVICES) {
    if (strncmp(entry->d_name, "event", 5) == 0) {
      char path[256];
      snprintf(path, sizeof(path), "%s/%s", INPUT_DEVICES_PATH, entry->d_name);
      
      InputDevice *dev = &g_devices[g_device_count];
      strcpy(dev->path, path);
      
      // Try to get device name
      char name_path[256];
      snprintf(name_path, sizeof(name_path), "/sys/class/input/%s/device/name", entry->d_name);
      int fd = open(name_path, O_RDONLY);
      if (fd >= 0) {
        ssize_t n = read(fd, dev->name, sizeof(dev->name) - 1);
        if (n > 0) {
          dev->name[n] = '\0';
          if (dev->name[n-1] == '\n') dev->name[n-1] = '\0';
        }
        close(fd);
      } else {
        strcpy(dev->name, entry->d_name);
      }
      
      dev->type = input_detect_device_type(dev->name);
      dev->fd = -1;
      
      input_log("Detected device: %s (%s)", dev->path, dev->name);
      g_device_count++;
    }
  }
  
  closedir(dir);
  input_log("Found %d input devices", g_device_count);
  
  return g_device_count;
}

int input_open_device(int index) {
  if (index < 0 || index >= g_device_count) {
    input_log("ERROR: Invalid device index %d", index);
    return -1;
  }
  
  InputDevice *dev = &g_devices[index];
  
  dev->fd = open(dev->path, O_RDONLY | O_NONBLOCK);
  if (dev->fd < 0) {
    input_log("ERROR: Cannot open %s", dev->path);
    return -1;
  }
  
  input_log("Opened device %d: %s", index, dev->path);
  return dev->fd;
}

void input_close_device(int index) {
  if (index < 0 || index >= g_device_count) return;
  
  InputDevice *dev = &g_devices[index];
  if (dev->fd >= 0) {
    close(dev->fd);
    dev->fd = -1;
    input_log("Closed device %d", index);
  }
}

int input_get_device_count(void) {
  return g_device_count;
}

InputDevice *input_get_device(int index) {
  if (index < 0 || index >= g_device_count) return NULL;
  return &g_devices[index];
}

int input_init(void) {
  system("mkdir -p /var/log/androidos/input");
  input_log("Input manager initialized");
  
  if (input_scan_devices() < 0) {
    input_log("WARNING: No input devices found");
    return 0;  // Not fatal
  }
  
  return 0;
}

void input_shutdown(void) {
  for (int i = 0; i < g_device_count; i++) {
    input_close_device(i);
  }
  input_log("Input manager shutdown");
}
