#!/usr/bin/env python3
"""
AndroidOS Clock - Complete Build System
PC (EXE) + Mobile (APK) + Software Package (r36s)
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

VERSION = "1.0.0"
APP_NAME = "androidos-clock"
BUILD_DATE = datetime.now().strftime("%Y-%m-%d")

# ============================================================================
# 1. PC (WINDOWS EXE) CONFIGURATION
# ============================================================================

PC_PACKAGE_JSON = {
    "name": "androidos-clock-pc",
    "version": VERSION,
    "description": "Multi-Timezone Digital Clock for PC",
    "main": "main.js",
    "homepage": "./",
    "scripts": {
        "react-start": "react-scripts start",
        "react-build": "react-scripts build",
        "electron-start": "electron .",
        "electron-dev": "concurrently \"npm run react-start\" \"wait-on http://localhost:3000 && electron .\"",
        "build": "npm run react-build && electron-builder --win --publish=never"
    },
    "dependencies": {
        "react": "^18.2.0",
        "react-dom": "^18.2.0",
        "react-scripts": "5.0.1"
    },
    "devDependencies": {
        "concurrently": "^7.6.0",
        "electron": "^27.0.0",
        "electron-builder": "^24.6.4",
        "wait-on": "^7.0.1"
    },
    "build": {
        "appId": "com.androidos.clock.pc",
        "productName": "AndroidOS Clock",
        "files": ["build/**/*", "main.js", "preload.js"],
        "win": {
            "target": ["nsis", "portable"],
            "certificateFile": None
        },
        "nsis": {
            "oneClick": False,
            "allowToChangeInstallationDirectory": True,
            "createDesktopShortcut": True
        }
    }
}

PC_MAIN_JS = """const { app, BrowserWindow, Menu } = require('electron');
const path = require('path');
const isDev = require('electron-is-dev');

let mainWindow;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1200, height: 800,
    webPreferences: { nodeIntegration: false, contextIsolation: true }
  });

  const startUrl = isDev ? 'http://localhost:3000' 
    : `file://${path.join(__dirname, '../build/index.html')}`;

  mainWindow.loadURL(startUrl);
  if (isDev) mainWindow.webContents.openDevTools();
  mainWindow.on('closed', () => { mainWindow = null; });
}

app.on('ready', createWindow);
app.on('window-all-closed', () => { if (process.platform !== 'darwin') app.quit(); });
app.on('activate', () => { if (mainWindow === null) createWindow(); });

const template = [
  {
    label: 'File',
    submenu: [{ label: 'Exit', accelerator: 'CmdOrCtrl+Q', click: () => app.quit() }]
  },
  {
    label: 'View',
    submenu: [
      { label: 'Toggle Dev Tools', accelerator: 'CmdOrCtrl+Shift+I', 
        click: () => mainWindow.webContents.toggleDevTools() }
    ]
  }
];

Menu.setApplicationMenu(Menu.buildFromTemplate(template));
"""

PC_PRELOAD_JS = """const { contextBridge } = require('electron');
contextBridge.exposeInMainWorld('electronAPI', {
  getAppVersion: () => require('../package.json').version
});
"""

PC_APP_JS = """import React, { useState, useEffect } from 'react';
import './App.css';

