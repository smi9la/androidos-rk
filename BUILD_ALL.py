#!/usr/bin/env python3
"""
R36s Smart System - Complete Build System
Android Phone Controller + R36s Device Firmware
All-in-one configuration and build automation
"""

import os
import sys
import json
import shutil
import zipfile
import subprocess
from pathlib import Path
from datetime import datetime

VERSION = "2.0.0"
APP_NAME = "r36s-controller"
BUILD_DATE = datetime.now().strftime("%Y-%m-%d")

# ============================================================================
# 1. ANDROID PHONE CONTROLLER CONFIGURATION
# ============================================================================

ANDROID_BUILD_GRADLE = """apply plugin: 'com.android.application'

android {
    compileSdkVersion 33
    defaultConfig {
        applicationId "com.r36s.controller"
        minSdkVersion 23
        targetSdkVersion 33
        versionCode 1
        versionName "2.0.0"
    }
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
        }
    }
}

dependencies {
    implementation 'androidx.appcompat:appcompat:1.6.1'
    implementation 'androidx.constraintlayout:constraintlayout:2.1.4'
    implementation 'com.google.android.material:material:1.10.0'
}
"""

ANDROID_MANIFEST = """<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.r36s.controller">
    
    <uses-permission android:name="android.permission.BLUETOOTH" />
    <uses-permission android:name="android.permission.BLUETOOTH_ADMIN" />
    <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
    <uses-permission android:name="android.permission.INTERNET" />
    
    <application
        android:allowBackup="true"
        android:icon="@mipmap/ic_launcher"
        android:label="@string/app_name"
        android:theme="@style/AppTheme">
        
        <activity
            android:name=".MainActivity"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
"""

ANDROID_MAIN_ACTIVITY = """package com.r36s.controller;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import java.util.Set;

public class MainActivity extends AppCompatActivity {
    private BluetoothAdapter bluetoothAdapter;
    private TextView statusView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        statusView = findViewById(R.id.statusView);
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        
        Button scanButton = findViewById(R.id.scanButton);
        scanButton.setOnClickListener(v -> scanForDevices());
        
        updateStatus();
    }

    private void scanForDevices() {
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        StringBuilder devices = new StringBuilder("Found devices:\\n");
        
        for (BluetoothDevice device : pairedDevices) {
            devices.append(device.getName()).append(" (").append(device.getAddress()).append(")\\n");
        }
        
        statusView.setText(devices.toString());
    }

    private void updateStatus() {
        if (bluetoothAdapter == null) {
            statusView.setText("Bluetooth not supported");
        } else if (bluetoothAdapter.isEnabled()) {
            statusView.setText("Bluetooth enabled. Ready to connect to R36s.");
        } else {
            statusView.setText("Bluetooth disabled. Enable to continue.");
        }
    }
}
"""

ANDROID_LAYOUT = """<?xml version="1.0" encoding="utf-8"?>
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:orientation="vertical"
    android:padding="20dp">

    <TextView
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="🎮 R36s Smart Controller"
        android:textSize="28sp"
        android:textStyle="bold"
        android:gravity="center"
        android:padding="20dp" />

    <TextView
        android:id="@+id/statusView"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="Initializing..."
        android:textSize="16sp"
        android:padding="20dp" />

    <Button
        android:id="@+id/scanButton"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="Scan for R36s Devices"
        android:textSize="18sp"
        android:padding="15dp" />

    <ScrollView
        android:layout_width="match_parent"
        android:layout_height="match_parent">
        
        <LinearLayout
            android:id="@+id/devicesGrid"
            android:layout_width="match_parent"
            android:layout_height="wrap_content"
            android:orientation="vertical"
            android:padding="16dp" />
    </ScrollView>
</LinearLayout>
"""

