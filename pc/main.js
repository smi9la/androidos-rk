const { app, BrowserWindow, Menu, ipcMain } = require('electron');
const { autoUpdater } = require('electron-updater');
const path = require('path');
const isDev = require('electron-is-dev');

let mainWindow;

// Configure auto-updater
autoUpdater.checkForUpdatesAndNotify();

// Schedule update check for Monday 7:00 AM
function scheduleUpdateCheck() {
  const checkUpdate = () => {
    const now = new Date();
    const dayOfWeek = now.getDay();
    const hours = now.getHours();
    const minutes = now.getMinutes();
    
    // Monday = 1, 7:00 AM
    if (dayOfWeek === 1 && hours === 7 && minutes === 0) {
      console.log('🔔 Checking for updates (Monday 7:00 AM)');
      autoUpdater.checkForUpdates();
    }
  };
  
  // Check every minute
  setInterval(checkUpdate, 60000);
  checkUpdate(); // Check immediately
}

autoUpdater.on('update-available', () => {
  console.log('✅ Update available!');
  mainWindow.webContents.send('update-available');
});

autoUpdater.on('update-downloaded', () => {
  console.log('✓ Update downloaded');
  mainWindow.webContents.send('update-downloaded');
});

autoUpdater.on('error', (error) => {
  console.error('Update error:', error);
});

ipcMain.on('install-update', () => {
  autoUpdater.quitAndInstall();
});

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1200,
    height: 800,
    minWidth: 800,
    minHeight: 600,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, 'preload.js')
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

  // Schedule updates
  scheduleUpdateCheck();
}

app.on('ready', createWindow);

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  if (mainWindow === null) {
    createWindow();
  }
});

const template = [
  {
    label: 'File',
    submenu: [
      {
        label: 'Exit',
        accelerator: 'CmdOrCtrl+Q',
        click: () => {
          app.quit();
        }
      }
    ]
  },
  {
    label: 'Help',
    submenu: [
      {
        label: 'Check for Updates',
        click: () => {
          autoUpdater.checkForUpdates();
        }
      }
    ]
  }
];

const menu = Menu.buildFromTemplate(template);
Menu.setApplicationMenu(menu);
