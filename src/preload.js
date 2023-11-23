const {ipcRenderer, contextBridge} = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
  onTerminalData: (callback) => {
    ipcRenderer.on('terminal-data', (event, data) => {
      callback(data);
    });
  },
  sendTerminalInput: (input) => {
    ipcRenderer.send('terminal-input', input);
  }

});
