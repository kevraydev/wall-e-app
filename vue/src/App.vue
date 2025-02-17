<template>
  <router-view />
</template>

<script setup>
//import { defineComponent } from 'vue'
import { useQuasar } from 'quasar';

    const $q = useQuasar();
    $q.dark.set(true);

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