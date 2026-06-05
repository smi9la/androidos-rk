# 📅 Automatic Update System - Monday 7:00 AM

## Overview

The AndroidOS Clock now has a comprehensive automatic update system that checks for new versions every **Monday at 7:00 AM**.

## Features

### 🖥️ Windows (PC) - Electron Auto-Updater
- ✅ Automatic checks every Monday 7:00 AM
- ✅ Silent background updates
- ✅ User notification when update available
- ✅ One-click installation
- ✅ Automatic restart after install

**Configuration:**
- Check time: Monday 7:00 AM (system time)
- Notification: Toast notification in top-right corner
- Download: Automatic in background
- Install: User prompt or silent install

### 📱 Android (Mobile) - WorkManager
- ✅ Background work scheduling
- ✅ Monday 7:00 AM checks
- ✅ Google Play Store integration ready
- ✅ Custom server updates support
- ✅ Push notifications

**Configuration:**
- Check time: Monday 7:00 AM (device time)
- Frequency: Weekly (7 days)
- Notification: Android notification bar
- Installation: Play Store or APK direct install

### 📦 Update Server - GitHub Actions
- ✅ Automated workflow runs Monday 7:00 AM UTC
- ✅ Checks for new releases
- ✅ Creates release notes automatically
- ✅ Logs update checks
- ✅ Notifies developers

**Workflow File:** `.github/workflows/auto-update-check.yml`

## How It Works

### Step 1: Update Check Trigger
**Every Monday at 7:00 AM:**
- PC: Electron auto-updater checks GitHub releases
- Mobile: WorkManager runs UpdateCheckWorker
- Server: GitHub Actions workflow executes

### Step 2: Version Comparison
- Current version vs Latest version
- If newer version found:
  - ✅ Download update package
  - ✅ Verify integrity
  - ✅ Prepare for installation

### Step 3: User Notification
**PC:**
```
🔔 Update Available
New version v1.0.1 is ready
[Later] [Install Now]
```

**Mobile:**
```
📱 AndroidOS Clock Update
Tap to update to latest version
```

### Step 4: Installation
- **PC:** Automatic restart and install
- **Mobile:** Direct to Play Store or APK install

## Configuration Files

### PC (Electron)
- **File:** `pc/main.js`
- **Module:** `electron-updater`
- **Schedule:** Every Monday 7:00 AM
- **Config:** `package.json` build section

### Mobile (Android)
- **File:** `mobile/android/app/src/main/java/.../UpdateCheckWorker.java`
- **Module:** `androidx.work.WorkManager`
- **Schedule:** Every 7 days (Monday)
- **Config:** `android/app/build.gradle`

### Server (GitHub)
- **File:** `.github/workflows/auto-update-check.yml`
- **Trigger:** Cron schedule `0 7 * * 1` (Monday 7:00 UTC)
- **Action:** Check releases, log, commit

## Setup Instructions

### 1. Enable Auto-Updater for PC
```bash
cd pc
npm install electron-updater
npm install
npm run build
```

### 2. Enable Work Manager for Android
```bash
cd mobile
# Add to build.gradle:
# implementation 'androidx.work:work-runtime:2.8.1'
./gradlew assembleRelease
```

### 3. GitHub Actions Setup
- Workflow file already in: `.github/workflows/auto-update-check.yml`
- No additional setup needed
- Runs automatically on schedule

## Testing

### Test PC Updates
```javascript
// In DevTools console:
require('electron').ipcRenderer.send('check-for-updates');
```

### Test Mobile Updates
```bash
# Run WorkManager test
adb shell am broadcast -a "com.android.intent.action.BOOT_COMPLETED"
```

### Test GitHub Actions
- Go to: Actions → auto-update-check
- Click: Run workflow → Run workflow

## User Experience

### Before Update
- 🟢 Green status in app (up-to-date)
- No notifications

### Update Available
- 🔔 Notification appears
- User can install immediately or defer
- Auto-downloads in background

### After Installation
- ✅ App restarts with new version
- 🎉 New features available

## Troubleshooting

### PC Update Not Working
1. Check internet connection
2. Verify GitHub releases exist
3. Check logs: `%APPDATA%/androidos-clock/logs`
4. Try manual check: Help → Check for Updates

### Mobile Update Not Working
1. Verify WorkManager is enabled
2. Check device battery optimization settings
3. Verify Google Play Services installed
4. Check logs in logcat: `adb logcat | grep UpdateCheck`

### GitHub Actions Not Running
1. Check workflow file syntax
2. Verify repository has Actions enabled
3. Check cron schedule: `0 7 * * 1` (UTC Monday 7:00 AM)
4. View logs in Actions tab

## Update Schedule

| Day | Time | Action |
|-----|------|--------|
| Monday | 7:00 AM | Check for updates |
| Any | Anytime | Manual check available |
| When Available | Automatic | Download & install |

## Version Format

Current: **v1.0.0**
- Format: `vMAJOR.MINOR.PATCH`
- Example: v1.0.1, v1.1.0, v2.0.0

## Security

- ✅ HTTPS only for downloads
- ✅ SHA256 signature verification
- ✅ Release authentication
- ✅ Tamper protection
- ✅ Rollback capability

## Disabling Auto-Updates

### PC (Electron)
```javascript
// In main.js:
// autoUpdater.checkForUpdatesAndNotify();  // Comment this line
```

### Mobile (Android)
```java
// In MainActivity.java:
// scheduleUpdateCheck();  // Comment this line
```

### GitHub Actions
- Disable workflow or delete `.github/workflows/auto-update-check.yml`

## Support

For update-related issues:
- **PC:** Help → Check for Updates
- **Mobile:** Settings → About → Check for Updates
- **GitHub:** https://github.com/smi9la/androidos-rk/issues

---

**Next Update Check:** Next Monday at 7:00 AM ⏰

**Version:** 1.0.0
**Last Updated:** 2026-06-05
