// @ts-check
import { useSettingStore } from '../../stores/settingsStore'
import { computed, reactive } from 'vue'

export default function getStore() {
    const settingStore = useSettingStore()
    const joysticks = computed(() => settingStore.getAllJoysticks)
    const servos = computed(() => settingStore.getAllServos)
    const appJoysticks = computed(() => joysticks.value.map(joystick => {
        const servo = servos.value.find((item) => item.id === joystick.ctrlId)
        
        return {
        ...joystick,
        label: (servo && servo.label) || null,
        ctrl: (servo && servo.id) || null,
        }
    }))
    const appSettings = computed(() => settingStore.getSettingById(1))
    const inputIP = reactive({
        url: appSettings.value.ipaddress,
        port: appSettings.value.port
    })
    return {
        appJoysticks,
        inputIP
    }
}