const MT166 = require('./index.js')
let dispenser = new MT166({ debug: true });

// dispenser.checkIfStockIsEnding().then(() => { console.log('STOCK IS ENDING!') });

// dispenser.discard().then(readAndDiscard).catch(readAndDiscard);
// dispenser.finalPosition().then(() => { }).catch()

function cont() {
    // dispenser.discard().then(readAndDiscard).catch(readAndDiscard)
}

function readAndDiscard() {
    // dispenser.readingPosition().then(cont).catch(cont);
}

// dispenser.finalPosition()

dispenser.on('discard.error', () => {
    console.log('discard bay error');
    dispenser.finalPositionIsOccupied().then(() => {
        console.log('there\'s a card occuping the final position, discard bay full');
    }).catch(() => {
        dispenser.readingPositionIsOccupied().then(() => {
            console.log('there\'s a card occuping the reading position, discard bay full');
        }).catch(() => {
            console.log('there\'s no card')
        })
    })
})
dispenser.on('stock.ending', () => { console.log('stock bay is ending'); })
dispenser.on('stock.empty', () => { console.log('stock bay is empty'); })