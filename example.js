const MT166 = require('./index.js')
let dispenser = new MT166({ debug: true, port: 6 });

function check() {
    setTimeout(() => {
        dispenser.readingPositionIsOccupied().then(() => {
            dispenser.finalPosition().then(check).catch(check);
        }).catch(check)
    }, 2e3);
}

check();

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
