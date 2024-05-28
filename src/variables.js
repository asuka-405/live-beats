import fs from "fs"
import os from "os"
import path from "path"
import { fileURLToPath } from "url"

export const __dirname = path.dirname(fileURLToPath(import.meta.url))
const configFile = path.join(os.homedir(), "live-beats", "config.json")
const configDir = path.join(os.homedir(), "live-beats")

export const audioExtName = getAudioExtNameList()
export const config = getConfig()

function getConfig() {
  if (!fs.existsSync(configDir)) fs.mkdirSync(configDir)
  if (!fs.existsSync(configFile))
    return {
      music: [],
    }
  return JSON.parse(fs.readFileSync(configFile, "utf-8"))
}

export function saveConfig(config) {
  fs.writeFileSync(configFile, JSON.stringify(config))
}

function getAudioExtNameList() {
  return [
    ".mp3",
    ".wav",
    ".flac",
    ".aac",
    ".ogg",
    ".m4a",
    ".wma",
    ".aiff",
    ".ape",
    ".alac",
  ]
}

export function getPlayerJS(abcNotation) {
  return `
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
}
