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
