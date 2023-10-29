import { api } from 'boot/axios'
import { ref } from 'vue'
import { showNote } from './NotifyController'

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

export async function tryConnect(url, port){

  showNote('Connecting..', 'blue-grey-10', true, true, 2500)
  function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms))
  }
  await delay(1500)

  try{
    await api.get(`http://${url}:${port}/check`)
    showNote('Connected', 'positive', false, true)
    return 'true'
  }
  catch(error){
    showNote('Disconnected', 'negative', false, true)
    return 'false'
  }
  
}
  

