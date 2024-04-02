<template>
    <q-btn color="primary" text-color="black"
    label="Joysticks"
    @click="showJoystickDia = true" />
    <q-dialog v-model="showJoystickDia" persistent>
        <q-card style="width: 70%; height: 70%">
        <q-splitter
      v-model="splitterModel">

      <template v-slot:before>
        <q-tabs
          v-model="tab"
          vertical
          class="text-primary">
          <q-tab name="joystick_tab" icon="videogame_asset" label="Joystick" />
          <q-tab name="joystick_settings" icon="settings" label="Settings" />
        </q-tabs>
      </template>

      <template v-slot:after>
        <q-tab-panels
          v-model="tab"
          animated
          swipeable
          vertical
          transition-prev="jump-up"
          transition-next="jump-up"
        >
          <q-tab-panel name="joystick_tab">
            <div class="text-h4 q-mb-md">Joystick List</div>
            <q-list>
                <q-item-label caption class="q-pl-sm">Joysticks</q-item-label>
                            <q-item v-for="item in joystickList" :key="item.id" class="settingList" clickable v-ripple>
                                <q-item-section>
                                    <div class="full-width row inline wrap justify-between">
                                        <p class="listItem">{{ item.id }}</p>
                                        <p class="listItem">{{ item.label || "No Label" }}</p>
                                        <q-expansion-item :content-inset-level="0.5" expand-separator>
                                            <q-badge color="primary" text-color="black" class="subListItem">Rest Lock:
                                                {{ item.restLock }}</q-badge>
                                            <q-badge color="primary" text-color="black" class="subListItem">Ctrl Num: {{
                item.ctrl }}</q-badge>
                                        </q-expansion-item>
                                        <q-btn flat icon="cancel" @click="removeItem(item.id)" class="q-pl-sm"
                                            size="1.2rem" color="primary" />
                                    </div>
                                </q-item-section>
                            </q-item>
            </q-list>
          </q-tab-panel>

          <q-tab-panel name="joystick_settings">
            <div class="text-h4 q-mb-md">Joystick Settings</div>
            <div v-if="options[0].label">
            <q-card-section>
              
                <q-select filled v-model="servo" :options="options" />
              
            </q-card-section>
            <q-card-section>
                <div class="q-pa-sm">Rest Lock?</div>
                <q-radio v-model="lock" val="true" label="Yes"/>
                <q-radio v-model="lock" val="false" label="No"/>
            </q-card-section>
            <q-card-section class="fit row wrap justify-center">
                <q-btn icon="check" label="Add Joystick" @click="addJoystick" class="q-pl-sm" size="0.8rem" color="primary" text-color="black"/>
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
import { showNote } from './js/NotifyController'
import { app, getJoySticks } from './js/StoreController'

    const showJoystickDia = ref(false);
    const tab = ref('joystick_tab');
      //const splitterModel = ref(20);
    const servo = ref(null);
    const lock = ref('false');

    const { joystickList } = getJoySticks();

    const options = app.options
   // if(options.value[0].label == null)
    //  options.value[0].label = 'No controls'
    //if(options.value[0].label == '')

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

    const removeItem = (id) => {
    app.removeItem("joysticks", id);
    showNote("Removed", "blue-grey-10");
};

</script>

<style>

</style>