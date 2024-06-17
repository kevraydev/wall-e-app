<template>
  <div v-if="app.connect.value">
    <img class="q-pt-md" v-if="app.video.value.show" :src="getVideo.url"
    :height="app.video.value.height" :width="app.video.value.width">
  </div>
  <div class="fit row wrap justify-center q-mt-xl">
    <div v-for="(joystick, index) in joystickList" :key="index"
      :ref="stick => addJoystick(stick, joystick.id, joystick.restLock, joystick.ctrl)" class="q-px-xl q-mt-xl column">
      <div>
        <q-badge color="primary" text-color="black" class="q-gutter-lg q-mt-xl">
          {{ joystick.label || 'joystick' }}
        </q-badge>
      </div>
    </div>
  </div>
</template>

<script setup>
import nipplejs from 'nipplejs'
import { controlServo } from './js/ServerController'
import { app, getJoySticks, getVideo } from './js/StoreController'
import { onMounted, onUnmounted } from 'vue'
import { tryConnect } from "../components/js/ServerController"

let jstTag = []
let timerId = null;

onMounted(() => {
  if(!app.connect.value){
    checkConnect()
  }
  timeJstClear()
  timerId = setInterval(sendData, 50)
})

onUnmounted(() => {
  timeJstClear()
})

const { joystickList } = getJoySticks()


const checkConnect = () => {
  tryConnect(app.ip.value.ipaddress, app.ip.value.port).then((con) => {
        app.setStatus(con)
  })
}

const timeJstClear = () => {
  if(app.connect.value){
  if(timerId != null)
    clearInterval(timerId)
  }
}

const sendJoystickValue = (id, ctrl, data) => {
    let angle = (Math.round(data.angle.degree))
    let distance = Math.round(data.distance)
    
    if (distance == 0)
      distance = 1

      jstTag[id] = {ctrl, angle, distance}
}

const sendData = () => {
  let status = false

  if(jstTag.length > 0 && app.connect.value){
    for(let jst in jstTag){
      status = controlServo(app.ip.value.ipaddress, app.ip.value.port, jstTag[jst].ctrl, jstTag[jst].angle, jstTag[jst].distance)
      console.log(jstTag[jst])
    }
    //if(!status)
    //  checkConnect()
  }
}

const removeJst = (id) => {
    if(jstTag[id]){
      jstTag.splice(id,1)
    }
    console.log(jstTag)
}

const addJoystick = (el, joystickId, restlock, ctrlNum) => {
  if (el) {
    initJoystick(el, joystickId, restlock, ctrlNum)
  }
}

const initJoystick = (el, stickId, restlock, ctrlNum) => {
  const options = {
    zone: el,
    color: "white",
    size: 115,
    mode: "static",
    dynamicPage: true,
    //lockX: true
  }
  if (restlock === "true")
    options.restJoystick = false
  const joystick = nipplejs.create(options)

  joystick.on('move', (evt, data) => {
    sendJoystickValue(stickId, ctrlNum, data)
  });
  joystick.on('end', (evt, data) => {
    removeJst(stickId)
  })
}

</script>
<style>
@media screen and (min-width: 928px) {
  .joystickDiv {
    width: 365px;
  }
}

@media screen and (min-width: 929px) {
  .joystickDiv {
    width: 700px;
  }
}

.stream-video {
  width: 500px;
  height: 400px;
}

.badge-item {
  margin-bottom: 3rem;
}
</style>