import React, { useState, useEffect } from 'react';
import './App.css';

function App() {
  const defaultTimezones = ['UTC', 'America/New_York', 'Europe/London', 'Europe/Paris', 
                           'Asia/Tokyo', 'Asia/Dubai', 'Australia/Sydney', 'America/Los_Angeles'];
  const [timezones, setTimezones] = useState(defaultTimezones);
  const [showAddPanel, setShowAddPanel] = useState(false);
  const [inputValue, setInputValue] = useState('');
  const [, setUpdateTrigger] = useState(0);
  const [updateNotification, setUpdateNotification] = useState(null);
  const [showUpdateModal, setShowUpdateModal] = useState(false);

  useEffect(() => {
    const interval = setInterval(() => setUpdateTrigger(prev => prev + 1), 1000);
    return () => clearInterval(interval);
  }, []);

  // Listen for update notifications
  useEffect(() => {
    if (window.electronAPI) {
      window.electronAPI.onUpdateAvailable?.(() => {
        setUpdateNotification({
          type: 'available',
          message: 'New version available! Download will start automatically.'
        });
        setShowUpdateModal(true);
      });

      window.electronAPI.onUpdateDownloaded?.(() => {
        setUpdateNotification({
          type: 'downloaded',
          message: 'Update downloaded! Restart to install.'
        });
      });
    }
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

  const installUpdate = () => {
    if (window.electronAPI?.installUpdate) {
      window.electronAPI.installUpdate();
    }
  };

  return (
    <div className="app-container">
      {/* Update Notification */}
      {updateNotification && (
        <div className={`update-notification ${updateNotification.type}`}>
          <span>🔔 {updateNotification.message}</span>
          {updateNotification.type === 'downloaded' && (
            <button onClick={installUpdate} className="install-btn">
              Install & Restart
            </button>
          )}
        </div>
      )}

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
                 onChange={(e) => setInputValue(e.target.value)} 
                 onKeyPress={(e) => e.key === 'Enter' && addTimezone()} />
          <button className="btn" onClick={addTimezone}>Add</button>
        </div>
      )}

      {/* Update Modal */}
      {showUpdateModal && (
        <div className="modal-overlay">
          <div className="modal">
            <h2>🔄 Update Available</h2>
            <p>A new version of AndroidOS Clock is available.</p>
            <p>The update will be installed on the next restart.</p>
            <button className="modal-btn" onClick={() => setShowUpdateModal(false)}>
              Later
            </button>
            <button className="modal-btn primary" onClick={installUpdate}>
              Install Now
            </button>
          </div>
        </div>
      )}
    </div>
  );
}

export default App;
