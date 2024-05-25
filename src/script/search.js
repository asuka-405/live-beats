const searchInput = document.querySelector("#search-yt")
const searchButton = document.querySelector("#search-button")

const search = window.liveBeats.ytSearch

searchButton.addEventListener("click", async () => {
  const playlist = document.querySelector(".playlist")
  playlist.innerHTML = ""
  const query = searchInput.value
  const results = JSON.parse(await search(query))
  const playlistItems = []
  results.forEach((video) => {
    playlistItems.push(
      createPlaylistItem(video.title, video.duration.timestamp, video.url)
    )
  })
  playlist.append(...playlistItems)
})

function createPlaylistItem(name, duration, url) {
  const item = document.createElement("div")
  item.classList.add("item")
  const nameElement = document.createElement("div")
  nameElement.classList.add("name")
  nameElement.textContent = name
  const durationElement = document.createElement("div")
  durationElement.classList.add("duration")
  durationElement.textContent = duration
  const buttons = document.createElement("div")
  buttons.classList.add("buttons")
  const playButton = document.createElement("span")
  playButton.classList.add("play-music")
  const playButtonImg = document.createElement("img")
  playButtonImg.src = "images/play.svg"
  playButtonImg.alt = ""
  playButton.appendChild(playButtonImg)

  playButton.addEventListener("click", () => {
    document.querySelector("mu-player").playNow(name, duration, url)
  })

  buttons.appendChild(playButton)
  item.appendChild(nameElement)
  item.appendChild(durationElement)
  item.appendChild(buttons)
  return item
}

document
  .querySelector(".exit-btn")
  .addEventListener("click", window.liveBeats.quitApp)
