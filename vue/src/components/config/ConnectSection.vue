<template>
<div class="column items-center q-pa-md">
    <q-list dense bordered padding class="rounded-borders" style="width: 100%">
        <q-item-label caption class="q-pl-sm">IP Address</q-item-label>
        <q-item class="settingList" clickable v-ripple>
            <q-item-section>
                <div class="full-width row inline wrap justify-between">
                    <p class="listItem">{{ ipaddr }}</p>
                    <p class="listItem">{{ port }}</p>
                    <q-btn flat :icon="connect ? 'refresh' : 'sync_problem'"
                        @click="$emit('refreshConnection')" class="q-pl-sm" size="1.2rem"
                        :color="connect ? 'primary' : 'red'" />
                </div>
            </q-item-section>
        </q-item>
    </q-list>
</div>
</template>

<script setup>
import { defineProps, defineEmits } from 'vue';

const emit = defineEmits(['refreshConnection'])

const props = defineProps({
    ipaddr: {
        type: String,
        required: true
    },
    port: {
        type: String,
        required: true
    },
    connect: {
        type: Boolean,
        required: true
    },
})

</script>