<template>
  <div class="column items-center q-pt-lg">
    <q-list bordered>
      <q-expansion-item
        group="settingsGroup"
        icon="settings"
        label="Connection"
        default-opened
        header-class="text-primary listItem"
        class="expList">
        <q-card>
          <ConnectSection
            :ipaddr="ipaddress"
            :port="appPort"
            :connect="appConnect"
            @refreshConnection="showUpdate = true"
          />
        </q-card>
      </q-expansion-item>

      <q-separator />

      <q-expansion-item
        v-model="showUpdate"
        group="settingsGroup"
        icon="add_circle"
        label="Update Settings"
        header-class="text-primary listItem"
        class="width:expList"
      >
        <q-card>
          <UpdateSection
            :ipaddr="ipaddress"
            :port="appPort"
            @update-ip="updateIP"
          />
          <q-separator class="q-mt-sm" />
          <VideoSection
            :videoShow="vidShow"
            :videoHeight="vidHeight"
            :videoWidth="vidWidth"
            @toggle-vid="toggleVideo"
          />
          <div class="row justify-center q-pt-xl">
            <div class="q-ma-md">
              <JoystickController />
            </div>
            <div class="q-ma-md">
              <ControlList
                :url="app.ip.value.ipaddress"
                :port="app.ip.value.port"
              />
            </div>
          </div>
        </q-card>
      </q-expansion-item>
    </q-list>
  </div>
</template>

<script setup>
import { ref, computed } from "vue";
import ConnectSection from "./config/ConnectSection.vue";
import UpdateSection from "./config/UpdateSection.vue";
import VideoSection from "./config/VideoSection.vue";
import ControlList from "../components/ControlList.vue";
import { showNote } from "../components/js/NotifyController";
import JoystickController from "src/components/JoystickController.vue";
import { tryConnect } from "../components/js/ServerController";
import { app } from "../components/js/StoreController";

const showUpdate = ref(false);
const splitterModel = ref(20);

const ipaddress = computed(() => app.ip.value.ipaddress);
const appPort = computed(() => app.ip.value.port);
const appConnect = computed(() => app.connect.value);
const vidShow = computed(() => app.video.value.show);
const vidHeight = computed(() => app.video.value.height);
const vidWidth = computed(() => app.video.value.width);

const updateIP = (ip, port) => {
  app.updateIP(ip, port);
  tryConnect(app.ip.value.ipaddress, app.ip.value.port).then((con) => {
    app.setStatus(con);
  });
};

const toggleVideo = (width, height) => {
  app.video.value.height = height;
  app.video.value.width = width;
  if (app.video.value.show == true) app.updateVideo(false);
  else app.updateVideo(true);
};
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
