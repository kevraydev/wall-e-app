<template>
    <div class="column items-center q-pt-lg">
        
        <q-list bordered>
            <q-expansion-item group="settingsGroup" icon="settings" label="Connection" default-opened
                header-class="text-primary listItem" class="expList">
                <q-card>
                    <div class="column items-center q-pa-md">
                        <q-list dense bordered padding class="rounded-borders" style="width: 100%">
                            <q-item-label caption class="q-pl-sm">IP Address</q-item-label>
                            <q-item class="settingList" clickable v-ripple>
                                <q-item-section>
                                    <div class="full-width row inline wrap justify-between">
                                        <p class="listItem">{{ app.ip.value.ipaddress }}</p>
                                        <p class="listItem">{{ app.ip.value.port }}</p>
                                        <q-btn flat :icon="app.connect.value ? 'refresh' : 'sync_problem'"
                                            @click="showUpdate = true" class="q-pl-sm" size="1.2rem"
                                            :color="app.connect.value ? 'primary' : 'red'" />
                                    </div>
                                </q-item-section>
                            </q-item>
                        </q-list>
                    </div>
                </q-card>
            </q-expansion-item>

            <q-separator />

            <q-expansion-item v-model="showUpdate" group="settingsGroup" icon="add_circle" label="Update Settings"
                header-class="text-primary listItem" class="width:expList">
                <q-card>
                    <div class="row justify-center">
                        <q-input v-model="app.ip.value.ipaddress" type="text" hint="Enter IP Address" lazy-rules :rules="[
                (val) => (val && val.length > 0) || 'Please type something!',
            ]" mask="###.###.#.###" class="q-pr-md" />
                        <q-input v-model="app.ip.value.port" type="text" class="q-pl-md shortInput" hint="Port"
                            maxlength="4" />
                        <q-btn flat icon="check" @click="updateIP" class="q-pl-sm" size="1.3rem" color="positive" />
                    </div>
                    <q-separator class="q-mt-sm" />
                    
                    <div class="row items-center justify-center q-pb-sm q-pt-lg">
            <div class="q-pr-sm">Show Video?</div>
            <q-btn outline style="color: goldenrod" :icon="app.video.value.show ? 'play_circle' : 'stop_circle'"
            :label="app.video.value.show ? 'Yes' : 'No'"
            @click="toggleVideo" />
            <div v-if="app.video.value.show" class="flex flex-row">
                <q-input v-model="app.video.value.width" type="text" class="q-pl-md shortInput" hint="Video Width"
                    maxlength="4" />
                <q-input v-model="app.video.value.height" type="text" class="q-pl-md shortInput" hint="Video Height"
                    maxlength="4" />
            </div>
        </div>
                
                    <div class="row justify-center q-pt-xl">
                        <div class="q-ma-md">
                            <JoystickController />
                        </div>
                        <div class="q-ma-md">
                            <ControlList :url="app.ip.value.ipaddress" :port="app.ip.value.port" />
                        </div>
                    </div>
                </q-card>
            </q-expansion-item>
        </q-list>
    </div>
</template>

<script setup>
import { ref } from "vue"
import ControlList from "../components/ControlList.vue"
import { showNote } from "../components/js/NotifyController"
import JoystickController from "src/components/JoystickController.vue"
import { tryConnect } from "../components/js/ServerController"
import { app } from "../components/js/StoreController"

const showUpdate = ref(false)
const splitterModel = ref(20)

const updateIP = () => {
    app.updateIP(app.ip.value.ipaddress, app.ip.value.port)
    tryConnect(app.ip.value.ipaddress, app.ip.value.port).then((con) => {
        app.setStatus(con)
    })
}

const toggleVideo = () => {
    if (app.video.value.show == true) app.updateVideo(false)
    else app.updateVideo(true)
}
</script>

<style>
.listItem {
    font-size: 1.2rem;
    text-align: center;
    padding-top: 10px;
}

.subListItem {
    font-size: 0.8rem;
    text-align: left;
}

.shortInput {
    width: 70px;
}

#listHeader {
    color: black;
    background-color: darkgoldenrod;
}

.hide-badge {
    display: none;
}

@media screen and (min-width: 928px) {
    .expList {
        width: 365px;
    }
}

@media screen and (min-width: 929px) {
    .expList {
        width: 500px;
    }
}
</style>
