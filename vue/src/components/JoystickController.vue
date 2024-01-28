<template>
  <div v-if="app.connect.value">
    <img class="q-pt-md stream-video" v-if="app.video.value" :src="getVideo.url">
  </div>
    <div class="fit row wrap justify-center q-mt-xl">
      <div v-for="(joystick, index) in joystickList" :key="index" 
        :ref="stick => addJoystick(stick, joystick.id, joystick.restLock, joystick.ctrl)"
        class="q-px-xl q-mt-xl column">
        <div>
            <q-badge color="primary" text-color="black" class="q-gutter-lg q-mt-xl">
                {{ joystick.label || 'joystick'}}
            </q-badge>
        </div>
      </div>
    </div>
</template>
  
<script setup>
import nipplejs from 'nipplejs'
import { controlServo } from './js/ServerController'
import { app, getJoySticks, getVideo } from './js/StoreController'


  const { joystickList } = getJoySticks()
  let jstTag = {}

      const sendJoystickValue = (id, ctrl, data) => {
          if(!jstTag[id]) {
            jstTag[id] = true
            let angle = (Math.round(data.angle.degree))
            let distance = Math.round(data.distance)
            if(distance === 0)
              distance = 1
            setTimeout(() => {
              jstTag[id] = false
              console.log(`stickId: ${id}, Angle: ${angle}, cos: ${Math.sin(angle)}, Distane: ${distance}, Ctrl: ${ctrl}`)
              controlServo(app.ip.value.ipaddress, app.ip.value.port, ctrl, angle, distance)
             // if(!status.value) {
              //  tryConnect()
             // }
            }, 15)
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
          size: 80,
          mode: "static",
          dynamicPage: true,
          //lockX: true
        }
        if(restlock === "true")
          options.restJoystick = false
        const joystick = nipplejs.create(options)
        
        joystick.on('move', (evt, data) => {
          sendJoystickValue(stickId, ctrlNum, data)
        })
      }

      const editSetting = () => {
        console.log('clicked')
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