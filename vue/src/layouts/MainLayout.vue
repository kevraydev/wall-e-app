<template>
  <q-layout view="lHh Lpr lFf">
      <HeaderLayout :status="app.connect.value"/>
    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script setup>
import HeaderLayout from './HeaderLayout.vue'
import { app } from '../components/js/StoreController.js'

</script>
<style>
</style>