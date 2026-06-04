#ifndef CONTROLLER_MAP_H
#define CONTROLLER_MAP_H

#include "input-manager.h"

typedef struct {
  char game_id[256];
  int button_up;
  int button_down;
  int button_left;
  int button_right;
  int button_a;
  int button_b;
  int button_x;
  int button_y;
  int button_l1;
  int button_r1;
  int button_start;
  int button_select;
  int analog_l_x;
  int analog_l_y;
  int analog_r_x;
  int analog_r_y;
} ControllerProfile;

// Profile management
int controller_load_profile(const char *game_id);
int controller_load_default_profile(void);
char *controller_get_profile_name(void);
int controller_map_button(ButtonCode button);

// Lifecycle
int controller_init(void);
void controller_shutdown(void);
void controller_log(const char *format, ...);

#endif  // CONTROLLER_MAP_H
