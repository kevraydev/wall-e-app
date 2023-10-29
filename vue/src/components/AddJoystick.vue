<template>
    <q-btn color="primary" text-color="black"
    label="Add Joystick"
    @click="showJoystickDia = true" />
    <q-dialog v-model="showJoystickDia" persistent>
    <q-card style="width: 500px; height: 300px;">
        <div id="listHeader" class="listItem">Add Joystick</div>
        <q-separator />
        <div class="row justify-center">
            <q-card-section>
                <q-select filled v-model="servo" :options="options" />
            </q-card-section>
            <q-card-section>
                <div class="q-pa-sm">Rest Lock?</div>
                <q-radio v-model="lock" val="true" label="Yes"/>
                <q-radio v-model="lock" val="false" label="No"/>
            </q-card-section>
            <q-card-section class="fit row wrap justify-center">
                <q-btn flat icon="check" @click="addJoystick" class="q-pl-sm" size="1.3rem" color="positive"/>
                <q-btn flat icon="close" v-close-popup />
            </q-card-section>
        </div>
    </q-card>
    </q-dialog>
</template>

<script setup>
import { ref } from 'vue'
import { showNote } from './js/NotifyController'
import { app } from './js/StoreController'

    const showJoystickDia = ref(false)

    const servo = ref(null)
    const lock = ref('false')

    const options = app.options
    
    servo.value = options.value[0].label
    
    const addJoystick = () => {
    const newId = app.lastJoystickId.value + 1
    let servoInit = 0
    if(servo.value.id === undefined)
        servoInit = options.value[0].id
    else
        servoInit = servo.value.id
    app.addJoystick(newId, servoInit, lock.value)
        showNote('Joystick Added!', 'positive')
    }

</script>

<style>

</style>