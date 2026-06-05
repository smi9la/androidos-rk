#!/usr/bin/env python3
"""
AndroidOS Clock - Automatic Update System
Checks for updates every Monday at 7:00 AM
"""

import os
import sys
import json
import time
import requests
import schedule
from datetime import datetime
from pathlib import Path
from threading import Thread

# Configuration
VERSION = "1.0.0"
GITHUB_REPO = "smi9la/androidos-rk"
GITHUB_API = f"https://api.github.com/repos/{GITHUB_REPO}"
CHECK_TIME = "07:00"  # 7:00 AM
CHECK_DAY = "Monday"

class UpdateChecker:
    def __init__(self):
        self.current_version = VERSION
        self.latest_version = None
        self.update_available = False
        self.config_file = Path.home() / ".androidos-clock" / "update_config.json"
        self.config_file.parent.mkdir(parents=True, exist_ok=True)
        self.load_config()
    
    def load_config(self):
        """Load update configuration"""
        if self.config_file.exists():
            with open(self.config_file, 'r') as f:
                self.config = json.load(f)
        else:
            self.config = {
                "last_check": None,
                "auto_update": True,
                "notify_user": True,
                "check_time": CHECK_TIME,
                "check_day": CHECK_DAY
            }
            self.save_config()
    
    def save_config(self):
        """Save update configuration"""
        with open(self.config_file, 'w') as f:
            json.dump(self.config, f, indent=2)
    
    def get_latest_release(self):
        """Fetch latest release from GitHub"""
        try:
            response = requests.get(f"{GITHUB_API}/releases/latest", timeout=5)
            if response.status_code == 200:
                data = response.json()
                self.latest_version = data['tag_name'].lstrip('v')
                return data
            return None
        except Exception as e:
            print(f"Error fetching latest release: {e}")
            return None
    
    def compare_versions(self, v1, v2):
        """Compare two version strings
        Returns: -1 if v1 < v2, 0 if equal, 1 if v1 > v2
        """
        try:
            v1_parts = [int(x) for x in v1.split('.')]
            v2_parts = [int(x) for x in v2.split('.')]
            
            for i in range(max(len(v1_parts), len(v2_parts))):
                p1 = v1_parts[i] if i < len(v1_parts) else 0
                p2 = v2_parts[i] if i < len(v2_parts) else 0
                
                if p1 < p2:
                    return -1
                elif p1 > p2:
                    return 1
            return 0
        except:
            return 0
    
    def check_for_updates(self):
        """Check if update is available"""
        print(f"\n📅 Checking for updates... ({datetime.now().strftime('%Y-%m-%d %H:%M:%S')})")
        
        release_data = self.get_latest_release()
        
        if release_data:
            self.latest_version = release_data['tag_name'].lstrip('v')
            comparison = self.compare_versions(self.current_version, self.latest_version)
            
            if comparison < 0:  # Update available
                self.update_available = True
                self.config['last_check'] = datetime.now().isoformat()
                self.save_config()
                
                print(f"✅ Update available: v{self.current_version} → v{self.latest_version}")
                self.notify_update_available(release_data)
                return True
            else:
                print(f"✓ Already on latest version: v{self.current_version}")
                self.config['last_check'] = datetime.now().isoformat()
                self.save_config()
                return False
        else:
            print("⚠ Could not check for updates (network error)")
            return False
    
    def notify_update_available(self, release_data):
        """Notify user about available update"""
        notification = {
            "title": "🔔 Update Available!",
            "message": f"AndroidOS Clock v{self.latest_version} is now available",
            "current_version": self.current_version,
            "latest_version": self.latest_version,
            "download_url": release_data.get('html_url'),
            "release_notes": release_data.get('body', ''),
            "timestamp": datetime.now().isoformat()
        }
        
        # Save notification
        notif_file = Path.home() / ".androidos-clock" / "notifications.json"
        notifications = []
        
        if notif_file.exists():
            with open(notif_file, 'r') as f:
                notifications = json.load(f)
        
        notifications.append(notification)
        
        with open(notif_file, 'w') as f:
            json.dump(notifications, f, indent=2)
        
        print(f"\n📢 Notification saved to {notif_file}")
        return notification
    
    def schedule_check(self):
        """Schedule update check for Monday 7:00 AM"""
        print(f"\n⏰ Scheduling update checks for {CHECK_DAY} at {CHECK_TIME}")
        
        # Schedule for Monday at 7:00 AM
        schedule.every().monday.at(CHECK_TIME).do(self.check_for_updates)
        
        # Also check immediately on startup
        self.check_for_updates()
        
        print("✓ Update scheduler started")
        
        # Run scheduler in background
        def run_scheduler():
            while True:
                schedule.run_pending()
                time.sleep(60)  # Check every minute if scheduled tasks need to run
        
        scheduler_thread = Thread(target=run_scheduler, daemon=True)
        scheduler_thread.start()
    
    def get_notifications(self):
        """Get pending notifications"""
        notif_file = Path.home() / ".androidos-clock" / "notifications.json"
        
        if notif_file.exists():
            with open(notif_file, 'r') as f:
                return json.load(f)
        return []
    
    def clear_notifications(self):
        """Clear all notifications"""
        notif_file = Path.home() / ".androidos-clock" / "notifications.json"
        if notif_file.exists():
            notif_file.unlink()

def main():
    print("="*60)
    print("AndroidOS Clock - Update System")
    print("="*60)
    
    updater = UpdateChecker()
    
    # Start scheduler
    updater.schedule_check()
    
    # Keep running
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n\n✓ Update checker stopped")
        sys.exit(0)

if __name__ == "__main__":
    main()
