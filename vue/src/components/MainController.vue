<template>
  <div v-if="app.connect.value">
    <img class="q-pt-md" v-if="app.video.value.show" :src="getVideo.url"
    :height="app.video.value.height" :width="app.video.value.width">
  </div>
  <div class="fit row wrap justify-center q-mt-xl">
    <div v-for="(joystick) in joystickList" :key="joystick.id"
      :ref="stick => addJoystick(stick, joystick.id, joystick.restLock, joystick.ctrl)" class="q-px-xl q-mt-xl column">
      <div>
        <q-badge color="primary" text-color="black" class="q-gutter-lg badge-item">
          {{ joystick.label || 'joystick' }}
        </q-badge>
      </div>
    </div>
  </div>
  <div class="column wrap justify-center q-mt-xl">
    <q-select filled v-model="sounds" :options="soundOptions" label="Play a sound" 
      @input="onSelect"/>
  </div>
</template>

<script setup>
import nipplejs from 'nipplejs'
import { controlServo } from './js/ServerController'
import { app, getJoySticks, getVideo } from './js/StoreController'
import { ref, onMounted, onUnmounted } from 'vue'
import { tryConnect } from "../components/js/ServerController"

let lastJst = {}
//const playsound = ref(null)
const sounds = ref('')
const soundOptions = ref([
  {label: 'Wall-E', value: 1},
  {label: 'Whoa!', value: 2},
  {label: 'Boot Up', value: 3},
  {label: 'Grunt', value: 4},
  {label: 'Ta-da!', value: 5},
  {label: 'Music', value: 6},
])

onMounted(() => {
  if(!app.connect.value){
    checkConnect()
  }

})

const { joystickList } = getJoySticks()


const checkConnect = () => {
  tryConnect(app.ip.value.ipaddress, app.ip.value.port).then((con) => {
        app.setStatus(con)
  })
}

const sendJoystickValue = (id, ctrl, data) => {
  let status = false
    let angle = (Math.round(data.angle.degree))
    let distance = Math.round(data.distance)
    //console.log(angle);
    if (distance == 0)
      distance = 1
    const currentTime = Date.now()
    if (!lastJst[id] || (currentTime - lastJst[id] >= 15)) {

      status = controlServo(app.ip.value.ipaddress, app.ip.value.port, ctrl, angle, distance)
      lastJst[id] = currentTime
    }
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
    size: 105,
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

}

const onSelect = (selectedValue) => {
  
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
  margin-top: 3rem;
}
</style>