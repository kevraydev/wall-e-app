<template>
  <div class="column items-center">
    <div class="animate__animated animate__fadeInDown animate__delay-0.5s">
      <img
        alt="Wall-e logo"
        src="~assets/walle_logo_sm.png"
        class="centeredImg"
      />
    </div>
    <div id="nav" class="fit row justify-center q-pt-md">
      <router-link to="/" class="routeLinks">
        <q-btn outline color="primary" label="App" size="1.2rem" />
      </router-link>
      <div class="q-px-md"></div>
      <router-link to="/config" class="routeLinks">
        <q-btn outline color="primary" label="Config" size="1.2rem">
          <q-badge :color="connected ? 'green' : 'red'" rounded floating
        /></q-btn>
      </router-link>
    </div>
  </div>
</template>

<script setup>
import { computed } from "vue";

const props = defineProps(["status"]);

const connected = computed(() => props.status);
</script>
<style>
.centeredImg {
  margin-top: 15px;
  max-width: 100%;
  height: auto;
}

.statusConnect {
  background-color: rgb(12, 109, 12);
}

.statusDisconnect {
  background-color: rgb(155, 39, 39);
}

.routeLinks {
  text-decoration: none !important;
}

#nav a.router-link-exact-active {
  color: rgb(26, 26, 26);
  background-color: rgb(58, 57, 53);
  border-color: rgb(201, 171, 34);
  border-radius: 4px;
}
</style>

