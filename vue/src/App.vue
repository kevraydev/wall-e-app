<template>
  <router-view />
</template>

<script setup>
//import { defineComponent } from 'vue'
import { useQuasar } from 'quasar';
//import { useWebSocket } from 'vue-websocket';
//import "animate.css"
//export default defineComponent({
  //name: 'App',
  //setup() {
  //const webSocket = useWebSocket('/');
    const $q = useQuasar();
    $q.dark.set(true);
 // }
//})
</script>

<style>
body::-webkit-scrollbar {
  display: none;
}

body {
  -ms-overflow-style: none;
  scrollbar-width: none;
}
</style>