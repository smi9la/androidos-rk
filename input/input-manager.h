#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <time.h>
#include <stdarg.h>

typedef enum {
  INPUT_DEVICE_UNKNOWN,
  INPUT_DEVICE_GAMEPAD,
  INPUT_DEVICE_TOUCHSCREEN,
  INPUT_DEVICE_KEYBOARD
} InputDeviceType;

typedef enum {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_A,
  BUTTON_B,
  BUTTON_X,
  BUTTON_Y,
  BUTTON_L1,
  BUTTON_R1,
  BUTTON_L2,
  BUTTON_R2,
  BUTTON_START,
  BUTTON_SELECT,
  BUTTON_HOME,
  BUTTON_MENU
} ButtonCode;

typedef struct {
  ButtonCode code;
  int pressed;  // 1 if pressed, 0 if released
  time_t timestamp;
} InputEvent;

// Device management
int input_scan_devices(void);
int input_open_device(int index);
void input_close_device(int index);
int input_get_device_count(void);

// Lifecycle
int input_init(void);
void input_shutdown(void);
void input_log(const char *format, ...);

#endif  // INPUT_MANAGER_H
