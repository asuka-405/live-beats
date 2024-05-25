const { contextBridge, ipcRenderer } = require("electron")

contextBridge.exposeInMainWorld("liveBeats", {
  getMusicList: () => ipcRenderer.invoke("get-music-list"),
  getMusic: () => ipcRenderer.invoke("get-music"),
  ytSearch: async (query) => ipcRenderer.invoke("yt-search", query),
  ytStream: async (url) => ipcRenderer.invoke("yt-stream", url),
  aiGen: async () => ipcRenderer.invoke("ai-gen"),
  showInDir: async (path) => ipcRenderer.invoke("show-in-dir", path),
  quitApp: async () => ipcRenderer.invoke("quit"),
})
