/*
 * ANDROIDOS-RK Desktop Application
 * Cross-Platform (Windows/macOS/Linux)
 * Framework: Electron with React
 */

// main.js - Electron main process
const { app, BrowserWindow, Menu, ipcMain } = require('electron');
const path = require('path');
const isDev = require('electron-is-dev');

let mainWindow;
let connectedDevice = null;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1400,
    height: 900,
    minWidth: 1000,
    minHeight: 700,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      enableRemoteModule: false,
      nodeIntegration: false
    }
  });

  const startUrl = isDev
    ? 'http://localhost:3000'
    : `file://${path.join(__dirname, '../build/index.html')}`;

  mainWindow.loadURL(startUrl);

  if (isDev) {
    mainWindow.webContents.openDevTools();
  }

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
}

app.on('ready', createWindow);
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});
app.on('activate', () => {
  if (mainWindow === null) createWindow();
});

// IPC Handlers for device communication
ipcMain.handle('device:discover', async () => {
  // Discover R36S devices on network
  return discoverDevices();
});

ipcMain.handle('device:connect', async (event, ip) => {
  return connectToDevice(ip);
});

ipcMain.handle('device:disconnect', async () => {
  if (connectedDevice) {
    await connectedDevice.disconnect();
    connectedDevice = null;
  }
});

ipcMain.handle('device:getInfo', async () => {
  if (connectedDevice) {
    return connectedDevice.getSystemInfo();
  }
  return null;
});

ipcMain.handle('device:getStats', async () => {
  if (connectedDevice) {
    return connectedDevice.getStats();
  }
  return null;
});

ipcMain.handle('device:launchGame', async (event, gameId) => {
  if (connectedDevice) {
    return connectedDevice.launchGame(gameId);
  }
});

ipcMain.handle('device:transferRoms', async (event, files) => {
  if (connectedDevice) {
    return connectedDevice.transferRoms(files);
  }
});

ipcMain.handle('library:scan', async (event, path) => {
  return scanROMLibrary(path);
});

ipcMain.handle('library:export', async (event, format) => {
  return exportLibrary(format);
});

// Device discovery and connection
async function discoverDevices() {
  const { networkInterfaces } = require('os');
  const dgram = require('dgram');
  const devices = [];

  return new Promise((resolve) => {
    const client = dgram.createSocket('udp4');
    client.on('message', (msg, rinfo) => {
      try {
        const device = JSON.parse(msg.toString());
        devices.push({ ...device, ip: rinfo.address });
      } catch (e) {}
    });

    client.bind(5555, () => {
      client.setBroadcast(true);
      const broadcast = Buffer.from(JSON.stringify({
        action: 'discover_request',
        version: 1
      }));
      
      // Send broadcast on all interfaces
      Object.values(networkInterfaces()).forEach(iface => {
        iface.forEach(addr => {
          if (addr.family === 'IPv4' && !addr.internal) {
            const parts = addr.address.split('.');
            const broadcastAddr = parts.slice(0, 3).join('.') + '.255';
            client.send(broadcast, 0, broadcast.length, 5555, broadcastAddr);
          }
        });
      });
    });

    setTimeout(() => {
      client.close();
      resolve(devices);
    }, 2000);
  });
}

class DeviceConnection {
  constructor(ip) {
    this.ip = ip;
    this.baseUrl = `http://${ip}:8888/api/v1`;
    this.ws = null;
    this.connected = false;
  }

  async connect() {
    try {
      // Get device info to verify connection
      const response = await fetch(`${this.baseUrl}/system/info`);
      if (response.ok) {
        this.connected = true;
        this.connectWebSocket();
        return true;
      }
      return false;
    } catch (e) {
      console.error('Connection failed:', e);
      return false;
    }
  }

  connectWebSocket() {
    this.ws = new (require('ws'))(`ws://${this.ip}:8888/ws`);
    this.ws.on('message', (data) => {
      mainWindow.webContents.send('device:event', JSON.parse(data));
    });
  }

  async disconnect() {
    if (this.ws) this.ws.close();
    this.connected = false;
  }

  async getSystemInfo() {
    const response = await fetch(`${this.baseUrl}/system/info`);
    return response.json();
  }

  async getStats() {
    const response = await fetch(`${this.baseUrl}/system/stats`);
    return response.json();
  }

  async launchGame(gameId) {
    const response = await fetch(`${this.baseUrl}/games/launch/${gameId}`, {
      method: 'POST'
    });
    return response.json();
  }

  async transferRoms(files) {
    // Upload ROMs to device
    const formData = new FormData();
    files.forEach(file => formData.append('files', file));
    
    const response = await fetch(`${this.baseUrl}/files/upload`, {
      method: 'POST',
      body: formData
    });
    return response.json();
  }

  async sendCommand(cmd, args = {}) {
    const response = await fetch(`${this.baseUrl}/system/command`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ command: cmd, args })
    });
    return response.json();
  }
}

async function connectToDevice(ip) {
  const device = new DeviceConnection(ip);
  const connected = await device.connect();
  if (connected) {
    connectedDevice = device;
    return true;
  }
  return false;
}

function scanROMLibrary(folderPath) {
  const fs = require('fs');
  const path = require('path');
  const games = [];

  const romExtensions = ['.iso', '.cso', '.nes', '.smc', '.bin', '.z64', '.n64', '.wbfs', '.gcz'];

  function scanDirectory(dir) {
    try {
      const files = fs.readdirSync(dir);
      files.forEach(file => {
        const fullPath = path.join(dir, file);
        const stat = fs.statSync(fullPath);

        if (stat.isDirectory()) {
          scanDirectory(fullPath);
        } else if (romExtensions.some(ext => file.toLowerCase().endsWith(ext))) {
          games.push({
            id: path.basename(file, path.extname(file)),
            title: path.basename(file, path.extname(file)),
            path: fullPath,
            size: stat.size,
            modified: stat.mtime
          });
        }
      });
    } catch (e) {
      console.error('Error scanning directory:', e);
    }
  }

  scanDirectory(folderPath);
  return games;
}

function exportLibrary(format) {
  // Export library as CSV or JSON
  // Implementation depends on loaded library
  return { status: 'ok', message: 'Library exported' };
}

// Create application menu
const template = [
  {
    label: 'File',
    submenu: [
      { label: 'Exit', accelerator: 'CmdOrCtrl+Q', click: () => app.quit() }
    ]
  },
  {
    label: 'Edit',
    submenu: [
      { role: 'undo' },
      { role: 'redo' },
      { type: 'separator' },
      { role: 'cut' },
      { role: 'copy' },
      { role: 'paste' }
    ]
  },
  {
    label: 'Help',
    submenu: [
      { label: 'About', click: () => {
        const { dialog } = require('electron');
        dialog.showMessageBox(mainWindow, {
          type: 'info',
          title: 'About AndriodOS-RK',
          message: 'AndriodOS-RK Desktop Manager v1.0.0',
          detail: 'Manage your R36S handheld gaming device'
        });
      }}
    ]
  }
];

const menu = Menu.buildFromTemplate(template);
Menu.setApplicationMenu(menu);
