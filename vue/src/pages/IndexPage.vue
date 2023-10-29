<template>
  <q-page class="fit column items-center">
    <joystick-controller />
  </q-page>
</template>

<script>
import JoystickController from 'src/components/JoystickController.vue';
import { defineComponent } from 'vue'

export default defineComponent({
  name: 'IndexPage',
  components: { JoystickController },
})
</script>
