#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <sys/stat.h>
#include "launcher.h"
#include "../power/battery.h"
#include "../power/thermal.h"
#include "../roms/rom-scanner.h"

#define LOG_DIR "/var/log/androidos/ui"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BATTERY_UPDATE_INTERVAL 5000  // ms

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *bg_texture;
  int current_screen;
  int battery_percent;
  char time_str[32];
  int running;
} LauncherState;

static LauncherState g_launcher = {0};

void ui_log(const char *format, ...) {
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

int ui_init(void) {
  system("mkdir -p /var/log/androidos/ui");
  ui_log("Launcher UI initialized");
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    ui_log("ERROR: SDL_Init failed: %s", SDL_GetError());
    return -1;
  }
  
  g_launcher.window = SDL_CreateWindow(
    "ANDROIDOS-RK Launcher",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN
  );
  
  if (!g_launcher.window) {
    ui_log("ERROR: Failed to create window: %s", SDL_GetError());
    SDL_Quit();
    return -1;
  }
  
  g_launcher.renderer = SDL_CreateRenderer(g_launcher.window, -1, SDL_RENDERER_ACCELERATED);
  if (!g_launcher.renderer) {
    ui_log("ERROR: Failed to create renderer: %s", SDL_GetError());
    SDL_DestroyWindow(g_launcher.window);
    SDL_Quit();
    return -1;
  }
  
  g_launcher.current_screen = SCREEN_HOME;
  g_launcher.running = 1;
  
  ui_log("SDL initialized successfully (640x480)");
  return 0;
}

void ui_render_status_bar(void) {
  // Get current time
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  strftime(g_launcher.time_str, sizeof(g_launcher.time_str), "%H:%M", tm_info);
  
  // Get battery status
  battery_update_state();
  int capacity = battery_get_capacity();
  
  SDL_Rect status_bar = {0, 0, SCREEN_WIDTH, 30};
  SDL_SetRenderDrawColor(g_launcher.renderer, 33, 33, 33, 255);  // Dark gray
  SDL_RenderFillRect(g_launcher.renderer, &status_bar);
  
  // Battery indicator
  SDL_Rect battery_rect = {SCREEN_WIDTH - 50, 5, 45, 20};
  if (capacity >= 20) {
    SDL_SetRenderDrawColor(g_launcher.renderer, 0, 255, 0, 255);  // Green
  } else if (capacity >= 10) {
    SDL_SetRenderDrawColor(g_launcher.renderer, 255, 165, 0, 255);  // Orange
  } else {
    SDL_SetRenderDrawColor(g_launcher.renderer, 255, 0, 0, 255);  // Red
  }
  SDL_RenderDrawRect(g_launcher.renderer, &battery_rect);
  SDL_RenderFillRect(g_launcher.renderer, &battery_rect);
  
  ui_log("Rendered status bar: Battery %d%%, Time %s", capacity, g_launcher.time_str);
}

void ui_render_home_screen(void) {
  // Clear screen
  SDL_SetRenderDrawColor(g_launcher.renderer, 20, 20, 20, 255);
  SDL_RenderClear(g_launcher.renderer);
  
  // Render status bar
  ui_render_status_bar();
  
  // Title
  SDL_Rect title_rect = {50, 50, 540, 50};
  SDL_SetRenderDrawColor(g_launcher.renderer, 100, 150, 255, 255);
  SDL_RenderDrawRect(g_launcher.renderer, &title_rect);
  
  // Menu items
  SDL_Rect items[] = {
    {50, 120, 280, 60},   // Game Library
    {50, 190, 280, 60},   // Emulators
    {50, 260, 280, 60},   // Settings
    {50, 330, 280, 60}    // File Manager
  };
  
  for (int i = 0; i < 4; i++) {
    SDL_SetRenderDrawColor(g_launcher.renderer, 80, 80, 80, 255);
    SDL_RenderDrawRect(g_launcher.renderer, &items[i]);
  }
  
  ui_log("Rendered home screen");
}

void ui_render_game_library(void) {
  SDL_SetRenderDrawColor(g_launcher.renderer, 20, 20, 20, 255);
  SDL_RenderClear(g_launcher.renderer);
  
  ui_render_status_bar();
  
  int count = 0;
  ROMInfo *roms = rom_get_all(&count);
  
  SDL_Rect item = {10, 50, SCREEN_WIDTH - 20, 60};
  
  for (int i = 0; i < count && i < 6; i++) {
    SDL_SetRenderDrawColor(g_launcher.renderer, 60, 60, 60, 255);
    SDL_RenderDrawRect(g_launcher.renderer, &item);
    item.y += 70;
  }
  
  if (roms) free(roms);
  ui_log("Rendered game library with %d games", count);
}

void ui_handle_input(void) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        g_launcher.running = 0;
        ui_log("Quit event received");
        break;
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          g_launcher.running = 0;
          ui_log("Escape key pressed - exiting");
        }
        break;
    }
  }
}

void ui_update(void) {
  // Update display based on current screen
  switch (g_launcher.current_screen) {
    case SCREEN_HOME:
      ui_render_home_screen();
      break;
    case SCREEN_GAME_LIBRARY:
      ui_render_game_library();
      break;
  }
  
  SDL_RenderPresent(g_launcher.renderer);
}

int ui_run(void) {
  ui_log("Launcher main loop started");
  
  while (g_launcher.running) {
    ui_handle_input();
    ui_update();
    SDL_Delay(16);  // ~60 FPS
  }
  
  ui_log("Launcher main loop ended");
  return 0;
}

void ui_shutdown(void) {
  if (g_launcher.renderer) {
    SDL_DestroyRenderer(g_launcher.renderer);
  }
  if (g_launcher.window) {
    SDL_DestroyWindow(g_launcher.window);
  }
  SDL_Quit();
  ui_log("Launcher UI shutdown");
}
