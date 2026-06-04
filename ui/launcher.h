#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <stdarg.h>

typedef enum {
  SCREEN_HOME,
  SCREEN_GAME_LIBRARY,
  SCREEN_EMULATORS,
  SCREEN_SETTINGS,
  SCREEN_FILE_MANAGER
} ScreenType;

// UI management
int ui_init(void);
void ui_shutdown(void);
int ui_run(void);
void ui_handle_input(void);
void ui_update(void);

// Rendering
void ui_render_status_bar(void);
void ui_render_home_screen(void);
void ui_render_game_library(void);

void ui_log(const char *format, ...);

#endif  // LAUNCHER_H
