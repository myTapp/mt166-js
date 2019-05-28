const { exec } = require('child_process')
const path = require('path')

class MT166 {
    constructor(options) {
        let default_options = {
            port: 2,
            path: './bin/MT166.exe'
        }

        this.OP_CODES = {
            READING_POSITION: '1',
            FINAL_POSITION: '2',
            CHECK_CARD_DISPENSE: '3',
            CHECK_CARD_READING: '4',
            CHECK_STOCK_ENDING: '5',
            CHECK_STOCK_EMPTY: '6',
            DISCARD: '7',
            PING: '8',
            IS_EMPTY: '100',
            IS_ENDING: '101',
            IS_UNAVALIABLE: '-1'
        }

        this.listeners = {}
        this.options = Object.assign(default_options, options)
        this.options.path = `"${path.resolve(__dirname, this.options.path)}"`
        this.initNotifications();
    }

    initNotifications() {
        this.notifications = {};

        this.notifications[this.OP_CODES.IS_UNAVALIABLE] = () => {
            this.emit('service.unavaliable');
        }
        this.notifications[this.OP_CODES.DISCARD] = () => {
            this.emit('discard.error');
        }
        this.notifications[this.OP_CODES.IS_EMPTY] = () => {
            this.emit('stock.empty');
        }
        this.notifications[this.OP_CODES.IS_ENDING] = () => {
            this.emit('stock.ending');
        }
        this.notifications[this.OP_CODES.PING] = () => {
            this.emit('service.version');
        }
    }

    emit(code, data) {
        if (this.listeners[code]) {
            try {
                this.listeners[code](data);
            } catch (err) { }
        }
    }

    on(code, cb) {
        if (cb) {
            this.listeners[code] = cb;
        }
    }

    checkStock(cb) {
        this.checkIfStockIsEmpty().then(() => {
            this.notify(this.OP_CODES.IS_EMPTY);
            cb();
        }).catch(() => {
            this.checkIfStockIsEnding().then(() => {
                this.notify(this.OP_CODES.IS_ENDING);
                cb();
            }).catch(cb);
        });
    }

    readingPosition() {
        return this.execute(this.OP_CODES.READING_POSITION);
    }

    readingPositionIsOccupied() {
        return this.execute(this.OP_CODES.CHECK_CARD_READING);
    }

    finalPosition() {
        return this.execute(this.OP_CODES.FINAL_POSITION);
    }

    finalPositionIsOccupied() {
        return this.execute(this.OP_CODES.CHECK_CARD_DISPENSE);
    }

    checkIfStockIsEnding() {
        return this.execute(this.OP_CODES.CHECK_STOCK_ENDING);
    }

    checkIfStockIsEmpty() {
        return this.execute(this.OP_CODES.CHECK_STOCK_EMPTY);
    }

    ping() {
        return this.execute(this.OP_CODES.PING);
    }

    discard() {
        return this.execute(this.OP_CODES.DISCARD);
    }

    notify(code) {        
        if (this.notifications[code]) {
            try {                
                this.notifications[code]();
            }
            catch (err) {
                console.error(error);
            }
        }
    }

    execute(code) {
        return new Promise((resolve, reject) => {            
            exec(this.createCommand(code), (e, stdout, stderr) => {                
                const returnCode = +stdout;
                if (returnCode === 0) {
                    this.notify(code)
                }
                if (returnCode === -1) {
                    this.notify(-1)
                    return this.handleReturn(e, '-1', stderr, resolve, reject)
                }
                if (code === this.OP_CODES.READING_POSITION || code === this.OP_CODES.FINAL_POSITION) {
                    this.checkStock(() => {
                        this.handleReturn(e, stdout, stderr, resolve, reject)
                    });
                }
                else {
                    this.handleReturn(e, stdout, stderr, resolve, reject)
                }
            })
        })
    }

    createCommand(cmd) {
        return `${this.options.path} ${cmd} ${this.options.port}`
    }

    handleReturn(e, stdout, stderr, resolve, reject) {
        stdout = stdout.trim()
        if (this.options.debug === true) { console.log(stdout) }
        if (e instanceof Error) {
            console.error(e)
            return reject(e)
        }        
        if (+stdout === 1) {
            return resolve()
        }
        if (+stdout === 0) {
            return reject()
        }
        if (+stdout === -1) {
            return reject(-1)
        }
    }
}

module.exports = MT166