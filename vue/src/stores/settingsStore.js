// @ts-check
import { defineStore } from 'pinia'
import { LocalStorage } from 'quasar'



export const useSettingStore = defineStore('appSettings', {
  state: () => ({
      /** @type Settings[] */
      settings: LocalStorage.getItem('settings') ||
      [{id: 1, ipaddress: '192.168.0.1', port: '5000'}],
      /** @type Joystick[] */
      joysticks: LocalStorage.getItem('joysticks') || 
      [{id: 1, ctrlId: 0, restLock: false},
        {id: 2, ctrlId: 9, restLock: false}],
      /** @type Servo[] */
      servos: LocalStorage.getItem('servos') || 
      [{id: 1, label: ''}],
      connected: LocalStorage.getItem('connectStatus') === "true",
      video: LocalStorage.getItem('video') ||
      [{id: 1, show: false, width: 300, height: 300}],
  }),
  getters: {
    getConnection: (state) => {
      return state.connected
    },
    getVideoSettings: (state) => {
      return state.video.find((videos) => videos.id === 1)//state.video
    },
    getSettingById: (state) => (id) => {
      return state.settings.find((setting) => setting.id === id)
    },
    getJoystickById: (state) => (id) => {
      return state.joysticks.find((joystick) => joystick.id === id)
    },
    getServoById: (state) => (id) => {
      return state.servos.find((servo) => servo.id === id)
    },
    getAllSettings: (state) => {
      return state.settings
    },
    getServoLabels: (state) => {
      return state.servos.map(item => item.label)
    },
    getAllServos: (state) => {
      return state.servos
    },
    getAllJoysticks: (state) => {
      return state.joysticks
    },
    getLastJoystick: (state) => {
      if (state.joysticks.length > 0) {
        const lastJoystick = state.joysticks[state.joysticks.length - 1].id
        return lastJoystick
      }
      return -1
    },
    getLastServo: (state) => {
      if (state.joysticks.length > 0) {
        const lastServo = state.servos[state.servos.length - 1].id
        return lastServo
      }
      return -1
    }
  },
  actions: {
    addItem(propName, item) {
      const index = this[propName].findIndex((existingItem) => existingItem.id === item.id)
      if (index !== -1) {
        this[propName][index] = item
      }
      else {
        this[propName].push(item)
      }
      LocalStorage.set(`${propName}`, this[propName])
    },
    addOptions(newOptions){
      this.servos = newOptions
      LocalStorage.set('servos', newOptions)
    },
    removeItem(propName, id) {
      this[propName] = this[propName].filter((propField) => propField.id !== id)
      LocalStorage.set(`${propName}`, this[propName])
    },
    setConnection(status){
      this.connected = status
      LocalStorage.set('connectStatus', status.toString())
    },
  },
})