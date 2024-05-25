import { exec } from "child_process"
import { BrowserWindow, app, dialog, ipcMain, shell } from "electron"
import startup from "electron-squirrel-startup"
import fs from "fs"
import musicMetadata from "music-metadata"
import os from "os"
import path from "path"
import yts from "yt-search"
import ytdl from "ytdl-core"
import { __dirname, audioExtName, config, saveConfig } from "./variables.js"
if (startup) app.quit()

let win = null
const homedir = os.homedir()
if (!fs.existsSync(homedir + "/livebeats"))
  fs.mkdirSync(path.join(homedir, "livebeats"), { recursive: true })

ipcMain.handle("show-in-dir", async (event, item) => {
  shell.showItemInFolder(item)
})

ipcMain.handle("ai-gen", async () => {
  const abcNotation = await new Promise((resolve, reject) => {
    exec("docker run fyp", (err, stdout, stderr) => {
      if (err) {
        reject(err)
        return
      }
      if (stderr) {
      }
      resolve(stdout)
    })
  }).catch((err) => console.error(err))
  win.loadURL("https://notabc.app/abc-converter/")
  win.webContents.on("did-finish-load", async () => {
    await win.webContents
      .executeJavaScript(
        `
// Hide navigation and other elements
document.querySelector('nav').style.display = 'none';
document.querySelectorAll(".col-md-3.offset-md-1.col-sm-12.mb-4, h1, p").forEach(e => e.style.display = 'none');

// Set ABC notation in textarea
const textarea = document.querySelector('#abc');
const warnings = document.querySelector("#warnings");
warnings.style.display = 'none';
textarea.innerText = \`${abcNotation}\`;

// Create style element for CSS
const style = document.createElement('style');
style.textContent = \`
  body {
    background: linear-gradient(#ffedfa, #c9a6ec);
    display: flex;
    justify-content: center;
    align-items: center;
    margin-top: 3em;
  }

  .title-bar {
    display: flex;
    justify-content: space-between;
    align-items: center;
    width: 100%;
    height: 50px;
    padding: 0 1em;
    background: #aa6bea;
    color: #28282b;
    font-size: 1.5rem;
    font-weight: bold;
    position: fixed;
    top: 0;
    z-index: 999999999;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
    user-select: none;
    -webkit-app-region: drag;
  }

  .title-bar-btn {
    -webkit-app-region: no-drag;
    background: #c9a6ec;
    color: #28282b;
    border: none;
    padding: 0.5em 1em;
    border-radius: 5px;
    cursor: pointer;
    font-family: monospace;
  }

  .title-bar-btn:hover {
    background: #ffedfa;
    color: #28282b;
  }
\`;

// Append style element to the head
document.head.appendChild(style);

// Create and append title bar elements
const titleBar = document.createElement('div');
titleBar.classList.add('title-bar');

const title = document.createElement('span');
title.classList.add('title');
title.textContent = 'Live Beats';

const subtitle = document.createElement('span');
subtitle.classList.add('subtitle');
subtitle.textContent = 'AI Music Generator';

const exitBtn = document.createElement('span');
exitBtn.classList.add('title-bar-btn', 'exit-btn');
exitBtn.textContent = 'x';

titleBar.appendChild(title);
titleBar.appendChild(subtitle);
titleBar.appendChild(exitBtn);

document.body.insertBefore(titleBar, document.body.firstChild);
`
      )
      .catch((err) => console.error(err))
  })
})

ipcMain.handle("get-music-list", async () => {
  return config.music
})

ipcMain.handle("get-music", async () => {
  return config
})

ipcMain.handle("yt-search", async (e, query) => {
  const searchres = (await yts(query)).videos
  const str = JSON.stringify(searchres)
  return str
})

ipcMain.handle("yt-stream", async (event, url) => {
  try {
    const stream = ytdl(url, { filter: "audioonly" })
    const buffers = await streamToBuffer(stream)
    const buffer = Buffer.concat(buffers) // Concatenate the array of buffers
    return "data:audio/mp3;base64," + buffer.toString("base64")
  } catch (err) {
    return null
  }
})

function streamToBuffer(stream) {
  const chunks = []
  return new Promise((resolve, reject) => {
    stream.on("data", (chunk) => {
      chunks.push(chunk)
    })
    stream.on("end", () => {
      resolve(chunks)
    })
    stream.on("error", (err) => {
      reject(err)
    })
  })
}

const createWindow = async () => {
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    frame: false,
    webPreferences: {
      preload: path.join(__dirname, "preload.js"),
      contextIsolation: true,
    },
  })
  mainWindow.loadFile(path.join(__dirname, "ai.html"))

  if (!config.music || !config.meta) {
    config.music = await saveMusicList(config)
    config.meta = await getMusicMetadata(config.music)
    saveConfig(config)
    mainWindow.reload()
  }
  win = mainWindow
}

ipcMain.handle("quit", () => {
  app.quit()
})

app.on("ready", createWindow)
app.on("window-all-closed", () =>
  process.platform !== "darwin" ? app.quit() : ""
)

app.on("activate", () =>
  BrowserWindow.getAllWindows().length === 0 ? createWindow() : ""
)

function indexDir(dir) {
  const files = fs.readdirSync(dir)
  const musicList = []
  files.forEach((file) => {
    const filePath = path.join(dir, file)
    const stat = fs.statSync(filePath)
    if (stat.isDirectory()) musicList.push(...indexDir(filePath))
    else if (audioExtName.includes(path.extname(file))) {
      musicList.push(filePath)
    }
  })
  return musicList
}

async function saveMusicList(config) {
  const music = []
  const res = await dialog.showOpenDialog({
    properties: ["openFile", "multiSelections", "openDirectory"],
  })
  if (res.canceled) return
  music.push(...indexDir(res.filePaths[0]))
  return music
}

/**
 * @object metadata.common
 * Represents metadata for a music track.
 * @typedef {Object} TrackMetadata
 * @property {Object} track - The track information.
 * @property {number} track.no - The track number.
 * @property {number} track.of - The total number of tracks.
 * @property {Object} disk - The disk information.
 * @property {number|null} disk.no - The disk number.
 * @property {number|null} disk.of - The total number of disks.
 * @property {Object} movementIndex - The movement index information.
 * @property {string} title - The title of the track.
 * @property {string[]} artists - The list of artists for the track.
 * @property {string} artist - The primary artist for the track.
 * @property {string} album - The album name.
 * @property {number} year - The year of release.
 * @property {string[]} genre - The genre(s) of the track.
 * @property {string[]} comment - Additional comments about the track.
 * @property {string} copyright - The copyright information.
 * @property {string} originalartist - The original artist of the track.
 * @property {string} encodersettings - The encoding settings used for the track.
 * @property {string} website - The website associated with the track.
 * @property {Object[]} picture - The picture(s) associated with the track.
 * @property {string} picture.format - The format of the picture.
 * @property {string} picture.type - The type of the picture.
 * @property {string} picture.description - The description of the picture.
 * @property {Object} picture.data - The data of the picture.
 * @property {string} picture.data.type - The type of the picture data.
 * @property {number[]} picture.data.data - The actual picture data.
 */

async function getMusicMetadata(musicList) {
  const metadata = []
  for (const music of musicList) {
    metadata.push((await musicMetadata.parseFile(music)).common)
  }
  await fs.writeFileSync("./metadata.json", JSON.stringify(metadata))
  return metadata
}
