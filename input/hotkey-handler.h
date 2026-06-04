#ifndef HOTKEY_HANDLER_H
#define HOTKEY_HANDLER_H

#include "input-manager.h"
#include <stdarg.h>

// Hotkey combinations
int hotkey_handle_combination(int button1, int button2);

// Lifecycle
int hotkey_init(void);
void hotkey_shutdown(void);
void hotkey_log(const char *format, ...);

#endif  // HOTKEY_HANDLER_H
