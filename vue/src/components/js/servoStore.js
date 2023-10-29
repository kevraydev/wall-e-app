import { api } from 'boot/axios'
import { useSettingStore } from '../../stores/settingsStore'
import { computed } from 'vue'

export default function servoStore () {
    const servoStore = useSettingStore()
    const appIp = computed(() => servoStore.getSettingById(1))
    const options = computed(() => servoStore.getAllServos)

    //const options = computed(() => servo.value.map((item) => 
     //   ({id: item.id, label: item.label})))

    async function getServos() {
            await api.get(`http://${appIp.value.ipaddress}:${appIp.value.port}/options`)
            .then(res => {

                servoStore.addOptions(res.data)
                //console.log(res.data[0].id)

                  //  servoStore.addOptions(list)
                //let list = res.data.forEach((item) => {
                //    console.log(item.id)
               // })

                //const list = res.data.map(item => {
               //     return{
               //         id: item.id,
               //         label: item.label
               //     }
               // })
               // console.log(list)
               // console.log(res.data)
            })
            .catch(error => {
                console.error(error)
            })

        }

    return {
        options,
        getServos
    }
}