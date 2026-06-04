#ifndef ROM_SCANNER_H
#define ROM_SCANNER_H

#include <time.h>
#include <stdarg.h>

typedef struct {
  int id;
  char title[256];
  char filename[256];
  char system[32];
  char path[512];
  long size;
  time_t last_played;
  int play_count;
  int favorite;
} ROMInfo;

// Scanning
int rom_scan_all(void);
int rom_scan_directory(const char *system, const char *path);

// Querying
ROMInfo *rom_get_all(int *count);
ROMInfo *rom_get_favorites(int *count);
int rom_toggle_favorite(int rom_id);

// Lifecycle
int rom_init(void);
void rom_shutdown(void);
void rom_log(const char *format, ...);

#endif  // ROM_SCANNER_H
