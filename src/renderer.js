window.onload = function() {
  const terminalContainer = document.getElementById('terminal');
  const term = new Terminal();
  term.open(terminalContainer);

  const electronAPI = window.electronAPI;

  term.onData(function(data) {
    electronAPI.sendTerminalInput(data);
  })

  electronAPI.onTerminalData(function(data) {
    term.write(data);
  });
};