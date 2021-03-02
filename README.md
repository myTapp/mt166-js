# mt166-js
Node.js API to comunicate with Mingte tech MT166 RFID card collector and dispenser

<img width="300px" src="https://github.com/myTapp/mt166-js/blob/master/card-collector-dispenser-MT166-RF-for-both.jpg?raw=true"></img>

Original C code by [@eldertramontin](https://github.com/eldertramontin)

## Quick-start
```
npm install mt166-js
```
```javascript
const MT166 = require('mt166-js')
let dispenser = new MT166();
```
- with options:
```javascript
let dispenser = new MT166({ port: 3, debug: true }); // COM4
```

## Examples
```javascript
dispenser.readingPosition().then(() => {
    dispenser.finalPosition()
})
```
```javascript
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
```javascript
dispenser.on('stock.ending', () => { console.log('stock bay is ending'); })
dispenser.on('stock.empty', () => { console.log('stock bay is empty'); })
dispenser.on('service.unavaliable', () => { console.log('The Dispenser is unavaliable'); })
dispenser.on('service.avaliable', () => { console.log('The Dispenser is avaliable now!'); })
```

## Port discovery

The ```autoDiscovery``` option will make the instance search all available COM ports automatically until it finds a valid connection.
```javascript
// This instance will try to connect to the COM4 port
// If the port isn't available it will start the automatic search

let dispenser = new MT166({ port: 3, debug: true, autoDiscovery: true });
```