function App() {
  const defaultTimezones = ['UTC', 'America/New_York', 'Europe/London', 'Europe/Paris', 
                           'Asia/Tokyo', 'Asia/Dubai', 'Australia/Sydney', 'America/Los_Angeles'];
  const [timezones, setTimezones] = useState(defaultTimezones);
  const [showAddPanel, setShowAddPanel] = useState(false);
  const [inputValue, setInputValue] = useState('');
  const [, setUpdateTrigger] = useState(0);

  useEffect(() => {
    const interval = setInterval(() => setUpdateTrigger(prev => prev + 1), 1000);
    return () => clearInterval(interval);
  }, []);

  const formatTime = (date) => {
    const h = String(date.getHours()).padStart(2, '0');
    const m = String(date.getMinutes()).padStart(2, '0');
    const s = String(date.getSeconds()).padStart(2, '0');
    return `${h}:${m}:${s}`;
  };

  const getTimeInTimezone = (tz) => 
    new Date(new Date().toLocaleString('en-US', { timeZone: tz }));

  const addTimezone = () => {
    if (inputValue.trim() && !timezones.includes(inputValue.trim())) {
      try {
        new Date().toLocaleString('en-US', { timeZone: inputValue.trim() });
        setTimezones([...timezones, inputValue.trim()]);
        setInputValue('');
        setShowAddPanel(false);
      } catch (e) { alert('Invalid timezone'); }
    }
  };

  return (
    <div className="app-container">
      <h1 className="title">⏰ Global Time Zones</h1>
      <div className="clocks-grid">
        {timezones.map((tz) => {
          const tzDate = getTimeInTimezone(tz);
          return (
            <div key={tz} className="clock-card">
              <div className="timezone-name">{tz}</div>
              <div className="digital-time">{formatTime(tzDate)}</div>
            </div>
          );
        })}
      </div>
      <div className="controls">
        <button className="btn" onClick={() => setShowAddPanel(true)}>+ Add</button>
        <button className="btn" onClick={() => setTimezones(defaultTimezones)}>Reset</button>
      </div>
      {showAddPanel && (
        <div className="add-timezone">
          <input type="text" placeholder="Timezone" value={inputValue} 
                 onChange={(e) => setInputValue(e.target.value)} onKeyPress={(e) => e.key === 'Enter' && addTimezone()} />
          <button className="btn" onClick={addTimezone}>Add</button>
        </div>
      )}
    </div>
  );
}
export default App;
"""

PC_APP_CSS = """.app-container {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  padding: 20px;
}

.title {
  color: white;
  margin-bottom: 40px;
  font-size: 2.5em;
  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
}

.clocks-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 30px;
  margin-bottom: 30px;
  max-width: 1200px;
}

.clock-card {
  background: rgba(255, 255, 255, 0.95);
  border-radius: 20px;
  padding: 30px;
  box-shadow: 0 10px 40px rgba(0, 0, 0, 0.3);
  transition: transform 0.3s ease;
}

.clock-card:hover {
  transform: translateY(-10px);
}

.timezone-name {
  font-size: 1.2em;
  font-weight: bold;
  color: #667eea;
  margin-bottom: 15px;
  text-align: center;
}

.digital-time {
  font-size: 2.5em;
  font-weight: bold;
  text-align: center;
  color: #333;
  font-family: 'Courier New', monospace;
}

.controls {
  text-align: center;
  margin-top: 30px;
}

.btn {
  background: white;
  color: #667eea;
  border: none;
  padding: 12px 30px;
  font-size: 1em;
  border-radius: 25px;
  cursor: pointer;
  margin: 0 10px;
  font-weight: bold;
  transition: all 0.3s ease;
}

.btn:hover {
  background: #667eea;
  color: white;
}

.add-timezone {
  display: flex;
  justify-content: center;
  gap: 10px;
  margin-top: 20px;
}

.timezone-input {
  padding: 10px 15px;
  border: 2px solid white;
  border-radius: 25px;
  background: rgba(255, 255, 255, 0.9);
  color: #333;
}
"""

# ============================================================================
# 2. MOBILE (ANDROID APK) CONFIGURATION
# ============================================================================

ANDROID_BUILD_GRADLE = """apply plugin: 'com.android.application'

