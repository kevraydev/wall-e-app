<template>
    <div>
        <div ref="JoyStickDiv">
        
        </div>
    </div>
</template>

<script>
import nipplejs from 'nipplejs'

export default {
mounted() {
    var options = {
    zone: this.$refs.JoyStickDiv,
    color: 'white',
    size: 100,
    position: {top: '50%', left: '50%'},
    mode: 'static',
    dynamicPage: false
    };
    var manager = nipplejs.create(options)
    }
}
</script>

<style>
    .joy {
        width:150px;
        height:150px;
        padding:20px;
        margin:100px auto;
    }
</style>