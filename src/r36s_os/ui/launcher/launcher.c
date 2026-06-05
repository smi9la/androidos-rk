/*
 * ANDROIDOS-RK Launcher
 * Android-Inspired Graphical Interface for R36S/R35S
 * 
 * Framework: SDL2 for rendering
 * UI Toolkit: Custom lightweight framework
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FONT_SIZE 16

// Colors
#define COLOR_BG       {15, 15, 15, 255}        // Dark background
#define COLOR_PRIMARY  {0, 180, 220, 255}       // Cyan accent
#define COLOR_TEXT     {220, 220, 220, 255}    // Light text
#define COLOR_SECONDARY {45, 45, 45, 255}      // Gray

typedef enum {
    SCREEN_HOME,
    SCREEN_GAMES,
    SCREEN_SETTINGS,
    SCREEN_GAME_RUNNING
} LauncherScreen;

typedef struct {
    char id[64];
    char title[128];
    char emulator[64];
    char cover_path[256];
    char rom_path[256];
    SDL_Texture* cover_texture;
} Game;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    LauncherScreen current_screen;
    Game* games;
    int num_games;
    int selected_index;
    time_t start_time;
} LauncherState;

LauncherState launcher = {0};

// Initialize SDL and launcher
int launcher_init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 0;
    }
    
    launcher.window = SDL_CreateWindow(
        "AndriodOS-RK",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!launcher.window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    
    launcher.renderer = SDL_CreateRenderer(launcher.window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!launcher.renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(launcher.window);
        SDL_Quit();
        return 0;
    }
    
    launcher.current_screen = SCREEN_HOME;
    launcher.start_time = time(NULL);
    
    return 1;
}

// Load games from storage
int load_games_library() {
    // Scan for ROM directories
    const char* rom_dirs[] = {
        "/roms/psp",
        "/roms/nes",
        "/roms/snes",
        "/roms/gba",
        "/roms/genesis",
        NULL
    };
    
    launcher.games = malloc(sizeof(Game) * 256);
    launcher.num_games = 0;
    
    // Scan each ROM directory
    for (int i = 0; rom_dirs[i] != NULL; i++) {
        DIR* dir = opendir(rom_dirs[i]);
        if (dir == NULL) continue;
        
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type != DT_REG) continue;
            
            // Load game info (simplified)
            Game* game = &launcher.games[launcher.num_games++];
            snprintf(game->id, sizeof(game->id), "game_%d", launcher.num_games);
            snprintf(game->title, sizeof(game->title), "%s", entry->d_name);
            snprintf(game->rom_path, sizeof(game->rom_path), "%s/%s", rom_dirs[i], entry->d_name);
            
            // TODO: Load cover image
        }
        closedir(dir);
    }
    
    return launcher.num_games;
}

// Draw home screen
void draw_home_screen() {
    SDL_Color bg = COLOR_BG;
    SDL_SetRenderDrawColor(launcher.renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(launcher.renderer);
    
    // Status bar (time, battery, etc)
    SDL_Rect status_bar = {0, 0, SCREEN_WIDTH, 40};
    SDL_Color status_bg = COLOR_SECONDARY;
    SDL_SetRenderDrawColor(launcher.renderer, status_bg.r, status_bg.g, status_bg.b, status_bg.a);
    SDL_RenderFillRect(launcher.renderer, &status_bar);
    
    // Title
    SDL_Color text_color = COLOR_TEXT;
    SDL_SetRenderDrawColor(launcher.renderer, text_color.r, text_color.g, text_color.b, text_color.a);
    
    // Recent games horizontal scroll
    int y_offset = 50;
    int x_offset = 20;
    int game_width = 120;
    int game_height = 150;
    int games_per_row = 4;
    
    for (int i = 0; i < launcher.num_games && i < 4; i++) {
        SDL_Rect game_rect = {
            x_offset + i * (game_width + 20),
            y_offset,
            game_width,
            game_height
        };
        
        // Draw game thumbnail (placeholder)
        SDL_Color border = COLOR_PRIMARY;
        SDL_SetRenderDrawColor(launcher.renderer, border.r, border.g, border.b, border.a);
        
        // Border
        SDL_RenderDrawRect(launcher.renderer, &game_rect);
        
        // If selected, fill with highlight
        if (i == launcher.selected_index) {
            SDL_SetRenderDrawColor(launcher.renderer, border.r, border.g, border.b, 50);
            SDL_RenderFillRect(launcher.renderer, &game_rect);
        }
    }
    
    // Bottom quick access buttons
    int button_y = SCREEN_HEIGHT - 60;
    int button_height = 50;
    int button_width = (SCREEN_WIDTH - 60) / 5;
    
    const char* buttons[] = {"Browser", "Emulators", "Apps", "Files", "Settings"};
    for (int i = 0; i < 5; i++) {
        SDL_Rect btn = {
            20 + i * (button_width + 10),
            button_y,
            button_width,
            button_height
        };
        
        SDL_Color btn_color = COLOR_SECONDARY;
        SDL_SetRenderDrawColor(launcher.renderer, btn_color.r, btn_color.g, btn_color.b, btn_color.a);
        SDL_RenderFillRect(launcher.renderer, &btn);
        
        SDL_Color border = COLOR_PRIMARY;
        SDL_SetRenderDrawColor(launcher.renderer, border.r, border.g, border.b, border.a);
        SDL_RenderDrawRect(launcher.renderer, &btn);
    }
    
    SDL_RenderPresent(launcher.renderer);
}

// Draw settings screen
void draw_settings_screen() {
    SDL_Color bg = COLOR_BG;
    SDL_SetRenderDrawColor(launcher.renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(launcher.renderer);
    
    // Settings menu items
    const char* settings[] = {
        "Display",
        "Sound",
        "Network",
        "Storage",
        "Battery",
        "About Device"
    };
    
    int y = 40;
    int item_height = 50;
    
    for (int i = 0; i < 6; i++) {
        SDL_Rect item = {20, y, SCREEN_WIDTH - 40, item_height};
        
        if (i == launcher.selected_index) {
            SDL_Color highlight = COLOR_PRIMARY;
            SDL_SetRenderDrawColor(launcher.renderer, highlight.r, highlight.g, highlight.b, 30);
            SDL_RenderFillRect(launcher.renderer, &item);
        }
        
        SDL_Color border = COLOR_SECONDARY;
        SDL_SetRenderDrawColor(launcher.renderer, border.r, border.g, border.b, border.a);
        SDL_RenderDrawRect(launcher.renderer, &item);
        
        y += item_height + 10;
    }
    
    SDL_RenderPresent(launcher.renderer);
}

// Handle input
void handle_input() {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                exit(0);
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (launcher.selected_index > 0) launcher.selected_index--;
                        break;
                    case SDLK_DOWN:
                        launcher.selected_index++;
                        break;
                    case SDLK_LEFT:
                        if (launcher.current_screen != SCREEN_HOME) {
                            launcher.current_screen = SCREEN_HOME;
                            launcher.selected_index = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        // Navigate to selected screen
                        break;
                    case SDLK_RETURN:
                        // Launch selected item
                        if (launcher.current_screen == SCREEN_HOME) {
                            // Launch game
                            printf("Launching: %s\n", launcher.games[launcher.selected_index].title);
                        }
                        break;
                    case SDLK_ESCAPE:
                        launcher.current_screen = SCREEN_HOME;
                        launcher.selected_index = 0;
                        break;
                }
                break;
        }
    }
}

// Main launcher loop
void launcher_run() {
    int running = 1;
    
    while (running) {
        handle_input();
        
        // Draw appropriate screen
        switch (launcher.current_screen) {
            case SCREEN_HOME:
                draw_home_screen();
                break;
            case SCREEN_SETTINGS:
                draw_settings_screen();
                break;
            default:
                draw_home_screen();
        }
        
        SDL_Delay(16);  // ~60 FPS
    }
}

void launcher_cleanup() {
    if (launcher.games) free(launcher.games);
    if (launcher.renderer) SDL_DestroyRenderer(launcher.renderer);
    if (launcher.window) SDL_DestroyWindow(launcher.window);
    SDL_Quit();
}

int main() {
    if (!launcher_init()) {
        fprintf(stderr, "Launcher initialization failed\n");
        return 1;
    }
    
    if (load_games_library() == 0) {
        fprintf(stderr, "No games found\n");
    }
    
    launcher_run();
    launcher_cleanup();
    
    return 0;
}