android {
    compileSdkVersion 33
    defaultConfig {
        applicationId "com.androidos.clock.mobile"
        minSdkVersion 21
        targetSdkVersion 33
        versionCode 1
        versionName "1.0.0"
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
    package="com.androidos.clock.mobile">
    
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

ANDROID_MAIN_ACTIVITY = """package com.androidos.clock.mobile;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.LinearLayout;
import android.widget.TextView;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

public class MainActivity extends AppCompatActivity {
    private String[] timezones = {"UTC", "America/New_York", "Europe/London", 
                                  "Europe/Paris", "Asia/Tokyo", "Asia/Dubai", 
                                  "Australia/Sydney", "America/Los_Angeles"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        LinearLayout grid = findViewById(R.id.clocksGrid);
        
        for (String tz : timezones) {
            TextView clockView = new TextView(this);
            clockView.setText(getFormattedTime(tz));
            clockView.setTextSize(24);
            grid.addView(clockView);
        }
        
        Thread updateThread = new Thread(() -> {
            while (true) {
                try {
                    Thread.sleep(1000);
                    runOnUiThread(() -> {
                        LinearLayout g = findViewById(R.id.clocksGrid);
                        g.removeAllViews();
                        for (String tz : timezones) {
                            TextView v = new TextView(MainActivity.this);
                            v.setText(getFormattedTime(tz));
                            v.setTextSize(24);
                            g.addView(v);
                        }
                    });
                } catch (InterruptedException e) { e.printStackTrace(); }
            }
        });
        updateThread.setDaemon(true);
        updateThread.start();
    }

    private String getFormattedTime(String tz) {
        SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss", Locale.US);
        sdf.setTimeZone(TimeZone.getTimeZone(tz));
        return tz + ": " + sdf.format(new Date());
    }
}
"""

ANDROID_LAYOUT = """<?xml version="1.0" encoding="utf-8"?>
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:orientation="vertical"
    android:padding="16dp"
    android:background="#f5f5f5">

    <TextView
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        android:text="⏰ Global Time Zones"
        android:textSize="28sp"
        android:textStyle="bold"
        android:gravity="center"
        android:padding="20dp" />

    <ScrollView
        android:layout_width="match_parent"
        android:layout_height="match_parent">
        
        <LinearLayout
            android:id="@+id/clocksGrid"
            android:layout_width="match_parent"
            android:layout_height="wrap_content"
            android:orientation="vertical"
            android:padding="16dp" />
    </ScrollView>
</LinearLayout>
"""

# ============================================================================
# 3. BUILD FUNCTIONS
# ============================================================================

def create_pc_files():
    """Create all PC/Electron files"""
    print("🖥️  Creating PC (Electron) files...")
    
    pc_path = Path("pc")
    pc_path.mkdir(exist_ok=True)
    
    # Package.json
    with open(pc_path / "package.json", 'w') as f:
        json.dump(PC_PACKAGE_JSON, f, indent=2)
    
    # Main files
    with open(pc_path / "main.js", 'w') as f:
        f.write(PC_MAIN_JS)
    
    with open(pc_path / "preload.js", 'w') as f:
        f.write(PC_PRELOAD_JS)
    
    with open(pc_path / "App.js", 'w') as f:
        f.write(PC_APP_JS)
    
    with open(pc_path / "App.css", 'w') as f:
        f.write(PC_APP_CSS)
    
    print("  ✓ PC files created")

def create_mobile_files():
    """Create all Android files"""
    print("📱 Creating Mobile (Android) files...")
    
    android_path = Path("mobile/android/app/src/main")
    android_path.mkdir(parents=True, exist_ok=True)
    
    # Build gradle
    with open(Path("mobile/android/app/build.gradle"), 'w') as f:
        f.write(ANDROID_BUILD_GRADLE)
    
    # Manifest
    with open(android_path / "AndroidManifest.xml", 'w') as f:
        f.write(ANDROID_MANIFEST)
    
    # Java files
    java_path = android_path / "java/com/androidos/clock/mobile"
    java_path.mkdir(parents=True, exist_ok=True)
    
    with open(java_path / "MainActivity.java", 'w') as f:
        f.write(ANDROID_MAIN_ACTIVITY)
    
    # Layout
    layout_path = android_path / "res/layout"
    layout_path.mkdir(parents=True, exist_ok=True)
    
    with open(layout_path / "activity_main.xml", 'w') as f:
        f.write(ANDROID_LAYOUT)
    
    print("  ✓ Mobile files created")

def create_software_package():
    """Create complete software package"""
    print("📦 Creating Software Package (r36s)...")
    
    pkg_path = Path(f"androidos-clock-software-{VERSION}")
    
    # Create directories
    (pkg_path / "PC" / "Installer").mkdir(parents=True, exist_ok=True)
    (pkg_path / "PC" / "Portable").mkdir(parents=True, exist_ok=True)
    (pkg_path / "Mobile" / "Release").mkdir(parents=True, exist_ok=True)
    (pkg_path / "Documentation").mkdir(parents=True, exist_ok=True)
    (pkg_path / "Scripts").mkdir(parents=True, exist_ok=True)
    
    # Create README
    readme = f"""# AndroidOS Clock Software Package {VERSION}

## 📦 Contents

- **PC/Installer** - Windows installation package
- **PC/Portable** - Standalone executable
- **Mobile/Release** - Android APK
- **Documentation** - User guides and instructions
- **Scripts** - Installation scripts

## 🚀 Quick Start

### Windows
Run: `PC/Installer/setup.exe`

### Android
Transfer APK to phone and install

## 📋 Features

✓ Multi-timezone display
✓ Real-time updates
✓ Beautiful UI
✓ Cross-platform support

## 💻 System Requirements

**Windows:** 7 or higher, 200MB space
**Android:** 5.0+, 50MB space

## 📞 Support
https://github.com/smi9la/androidos-rk

Created: {BUILD_DATE}
Version: {VERSION}
"""
    
    with open(pkg_path / "README.md", 'w') as f:
        f.write(readme)
    
    # Create metadata
    metadata = {
        "name": "AndroidOS Clock",
        "version": VERSION,
        "buildDate": BUILD_DATE,
        "platforms": ["Windows", "Android"],
        "buildCommands": {
            "pc": "npm run build (in pc/ directory)",
            "mobile": "./gradlew assembleRelease (in mobile/ directory)"
        }
    }
    
    with open(pkg_path / "package.json", 'w') as f:
        json.dump(metadata, f, indent=2)
    
    # Create build instructions
    with open(pkg_path / "BUILD.md", 'w') as f:
        f.write("""# Build Instructions

## PC (Windows EXE)
cd pc
npm install
npm run build

Output: pc/dist/

## Mobile (Android APK)
cd mobile
./gradlew assembleRelease

Output: mobile/android/app/build/outputs/apk/release/

## Complete Package
python3 create_software_package.py
""")
    
    print("  ✓ Software package created")
    
    return pkg_path

def create_master_build_script():
    """Create master build script"""
    print("🔨 Creating master build script...")
    
    build_script = f"""#!/bin/bash
# AndroidOS Clock - Master Build Script v{VERSION}

echo "======================================"
echo "AndroidOS Clock Build System"
echo "Version: {VERSION}"
echo "======================================"
echo ""

# Build PC
echo "🖥️  Building PC (Electron) version..."
cd pc
npm install
npm run build
cd ..

# Build Mobile
echo "📱 Building Mobile (Android) version..."
cd mobile
./gradlew assembleRelease
cd ..

# Create package
echo "📦 Creating software package..."
python3 create_software_package.py

echo ""
echo "✅ Build complete!"
echo "Check pc/dist/ and mobile/android/app/build/outputs/ for outputs"
"""
    
    with open("build.sh", 'w') as f:
        f.write(build_script)
    os.chmod("build.sh", 0o755)
    
    print("  ✓ Build script created")

def main():
    """Main function"""
    print(f"\n{'='*60}")
    print(f"AndroidOS Clock - Complete Build System")
    print(f"Version {VERSION}")
    print(f"{'='*60}\n")
    
    try:
        # Create all files
        create_pc_files()
        create_mobile_files()
        pkg_path = create_software_package()
        create_master_build_script()
        
        print(f"\n{'='*60}")
        print("✅ All files created successfully!")
        print(f"{'='*60}\n")
        print("Next steps:")
        print("1. PC Build:   cd pc && npm install && npm run build")
        print("2. Mobile:    cd mobile && ./gradlew assembleRelease")
        print("3. Package:   python3 create_software_package.py")
        print("4. Or run all: bash build.sh")
        print("\n🚀 Ready to build!\n")
        
        return 0
        
    except Exception as e:
        print(f"\n❌ Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
