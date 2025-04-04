<template>
<div class="row items-center justify-center q-pb-sm q-pt-lg">
    <div class="q-pr-sm">Show Video?</div>
    <q-btn outline style="color: goldenrod" :icon="videoShow ? 'play_circle' : 'stop_circle'"
        :label="videoShow ? 'Yes' : 'No'"
        @click="$emit('toggleVid', width, height)" />
    <div v-if="videoShow" class="flex flex-row">
        <q-input v-model="width" type="text" class="q-pl-md shortInput" hint="Video Width"
            maxlength="4" />
        <q-input v-model="height" type="text" class="q-pl-md shortInput" hint="Video Height"
            maxlength="4" />
    </div>
</div>
</template>

<script setup>
import { ref } from 'vue';
const emit = defineEmits(['toggleVid'])
const props = defineProps({
    videoShow: {
        type: Boolean,
        required: true
    },
    videoWidth: {
        type: String,
        required: true
    },
    videoHeight: {
        type: String,
        required: true
    }
})

const width = ref(props.videoWidth)
const height = ref(props.videoHeight)


</script>