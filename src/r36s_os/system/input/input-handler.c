#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <syslog.h>
#include <json.h>  // For sending to launcher

#define MAX_INPUT_DEVICES 8
#define INPUT_DEVICES_PATH "/dev/input"

typedef struct {
    uint16_t buttons;      // Button states (bit field)
    int8_t analog_x;       // Left analog -127 to +127
    int8_t analog_y;
    int8_t analog_rx;      // Right analog
    int8_t analog_ry;
    uint16_t triggers;     // L1, L2, R1, R2
    uint32_t timestamp;
} GamepadState;

typedef struct {
    int fd;
    char path[256];
    char name[256];
    int active;
} InputDevice;

static InputDevice devices[MAX_INPUT_DEVICES] = {0};
static GamepadState current_state = {0};
static volatile int running = 1;

// Button mappings (standardized)
#define BTN_A      0x0001
#define BTN_B      0x0002
#define BTN_X      0x0004
#define BTN_Y      0x0008
#define BTN_L1     0x0010
#define BTN_L2     0x0020
#define BTN_R1     0x0040
#define BTN_R2     0x0080
#define BTN_START  0x0100
#define BTN_SELECT 0x0200
#define BTN_HOME   0x0400  // Power button
#define BTN_DPAD_UP    0x1000
#define BTN_DPAD_DOWN  0x2000
#define BTN_DPAD_LEFT  0x4000
#define BTN_DPAD_RIGHT 0x8000

// Detect input devices
int detect_input_devices() {
    int count = 0;
    syslog(LOG_INFO, "Scanning for input devices...");
    
    for (int i = 0; i < 16; i++) {
        char path[64];
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            char name[256] = {0};
            if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) > 0) {
                // Check if it's a gamepad/joystick
                if (strstr(name, "Joystick") || strstr(name, "Gamepad") || 
                    strstr(name, "Controller") || strstr(name, "Button")) {
                    
                    if (count < MAX_INPUT_DEVICES) {
                        devices[count].fd = fd;
                        strncpy(devices[count].path, path, sizeof(devices[count].path));
                        strncpy(devices[count].name, name, sizeof(devices[count].name));
                        devices[count].active = 1;
                        syslog(LOG_INFO, "Found input device: %s (%s)", name, path);
                        count++;
                        continue;
                    }
                }
            }
            close(fd);
        }
    }
    
    return count;
}

