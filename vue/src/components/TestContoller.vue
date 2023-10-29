<template>
    <q-btn color="primary" text-color="black"
            label="Test Servo" @click="showTestController = true" />
        <q-dialog v-model="showTestController" persistent class="q-pt-xl">
        <q-card style="width: 500px; height: 300px;">
            <div id="listHeader" class="listItem">Test Servo</div>
                <q-separator />
                <div class="row justify-center">
                    <q-card-section>
                        <q-select filled v-model="selected" :options="options" />
                    </q-card-section>
                    <q-card-section>
                        <q-input type="number" label="Angle" v-model="angle" class="inputSize"/>
                    </q-card-section>
                <q-card-section class="fit row wrap justify-center">
                    <div class="q-px-sm"><q-btn color="primary" text-color="black"
                    label="Run Test"
                    @click="moveServo" />
                    </div>
                <div class="q-px-sm">
                    <q-btn flat round icon="close" v-close-popup />
                </div>
                </q-card-section>
                </div>
        </q-card>
        </q-dialog>
</template>

<script setup>
import { ref } from 'vue'
import { controlServo } from './js/ServerController'
import getServos from './js/servoStore'

    const props = defineProps(['url', 'port'])
    const showTestController = ref(false)
    const angle = ref(0)

    const selected = ref(null)
    const { options } = getServos()
    selected.value = options.value[0].label
    const moveServo = () => {
            
    let ctrlNum = 0
    if(selected.value.id === undefined)
        ctrlNum = options.value[0].id
    else
        ctrlNum = selected.value.id
    controlServo(props.url, props.port, ctrlNum, angle.value, 0)
        
    }

</script>
<style>
.inputSize {
    width: 100px;
}
</style>