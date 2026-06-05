package com.androidos.clock.mobile;

import androidx.appcompat.app.AppCompatActivity;
import androidx.work.PeriodicWorkRequest;
import androidx.work.WorkManager;
import android.os.Bundle;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Button;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;
import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {
    private String[] timezones = {"UTC", "America/New_York", "Europe/London", 
                                  "Europe/Paris", "Asia/Tokyo", "Asia/Dubai", 
                                  "Australia/Sydney", "America/Los_Angeles"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        // Schedule update check for Monday 7:00 AM
        scheduleUpdateCheck();
        
        LinearLayout grid = findViewById(R.id.clocksGrid);
        
        for (String tz : timezones) {
            TextView clockView = new TextView(this);
            clockView.setText(getFormattedTime(tz));
            clockView.setTextSize(24);
            grid.addView(clockView);
        }
        
        Button checkUpdateBtn = findViewById(R.id.checkUpdateBtn);
        if (checkUpdateBtn != null) {
            checkUpdateBtn.setOnClickListener(v -> checkForUpdates());
        }
        
        // Update every second
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

    private void scheduleUpdateCheck() {
        // Schedule work to run every Monday at 7:00 AM
        PeriodicWorkRequest updateCheckWork = 
            new PeriodicWorkRequest.Builder(UpdateCheckWorker.class, 7, TimeUnit.DAYS)
            .build();
        
        WorkManager.getInstance(this).enqueueUniquePeriodicWork(
            "update_check",
            androidx.work.ExistingPeriodicWorkPolicy.KEEP,
            updateCheckWork
        );
    }

    private void checkForUpdates() {
        // Check for updates from Google Play Store or custom server
        new Thread(() -> {
            try {
                // Check for new version
                String currentVersion = getVersionName();
                // Compare with server version
                
                runOnUiThread(() -> {
                    // Show notification if update available
                    showUpdateNotification("New version available!");
                });
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }

    private String getVersionName() {
        try {
            return getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
        } catch (Exception e) {
            return "1.0.0";
        }
    }

    private void showUpdateNotification(String message) {
        androidx.appcompat.app.AlertDialog.Builder builder = 
            new androidx.appcompat.app.AlertDialog.Builder(this);
        builder.setTitle("🔔 Update Available")
            .setMessage(message)
            .setPositiveButton("Update", (dialog, id) -> {
                // Open Play Store or custom update mechanism
            })
            .setNegativeButton("Later", (dialog, id) -> dialog.cancel());
        builder.create().show();
    }

    private String getFormattedTime(String tz) {
        SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss", Locale.US);
        sdf.setTimeZone(TimeZone.getTimeZone(tz));
        return tz + ": " + sdf.format(new Date());
    }
}