// Handle input events from device
void process_input_event(struct input_event* ev) {
    if (ev->type == EV_KEY) {
        // Button event
        switch (ev->code) {
            case BTN_NORTH:   // Y button
                if (ev->value) current_state.buttons |= BTN_Y;
                else current_state.buttons &= ~BTN_Y;
                break;
            case BTN_SOUTH:   // A button
                if (ev->value) current_state.buttons |= BTN_A;
                else current_state.buttons &= ~BTN_A;
                break;
            case BTN_WEST:    // X button
                if (ev->value) current_state.buttons |= BTN_X;
                else current_state.buttons &= ~BTN_X;
                break;
            case BTN_EAST:    // B button
                if (ev->value) current_state.buttons |= BTN_B;
                else current_state.buttons &= ~BTN_B;
                break;
            case BTN_TL:      // L1
                if (ev->value) current_state.buttons |= BTN_L1;
                else current_state.buttons &= ~BTN_L1;
                break;
            case BTN_TR:      // R1
                if (ev->value) current_state.buttons |= BTN_R1;
                else current_state.buttons &= ~BTN_R1;
                break;
            case BTN_SELECT:  // Select
                if (ev->value) current_state.buttons |= BTN_SELECT;
                else current_state.buttons &= ~BTN_SELECT;
                break;
            case BTN_START:   // Start
                if (ev->value) current_state.buttons |= BTN_START;
                else current_state.buttons &= ~BTN_START;
                break;
            case BTN_MODE:    // Home/Power
                if (ev->value) current_state.buttons |= BTN_HOME;
                else current_state.buttons &= ~BTN_HOME;
                break;
            case BTN_DPAD_UP:
                if (ev->value) current_state.buttons |= BTN_DPAD_UP;
                else current_state.buttons &= ~BTN_DPAD_UP;
                break;
            case BTN_DPAD_DOWN:
                if (ev->value) current_state.buttons |= BTN_DPAD_DOWN;
                else current_state.buttons &= ~BTN_DPAD_DOWN;
                break;
            case BTN_DPAD_LEFT:
                if (ev->value) current_state.buttons |= BTN_DPAD_LEFT;
                else current_state.buttons &= ~BTN_DPAD_LEFT;
                break;
            case BTN_DPAD_RIGHT:
                if (ev->value) current_state.buttons |= BTN_DPAD_RIGHT;
                else current_state.buttons &= ~BTN_DPAD_RIGHT;
                break;
        }
    } else if (ev->type == EV_ABS) {
        // Analog stick / trigger
        switch (ev->code) {
            case ABS_X:       // Left stick X
                current_state.analog_x = (int8_t)(ev->value / 256);
                break;
            case ABS_Y:       // Left stick Y
                current_state.analog_y = (int8_t)(ev->value / 256);
                break;
            case ABS_RX:      // Right stick X
                current_state.analog_rx = (int8_t)(ev->value / 256);
                break;
            case ABS_RY:      // Right stick Y
                current_state.analog_ry = (int8_t)(ev->value / 256);
                break;
            case ABS_Z:       // L2 trigger
                current_state.triggers = (current_state.triggers & 0xFF0F) | 
                                       ((ev->value >> 6) & 0x00F0);
                break;
            case ABS_RZ:      // R2 trigger
                current_state.triggers = (current_state.triggers & 0xF0FF) | 
                                       ((ev->value >> 2) & 0x0F00);
                break;
        }
    }
    
    current_state.timestamp = ev->time.tv_sec * 1000 + ev->time.tv_usec / 1000;
}

// Input reading thread
void* input_reader_thread(void* arg) {
    openlog("input-handler", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Input Handler started");
    
    struct input_event ev;
    
    while (running) {
        for (int i = 0; i < MAX_INPUT_DEVICES; i++) {
            if (!devices[i].active || devices[i].fd < 0) continue;
            
            // Read all available events
            while (read(devices[i].fd, &ev, sizeof(ev)) == sizeof(ev)) {
                process_input_event(&ev);
            }
        }
        
        usleep(10000);  // 10ms polling
    }
    
    syslog(LOG_INFO, "Input Handler stopped");
    closelog();
    return NULL;
}

// Get current gamepad state
GamepadState get_gamepad_state() {
    return current_state;
}

// Send input to active application via IPC/socket
void send_input_to_app(const GamepadState* state) {
    // This would connect to the active emulator/app via socket or named pipe
    // Implementation depends on the emulator's input interface
    json_object *jobj = json_object_new_object();
    json_object_object_add(jobj, "buttons", json_object_new_int(state->buttons));
    json_object_object_add(jobj, "analog_x", json_object_new_int(state->analog_x));
    json_object_object_add(jobj, "analog_y", json_object_new_int(state->analog_y));
    json_object_object_add(jobj, "analog_rx", json_object_new_int(state->analog_rx));
    json_object_object_add(jobj, "analog_ry", json_object_new_int(state->analog_ry));
    
    // Send via socket to active app
    // socket_send("localhost:9999", json_object_get_string(jobj));
    
    json_object_put(jobj);
}

int main() {
    pthread_t reader_thread;
    int num_devices;
    
    // Detect connected input devices
    num_devices = detect_input_devices();
    if (num_devices == 0) {
        syslog(LOG_WARNING, "No input devices detected");
    }
    
    // Start input reader thread
    if (pthread_create(&reader_thread, NULL, input_reader_thread, NULL) != 0) {
        perror("Failed to create input reader thread");
        return 1;
    }
    
    pthread_join(reader_thread, NULL);
    
    // Clean up
    for (int i = 0; i < MAX_INPUT_DEVICES; i++) {
        if (devices[i].fd >= 0) close(devices[i].fd);
    }
    
    return 0;
}
