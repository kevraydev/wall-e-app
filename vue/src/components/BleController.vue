<template>
    <Teleport to="#backdrop">
        <div class="bleButton">
            <button @click="sendData">Send Data</button>
        </div>
    </Teleport>
</template>
<script>
export default {
    methods: {
        async sendData(){
            const sUUID = "f8c9eefe-8eae-42a7-95e6-23c010b36b9c"
            const cUUID = "f72f9f09-2c7c-49ac-88f8-5ed1527b2c7e"

        try {
            const bleDevice = await navigator.bluetooth.requestDevice({
                filters: [{ services: [sUUID] }]
            });

            const server = await device.gatt.connect()
            const service = await server.getPrimaryService(sUUID)
            const ch = await service.getCharacteristic(cUUID)

            const value = 40
            const data = Uint8Array([value])
            await ch.writeValue(data)
            console.log("Data Sent!")
        }
        catch(error) {
            console.error("problem sending data!", error)
        }
    }
}
}
</script>
<style>
    .bleButton {
        margin: 100px auto;
        width: 100px;
        height: 100px;
    }
</style>