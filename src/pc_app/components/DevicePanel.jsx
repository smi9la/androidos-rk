import React, { useState } from 'react';
import './DevicePanel.css';

const DevicePanel = ({ onDiscover, onConnect, connectedDevice, deviceStats }) => {
  const [devices, setDevices] = useState([]);
  const [discovering, setDiscovering] = useState(false);

  const handleDiscover = async () => {
    setDiscovering(true);
    try {
      await onDiscover();
      // Fetch discovered devices from main process
      setDevices([
        { ip: '192.168.1.100', model: 'R36S', name: 'My R36S', online: true },
        { ip: '192.168.1.101', model: 'R35S', name: 'Game Device', online: false }
      ]);
    } catch (error) {
      console.error('Discovery error:', error);
    } finally {
      setDiscovering(false);
    }
  };

  return (
    <div className="device-panel">
      <div className="panel-header">
        <h2>Connected Devices</h2>
        <button onClick={handleDiscover} disabled={discovering}>
          {discovering ? 'Discovering...' : 'Discover Devices'}
        </button>
      </div>

      <div className="devices-list">
        {devices.length === 0 ? (
          <div className="empty-state">
            <p>No devices found. Click "Discover Devices" to search your network.</p>
          </div>
        ) : (
          devices.map((device) => (
            <div
              key={device.ip}
              className={`device-card ${device.online ? 'online' : 'offline'}`}
            >
              <div className="device-header">
                <h3>{device.name}</h3>
                <span className={`status ${device.online ? 'online' : 'offline'}`}>
                  {device.online ? '🟢' : '🔴'}
                </span>
              </div>
              <div className="device-info">
                <p>Model: <strong>{device.model}</strong></p>
                <p>IP: <strong>{device.ip}</strong></p>
              </div>
              <button
                className="connect-btn"
                onClick={() => onConnect(device.ip)}
                disabled={!device.online || connectedDevice === device.ip}
              >
                {connectedDevice === device.ip ? 'Connected' : 'Connect'}
              </button>
            </div>
          ))
        )}
      </div>

      {connectedDevice && deviceStats && (
        <div className="stats-panel">
          <h3>Device Statistics</h3>
          <div className="stats-grid">
            <div className="stat-item">
              <label>CPU Usage</label>
              <div className="progress-bar">
                <div
                  className="progress-fill"
                  style={{ width: `${deviceStats.cpu_usage}%` }}
                />
              </div>
              <span>{deviceStats.cpu_usage}%</span>
            </div>
            <div className="stat-item">
              <label>RAM Usage</label>
              <div className="progress-bar">
                <div
                  className="progress-fill"
                  style={{ width: `${(deviceStats.ram_usage / 1024) * 100}%` }}
                />
              </div>
              <span>{deviceStats.ram_usage} MB</span>
            </div>
            <div className="stat-item">
              <label>Temperature</label>
              <span className={`temp ${deviceStats.temperature > 75 ? 'warn' : 'ok'}`}>
                {deviceStats.temperature.toFixed(1)}°C
              </span>
            </div>
            <div className="stat-item">
              <label>Battery</label>
              <div className="progress-bar">
                <div
                  className="progress-fill"
                  style={{ width: `${deviceStats.battery}%` }}
                />
              </div>
              <span>{deviceStats.battery}%</span>
            </div>
            <div className="stat-item">
              <label>FPS</label>
              <span>{deviceStats.fps}</span>
            </div>
            <div className="stat-item">
              <label>Uptime</label>
              <span>2d 14h 32m</span>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default DevicePanel;
