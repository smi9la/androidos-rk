#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>

#define BATTERY_PATH "/sys/class/power_supply/battery"
#define THERMAL_PATH "/sys/class/thermal"
#define CPUFREQ_PATH "/sys/devices/system/cpu/cpu0/cpufreq"
#define CRITICAL_TEMP 85.0
#define WARN_TEMP 75.0
#define LOW_BATTERY 10
#define CRITICAL_BATTERY 5

typedef struct {
    int level;              // 0-100%
    float temperature;      // Celsius
    int voltage;            // millivolts
    char status[32];        // Charging, Discharging, Not charging, Full
    char health[32];        // Good, Overheat, Dead, Over voltage, etc.
    int current;            // milliamps
    uint64_t uptime_ms;     // milliseconds
} PowerState;

typedef struct {
    float current_temp;     // CPU temperature
    float warning_threshold;  // 75°C
    float critical_threshold; // 85°C
    int throttle_level;     // 0-5
} ThermalState;

static volatile int running = 1;
static PowerState current_power = {0};
static ThermalState current_thermal = {0};

// Read battery level from sysfs
int read_battery_capacity() {
    char path[256];
    char buf[32];
    int fd, capacity = -1;
    
    snprintf(path, sizeof(path), "%s/capacity", BATTERY_PATH);
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        if (read(fd, buf, sizeof(buf)) > 0) {
            capacity = atoi(buf);
        }
        close(fd);
    } else {
        syslog(LOG_ERR, "Failed to read battery capacity from %s", path);
    }
    return capacity;
}

// Read battery temperature
float read_battery_temperature() {
    char path[256];
    char buf[32];
    int fd;
    float temp = 0.0;
    
    snprintf(path, sizeof(path), "%s/temp", BATTERY_PATH);
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        if (read(fd, buf, sizeof(buf)) > 0) {
            // temp is in tenths of a degree
            temp = atoi(buf) / 10.0;
        }
        close(fd);
    }
    return temp;
}

// Read CPU temperature from thermal zones
float read_cpu_temperature() {
    char path[256];
    char buf[32];
    int fd, temp_raw = 0;
    float temp = 0.0;
    
    // Try thermal_zone0
    snprintf(path, sizeof(path), "%s/thermal_zone0/temp", THERMAL_PATH);
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        if (read(fd, buf, sizeof(buf)) > 0) {
            temp_raw = atoi(buf);
            temp = temp_raw / 1000.0;  // millidegrees to degrees
        }
        close(fd);
    }
    return temp;
}

// Read battery voltage
int read_battery_voltage() {
    char path[256];
    char buf[32];
    int fd, voltage = 0;
    
    snprintf(path, sizeof(path), "%s/voltage_now", BATTERY_PATH);
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        if (read(fd, buf, sizeof(buf)) > 0) {
            voltage = atoi(buf) / 1000;  // microvolts to millivolts
        }
        close(fd);
    }
    return voltage;
}

// Read battery status
void read_battery_status(char *status, size_t len) {
    char path[256];
    int fd;
    
    snprintf(path, sizeof(path), "%s/status", BATTERY_PATH);
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        read(fd, status, len - 1);
        close(fd);
        // Remove newline
        for (int i = 0; i < len; i++) {
            if (status[i] == '\n') status[i] = '\0';
        }
    } else {
        strncpy(status, "Unknown", len - 1);
    }
}

// Handle low battery warning
void handle_low_battery(int level) {
    if (level <= CRITICAL_BATTERY) {
        syslog(LOG_ALERT, "CRITICAL: Battery at %d%%. Initiating emergency shutdown.", level);
        // Send signal to launcher to show critical battery dialog
        system("pkill -SIGUSR1 launcher");  // Custom signal handling
        sleep(5);  // Allow user to see warning
        // Graceful shutdown
        system("systemctl poweroff");
    } else if (level <= LOW_BATTERY) {
        syslog(LOG_WARNING, "WARNING: Battery at %d%%. Low battery mode enabled.", level);
        system("pkill -SIGUSR2 launcher");  // Custom signal handling
    }
}

// Handle thermal throttling
void handle_thermal_throttle(float temp) {
    if (temp >= CRITICAL_TEMP) {
        syslog(LOG_ALERT, "CRITICAL TEMPERATURE: %f°C. Shutting down for safety.", temp);
        system("systemctl poweroff");
    } else if (temp >= WARN_TEMP) {
        syslog(LOG_WARNING, "HIGH TEMPERATURE: %f°C. Throttling CPU.", temp);
        // Set CPU governor to "powersave" to reduce heat
        system("echo powersave > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    } else if (temp < (WARN_TEMP - 5.0)) {
        // Cool down, restore performance
        system("echo ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    }
}

// Main power monitoring loop
void* power_monitor_thread(void* arg) {
    openlog("power-manager", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Power Manager started");
    
    while (running) {
        // Read all power states
        current_power.level = read_battery_capacity();
        current_power.temperature = read_battery_temperature();
        current_power.voltage = read_battery_voltage();
        read_battery_status(current_power.status, sizeof(current_power.status));
        
        current_thermal.current_temp = read_cpu_temperature();
        
        // Handle low battery
        if (current_power.level >= 0) {
            handle_low_battery(current_power.level);
        }
        
        // Handle thermal issues
        if (current_thermal.current_temp > 0) {
            handle_thermal_throttle(current_thermal.current_temp);
        }
        
        // Log status every 60 seconds
        static int log_counter = 0;
        if (log_counter++ >= 60) {
            syslog(LOG_INFO, "Battery: %d%% | Temp: %.1f°C | Status: %s",
                   current_power.level, current_thermal.current_temp, current_power.status);
            log_counter = 0;
        }
        
        sleep(1);  // Check every second
    }
    
    syslog(LOG_INFO, "Power Manager stopped");
    closelog();
    return NULL;
}

// Get current power state (for API/UI)
PowerState get_power_state() {
    return current_power;
}

// Get current thermal state
ThermalState get_thermal_state() {
    return current_thermal;
}

// Signal handler for graceful shutdown
void signal_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        syslog(LOG_INFO, "Received shutdown signal");
        running = 0;
    }
}

int main() {
    pthread_t monitor_thread;
    struct sigaction sa;
    
    // Register signal handler
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    
    // Start monitoring thread
    if (pthread_create(&monitor_thread, NULL, power_monitor_thread, NULL) != 0) {
        perror("Failed to create monitor thread");
        return 1;
    }
    
    // Wait for thread
    pthread_join(monitor_thread, NULL);
    
    return 0;
}
