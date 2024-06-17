import { Notify } from 'quasar'



export function showNote(note, colorSelect, spin = false, hideBadge = false, time = 2000) {

    const options = {
        message: note,
        spinner: spin,
        color: colorSelect,
        timeout: time,
        group: 'message'
    }
    if(hideBadge)
        options.badgeClass = 'hide-badge'

    Notify.create(options)
    
}

