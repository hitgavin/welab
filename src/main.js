const {app, ipcMain, BrowserWindow} = require('electron');
const path = require('path');
const pty = require('node-pty');

function createWindow() {
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      nodeIntegration: true,
      preload: path.join(__dirname, 'preload.js'),
    },
  });

  mainWindow.loadFile('index.html');

  try {
    const shellProcess = pty.spawn('zsh', [], {
      name: 'xterm-color',
      cols: 80,
      rows: 80,
      cwd: process.env.HOME,
      env: process.env,
    });

    shellProcess.onData(data => {
      mainWindow.webContents.send('terminal-data', data);
    });

    ipcMain.on('terminal-input', (event, data) => {
      shellProcess.write(data);
    });
  } catch (error) {
    console.error('Error creating shell process:', error);
  }
}

app.whenReady().then(() => {
  createWindow();

  app.on('activate', function() {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', function() {
  if (process.platform !== 'darwin') app.quit();
});