R36S_MAIN_C = """#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "r36s_config.h"
#include "bluetooth.h"
#include "display.h"
#include "power.h"

// R36s Smart System Main Firmware v2.0.0

#define VERSION "2.0.0"
#define DEVICE_NAME "R36s_Smart"

typedef struct {
    char version[16];
    uint8_t status;
    uint16_t battery;
    uint32_t uptime;
} system_info_t;

system_info_t sys_info = {
    .version = VERSION,
    .status = 0x01,
    .battery = 100,
    .uptime = 0
};

void init_device() {
    printf("Initializing R36s Smart System v%s\\n", VERSION);
    display_init();
    display_print("R36s Smart v2.0.0");
    bluetooth_init();
    bluetooth_set_name(DEVICE_NAME);
    power_init();
    printf("Initialization complete\\n");
}

void handle_bluetooth_command(const char* cmd) {
    if (strcmp(cmd, "STATUS") == 0) {
        printf("Status: Battery=%d%%, Uptime=%d\\n", sys_info.battery, sys_info.uptime);
    }
    else if (strcmp(cmd, "POWER_OFF") == 0) {
        power_shutdown();
    }
    else if (strcmp(cmd, "REBOOT") == 0) {
        power_reboot();
    }
}

int main() {
    init_device();
    while (1) {
        char cmd[256];
        if (bluetooth_receive(cmd, sizeof(cmd)) > 0) {
            handle_bluetooth_command(cmd);
        }
        sys_info.uptime++;
        display_update();
        usleep(100000);
    }
    return 0;
}
"""

R36S_BLUETOOTH_H = """#ifndef R36S_BLUETOOTH_H
#define R36S_BLUETOOTH_H

#include <stdint.h>

typedef struct {
    char name[32];
    uint8_t address[6];
    int8_t rssi;
} bt_device_t;

void bluetooth_init(void);
void bluetooth_set_name(const char* name);
int bluetooth_receive(char* buffer, int size);
int bluetooth_send(const char* data, int size);
void bluetooth_scan(void);

#endif
"""

R36S_CONFIG_H = """#ifndef R36S_CONFIG_H
#define R36S_CONFIG_H

#define R36S_VERSION "2.0.0"
#define DEVICE_TYPE_R36S 0x36
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define DISPLAY_FPS 60
#define BLE_MTU 512
#define BLE_TIMEOUT 5000
#define BATTERY_THRESHOLD 10
#define POWER_TIMEOUT 900
#define FLASH_SIZE (16 * 1024 * 1024)

#endif
"""

def create_android_files():
    print("📱 Creating Android Phone Controller files...")
    android_path = Path("phone/android/app/src/main")
    android_path.mkdir(parents=True, exist_ok=True)
    with open(Path("phone/android/app/build.gradle"), 'w') as f:
        f.write(ANDROID_BUILD_GRADLE)
    with open(android_path / "AndroidManifest.xml", 'w') as f:
        f.write(ANDROID_MANIFEST)
    java_path = android_path / "java/com/r36s/controller"
    java_path.mkdir(parents=True, exist_ok=True)
    with open(java_path / "MainActivity.java", 'w') as f:
        f.write(ANDROID_MAIN_ACTIVITY)
    layout_path = android_path / "res/layout"
    layout_path.mkdir(parents=True, exist_ok=True)
    with open(layout_path / "activity_main.xml", 'w') as f:
        f.write(ANDROID_LAYOUT)
    print("  ✓ Android phone controller files created")

def create_r36s_firmware():
    print("🎮 Creating R36s Device Firmware files...")
    firmware_path = Path("r36s/firmware/src")
    firmware_path.mkdir(parents=True, exist_ok=True)
    with open(firmware_path / "main.c", 'w') as f:
        f.write(R36S_MAIN_C)
    header_path = firmware_path.parent / "include"
    header_path.mkdir(exist_ok=True)
    with open(header_path / "bluetooth.h", 'w') as f:
        f.write(R36S_BLUETOOTH_H)
    with open(header_path / "r36s_config.h", 'w') as f:
        f.write(R36S_CONFIG_H)
    print("  ✓ R36s device firmware files created")

def main():
    print(f"\n{'='*60}")
    print(f"R36s Smart System - Complete Build System")
    print(f"Version {VERSION}")
    print(f"{'='*60}\n")
    try:
        create_android_files()
        create_r36s_firmware()
        print(f"\n{'='*60}")
        print("✅ All files created successfully!")
        print(f"{'='*60}\n")
        print("Next steps:")
        print("1. Phone Build:   cd phone && ./gradlew build")
        print("2. Firmware:      cd r36s && cmake --build .")
        print("\n🚀 Ready to build!\n")
        return 0
    except Exception as e:
        print(f"\n❌ Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
