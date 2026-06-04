#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include <time.h>
#include "rom-scanner.h"

#define ROM_BASE_PATH "/roms"
#define ROMS_DB "/var/cache/androidos/roms.db"
#define LOG_DIR "/var/log/androidos/roms"

static sqlite3 *g_db = NULL;
static int g_rom_count = 0;

void rom_log(const char *format, ...) {
  char log_path[256];
  snprintf(log_path, sizeof(log_path), "%s/scanner.log", LOG_DIR);
  
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

int rom_init_database(void) {
  char *err_msg = 0;
  int rc = sqlite3_open(ROMS_DB, &g_db);
  
  if (rc != SQLITE_OK) {
    rom_log("ERROR: Cannot open database: %s", sqlite3_errmsg(g_db));
    return -1;
  }
  
  // Create ROMs table
  const char *sql = "CREATE TABLE IF NOT EXISTS roms ("
    "id INTEGER PRIMARY KEY,"
    "title TEXT NOT NULL,"
    "filename TEXT NOT NULL,"
    "system TEXT NOT NULL,"
    "path TEXT NOT NULL,"
    "size INTEGER,"
    "added_date DATETIME,"
    "last_played DATETIME,"
    "play_count INTEGER DEFAULT 0,"
    "favorite INTEGER DEFAULT 0,"
    "UNIQUE(path))";
  
  rc = sqlite3_exec(g_db, sql, 0, 0, &err_msg);
  
  if (rc != SQLITE_OK) {
    rom_log("ERROR: SQL error: %s", err_msg);
    sqlite3_free(err_msg);
    return -1;
  }
  
  rom_log("Database initialized");
  return 0;
}

int rom_scan_directory(const char *system, const char *path) {
  DIR *dir = opendir(path);
  if (!dir) {
    rom_log("ERROR: Cannot open %s", path);
    return 0;
  }
  
  struct dirent *entry;
  int count = 0;
  
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type != DT_REG) continue;
    
    // Check file extension
    const char *ext = strrchr(entry->d_name, '.');
    if (!ext) continue;
    
    int valid = 0;
    if (strcmp(system, "psp") == 0 && strcmp(ext, ".iso") == 0) valid = 1;
    if (strcmp(system, "psp") == 0 && strcmp(ext, ".cso") == 0) valid = 1;
    if (strcmp(system, "ps1") == 0 && strcmp(ext, ".iso") == 0) valid = 1;
    if (strcmp(system, "gba") == 0 && strcmp(ext, ".gba") == 0) valid = 1;
    if (strcmp(system, "snes") == 0 && strcmp(ext, ".smc") == 0) valid = 1;
    if (strcmp(system, "snes") == 0 && strcmp(ext, ".sfc") == 0) valid = 1;
    if (strcmp(system, "nes") == 0 && strcmp(ext, ".nes") == 0) valid = 1;
    if (strcmp(system, "md") == 0 && strcmp(ext, ".md") == 0) valid = 1;
    if (strcmp(system, "md") == 0 && strcmp(ext, ".gen") == 0) valid = 1;
    
    if (!valid) continue;
    
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
    
    struct stat st;
    stat(full_path, &st);
    
    // Insert into database
    const char *sql = "INSERT OR REPLACE INTO roms (title, filename, system, path, size, added_date) "
                      "VALUES (?, ?, ?, ?, ?, datetime('now'))";
    
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL);
    
    // Remove extension from title
    char title[256];
    strcpy(title, entry->d_name);
    char *dot = strrchr(title, '.');
    if (dot) *dot = '\0';
    
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, entry->d_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, system, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, full_path, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, st.st_size);
    
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    count++;
  }
  
  closedir(dir);
  rom_log("Scanned %s: found %d ROMs", system, count);
  return count;
}

int rom_scan_all(void) {
  int total = 0;
  
  const char *systems[] = {"psp", "ps1", "gba", "snes", "nes", "md", NULL};
  
  for (int i = 0; systems[i] != NULL; i++) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", ROM_BASE_PATH, systems[i]);
    
    // Create directory if it doesn't exist
    mkdir(path, 0755);
    
    total += rom_scan_directory(systems[i], path);
  }
  
  g_rom_count = total;
  rom_log("Total ROMs found: %d", total);
  return total;
}

ROMInfo *rom_get_all(int *count) {
  const char *sql = "SELECT id, title, filename, system, path, size, last_played, play_count, favorite FROM roms ORDER BY title";
  
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL);
  
  // Count results
  int result_count = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    result_count++;
  }
  
  if (result_count == 0) {
    *count = 0;
    sqlite3_finalize(stmt);
    return NULL;
  }
  
  // Allocate memory
  ROMInfo *results = malloc(sizeof(ROMInfo) * result_count);
  
  // Reset and read again
  sqlite3_reset(stmt);
  int idx = 0;
  
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    ROMInfo *rom = &results[idx++];
    rom->id = sqlite3_column_int(stmt, 0);
    strcpy(rom->title, (const char *)sqlite3_column_text(stmt, 1));
    strcpy(rom->filename, (const char *)sqlite3_column_text(stmt, 2));
    strcpy(rom->system, (const char *)sqlite3_column_text(stmt, 3));
    strcpy(rom->path, (const char *)sqlite3_column_text(stmt, 4));
    rom->size = sqlite3_column_int64(stmt, 5);
    rom->play_count = sqlite3_column_int(stmt, 7);
    rom->favorite = sqlite3_column_int(stmt, 8);
  }
  
  sqlite3_finalize(stmt);
  *count = result_count;
  return results;
}

ROMInfo *rom_get_favorites(int *count) {
  const char *sql = "SELECT id, title, filename, system, path, size, last_played, play_count, favorite FROM roms WHERE favorite=1 ORDER BY title";
  
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL);
  
  int result_count = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    result_count++;
  }
  
  if (result_count == 0) {
    *count = 0;
    sqlite3_finalize(stmt);
    return NULL;
  }
  
  ROMInfo *results = malloc(sizeof(ROMInfo) * result_count);
  sqlite3_reset(stmt);
  int idx = 0;
  
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    ROMInfo *rom = &results[idx++];
    rom->id = sqlite3_column_int(stmt, 0);
    strcpy(rom->title, (const char *)sqlite3_column_text(stmt, 1));
    strcpy(rom->system, (const char *)sqlite3_column_text(stmt, 3));
    strcpy(rom->path, (const char *)sqlite3_column_text(stmt, 4));
    rom->favorite = 1;
  }
  
  sqlite3_finalize(stmt);
  *count = result_count;
  return results;
}

int rom_toggle_favorite(int rom_id) {
  const char *sql = "UPDATE roms SET favorite = CASE WHEN favorite=0 THEN 1 ELSE 0 END WHERE id=?";
  
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, rom_id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  
  rom_log("Toggled favorite for ROM %d", rom_id);
  return 0;
}

int rom_init(void) {
  system("mkdir -p /var/log/androidos/roms");
  system("mkdir -p /var/cache/androidos");
  system("mkdir -p /roms");
  
  rom_log("ROM scanner initialized");
  
  if (rom_init_database() != 0) {
    rom_log("ERROR: Failed to initialize database");
    return -1;
  }
  
  rom_scan_all();
  return 0;
}

void rom_shutdown(void) {
  if (g_db) {
    sqlite3_close(g_db);
  }
  rom_log("ROM scanner shutdown");
}
