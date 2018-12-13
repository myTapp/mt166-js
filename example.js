const MT166 = require('./index.js')
let dispenser = new MT166({ debug: true });

dispenser.readingPosition().then(() => {
    dispenser.finalPosition()
})

dispenser.on('discard.error', () => {
    console.log('discard bay error');
    dispenser.finalPositionIsOccupied().then(() => {
        console.log('there\'s a card occupying the final position, discard bay full');
    }).catch(() => {
        dispenser.readingPositionIsOccupied().then(() => {
            console.log('there\'s a card occupying the reading position, discard bay full');
        }).catch(() => {
            console.log('there\'s no card')
        })
    })
})
dispenser.on('stock.ending', () => { console.log('stock bay is ending'); })
dispenser.on('stock.empty', () => { console.log('stock bay is empty'); })
