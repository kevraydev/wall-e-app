<template>
<div class="row justify-center">
    <q-input v-model="ip" type="text" hint="Enter IP Address" lazy-rules :rules="[
        (val) => (val && val.length > 0) || 'Please type something!', ]"
         mask="###.###.#.###" class="q-pr-md" />
    <q-input v-model="ipport" type="text" class="q-pl-md shortInput" hint="Port" maxlength="4" />
    <q-btn flat icon="check" @click="$emit('updateIp', ip, ipport)" class="q-pl-sm" size="1.3rem" color="positive" />
</div>
</template>

<script setup>
import { ref } from 'vue';

const emit = defineEmits(['updateIp'])
const props = defineProps({
    ipaddr: {
        type: String,
        reuqired: true
    },
    port: {
        type: String,
        required: true
    }
})

const ip = ref(props.ipaddr)
const ipport = ref(props.port)
</script>