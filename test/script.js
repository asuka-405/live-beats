window.onload = function () {
  document.getElementById("midi").addEventListener("click", downloadMidi)
}

function downloadMidi() {
  var abc = document.getElementById("abc").value
  var midi = ABCJS.synth.getMidiFile(abc, { midiOutputType: "encoded" })
  console.log(midi)
}
