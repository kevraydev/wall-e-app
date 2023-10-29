<template>
    <q-btn color="primary" text-color="black"
    label="Options"
    @click="showServoDia = true" />
    <q-dialog v-model="showServoDia" persistent>
    <q-card style="width: 500px; height: 380px;">
        <div id="listHeader" class="listItem">Servo List</div>
        <q-separator />
        <div class="column justify-center">
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
        </div>
    </q-card>
    </q-dialog>
</template>

<script setup>
import { ref } from 'vue'
import { fetchOptions, app } from './js/StoreController'

    const showServoDia = ref(false)

</script>

<style>
.servoList {
    background-color: rgb(45, 45, 45);
}
</style>