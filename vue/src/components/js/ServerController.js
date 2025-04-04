// @ts-check
import { api } from '../../boot/axios'
import { ref } from 'vue'
import { showNote } from './NotifyController'

/**
 * @param {string} url
 * @param {string} port
 * @param {Servo} servo
 * @param {number} angle
 * @param {number} distance
 */
export function controlServo(url, port, servo, angle, distance){
    const status = ref(false)
    console.log(url, port, servo, angle, distance)
    api.post(`http://${url}:${port}/control`, {
      angle: angle,
      servo: servo,
      distance: distance
      })
      .then(res => {
        status.value = true
      })
      .catch(error => {
        console.error(error)
      })
  return status.value
}


/**
 * @param {string} url
 * @param {string} port
 * @param {string} command
 */

export function sendCommand(url, port, command){
  const status = ref(false)
  api.post(`http://${url}:${port}/cmd`, {
    command: command,
    })
    .then(res => {
      status.value = true
    })
    .catch(error => {
      console.error(error)
    })
return status.value
}


/**
 * @param {string} url
 * @param {string} port 
 */
export async function tryConnect(url, port){

  showNote('Connecting..', 'blue-grey-10', true, true, 2500)
  function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms))
  }
  await delay(1500)

  try{
    await api.get(`http://${url}:${port}/check`)
    showNote('Connected', 'positive', false, true)
    return true
  }
  catch(error){
    showNote('Disconnected, check config', 'negative', false, true)
    return false
  }
  
}
  

