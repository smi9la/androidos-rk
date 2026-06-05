import React, { useState, useEffect } from 'react';
import './App.css';
import DevicePanel from './components/DevicePanel';
import LibraryPanel from './components/LibraryPanel';
import RemoteControl from './components/RemoteControl';
import Settings from './components/Settings';

const { ipcRenderer } = window.require('electron');

function App() {
  const [currentView, setCurrentView] = useState('devices');
  const [connectedDevice, setConnectedDevice] = useState(null);
  const [library, setLibrary] = useState([]);
  const [deviceStats, setDeviceStats] = useState(null);
  const [isLoading, setIsLoading] = useState(false);

  useEffect(() => {
    // Listen for device events
    const unsubscribe = ipcRenderer.on('device:event', (event, data) => {
      if (data.event === 'system_stats') {
        setDeviceStats(data.data);
      }
    });

    return unsubscribe;
  }, []);

  const handleDiscoverDevices = async () => {
    setIsLoading(true);
    try {
      const devices = await ipcRenderer.invoke('device:discover');
      console.log('Discovered devices:', devices);
    } catch (error) {
      console.error('Discovery failed:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const handleConnectDevice = async (ip) => {
    setIsLoading(true);
    try {
      const result = await ipcRenderer.invoke('device:connect', ip);
      if (result) {
        setConnectedDevice(ip);
        // Fetch initial device info
        const info = await ipcRenderer.invoke('device:getInfo');
        console.log('Device info:', info);
      }
    } catch (error) {
      console.error('Connection failed:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const handleScanLibrary = async (folderPath) => {
    setIsLoading(true);
    try {
      const games = await ipcRenderer.invoke('library:scan', folderPath);
      setLibrary(games);
    } catch (error) {
      console.error('Scan failed:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const handleLaunchGame = async (gameId) => {
    try {
      const result = await ipcRenderer.invoke('device:launchGame', gameId);
      console.log('Game launched:', result);
    } catch (error) {
      console.error('Launch failed:', error);
    }
  };

  const handleTransferRoms = async (files) => {
    setIsLoading(true);
    try {
      const result = await ipcRenderer.invoke('device:transferRoms', Array.from(files));
      console.log('Transfer complete:', result);
    } catch (error) {
      console.error('Transfer failed:', error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="app">
      <header className="app-header">
        <h1>AndriodOS-RK Manager</h1>
        <div className="header-status">
          {connectedDevice ? (
            <span className="connected">🟢 Connected: {connectedDevice}</span>
          ) : (
            <span className="disconnected">🔴 Disconnected</span>
          )}
        </div>
      </header>

      <nav className="app-nav">
        <button
          className={`nav-btn ${currentView === 'devices' ? 'active' : ''}`}
          onClick={() => setCurrentView('devices')}
        >
          Devices
        </button>
        <button
          className={`nav-btn ${currentView === 'library' ? 'active' : ''}`}
          onClick={() => setCurrentView('library')}
          disabled={!connectedDevice}
        >
          Library
        </button>
        <button
          className={`nav-btn ${currentView === 'remote' ? 'active' : ''}`}
          onClick={() => setCurrentView('remote')}
          disabled={!connectedDevice}
        >
          Remote Control
        </button>
        <button
          className={`nav-btn ${currentView === 'settings' ? 'active' : ''}`}
          onClick={() => setCurrentView('settings')}
          disabled={!connectedDevice}
        >
          Settings
        </button>
      </nav>

      <main className="app-main">
        {isLoading && <div className="loading-spinner">Loading...</div>}

        {currentView === 'devices' && (
          <DevicePanel
            onDiscover={handleDiscoverDevices}
            onConnect={handleConnectDevice}
            connectedDevice={connectedDevice}
            deviceStats={deviceStats}
          />
        )}

        {currentView === 'library' && (
          <LibraryPanel
            games={library}
            onScan={handleScanLibrary}
            onLaunch={handleLaunchGame}
            onTransfer={handleTransferRoms}
          />
        )}

        {currentView === 'remote' && (
          <RemoteControl device={connectedDevice} />
        )}

        {currentView === 'settings' && (
          <Settings device={connectedDevice} />
        )}
      </main>

      <footer className="app-footer">
        <p>AndriodOS-RK v1.0.0 | Built for RK3326</p>
      </footer>
    </div>
  );
}

export default App;
