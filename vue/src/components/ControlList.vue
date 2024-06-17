<template>
    <q-btn color="primary" text-color="black"
            label="Controls" @click="showTestController = true" />
            <q-dialog v-model="showTestController" persistent>
        <q-card style="width: 70%; height: 70%">
        <q-splitter
      v-model="splitterModel"
      
    >

      <template v-slot:before>
        <q-tabs
          v-model="servotab"
          vertical
          class="text-primary"
        >
          <q-tab name="servos_tab" icon="list" label="Controls" />
          <q-tab name="test_servo" icon="build" label="Test" />
        </q-tabs>
      </template>

      <template v-slot:after>
        <q-tab-panels
          v-model="servotab"
          animated
          swipeable
          vertical
          transition-prev="jump-up"
          transition-next="jump-up"
        >
          <q-tab-panel name="servos_tab">
            <div class="text-h4 q-mb-md">Control List</div>
            <q-card-section style="max-height: 20vh" class="scroll servoList">
                <q-list dense bordered padding class="rounded-borders">
                    <q-item v-for="(item, index) in app.options.value" :key="index" class="settingList">
                        <q-item-section class="q-px-lg">
                            {{ item.id }}
                        </q-item-section>
                        <q-item-section class="q-px-lg">
                            {{ item.label }}
                        </q-item-section>
                    </q-item>
                </q-list>
            </q-card-section>

            <q-card-section class="fit row wrap justify-center">
                <q-btn @click="fetchOptions" color="primary" text-color="black" label="Fetch Options"/>
                <q-btn flat icon="close" v-close-popup />
            </q-card-section>
          </q-tab-panel>

          <q-tab-panel name="test_servo">
            <div class="text-h4 q-mb-md">Test Control</div>
            <div v-if="options[0].label">
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
                    </q-card-section>
                    </div>
                    <div v-else>
                <p>No controls available, you'll need to fetch control options from server frst.</p>
              </div>
          </q-tab-panel>

        </q-tab-panels>

      </template>

    </q-splitter>
    <q-card-section>
                <q-btn flat icon="close" v-close-popup />
            </q-card-section>
        </q-card>
    </q-dialog>
</template>

<script setup>
import { ref } from 'vue'
import { controlServo } from './js/ServerController'
import getServos from './js/servoStore'
import { fetchOptions, app } from './js/StoreController'

const servotab = ref('servos_tab');
      //const splitterModel = ref(20);
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

.servoList {
    background-color: rgb(45, 45, 45);
}
</style>