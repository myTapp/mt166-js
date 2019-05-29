# mt166-js
Node.js API to comunicate with Mingtetech MT166 RFID card collector and dispenser

<img width="300px" src="https://github.com/myTapp/mt166-js/blob/master/card-collector-dispenser-MT166-RF-for-both.jpg?raw=true"></img>

Original C code by [@eldertramontin](https://github.com/eldertramontin)

## Quick-start
```
npm install mt166-js
```
```
const MT166 = require('mt166-js')
let dispenser = new MT166();
```
- with options:
```
let dispenser = new MT166({ port: 3, debug: true }); // COM4
```

## Examples
```
dispenser.readingPosition().then(() => {
    dispenser.finalPosition()
})
```
```
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
```

## Listeners
```
dispenser.on('stock.ending', () => { console.log('stock bay is ending'); })
dispenser.on('stock.empty', () => { console.log('stock bay is empty'); })
dispenser.on('service.unavaliable', () => { console.log('The Dispenser is unavaliable'); })
dispenser.on('service.avaliable', () => { console.log('The Dispenser is avaliable now!'); })
```

## Port discovery

Using the ```autoDiscovery``` option, the dispense instance will search for the dispenser in all avaliable com port.
```
let dispenser = new MT166({ port: 3, debug: true, autoDiscovery: true });
```
The object above will try to connect in the port 3, if that port wont be avaliable, it will search for some port avaliable.

