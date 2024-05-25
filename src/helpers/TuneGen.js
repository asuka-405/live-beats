import { exec } from "child_process"
import Result from "../helpers/Result.js"

function getABC() {
  return new Promise((resolve, reject) => {
    exec("docker run fyp", (error, stdout, stderr) => {
      if (error) {
        resolve(Result.err(error))
      } else {
        resolve(Result.ok(stdout))
      }
    })
  })
}

async function getMIDI() {
  const result = await getABC()
  if (result.isErr()) {
    return result
  }

  const abc = result.unwrap()
  const sakura = new SakuraCompiler()
  const midi = sakura.compile(abc)
  console.log(midi)
}

await getMIDI()

// async function getWAV(midiBuffer){
//   return new Promise((resolve, reject)=>{
//     const audioContext = new AudioContext()
//     audioContext.decodeAudioData(midiBuffer,buffer=>{

//     })
//   })
// }
