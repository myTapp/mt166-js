const { exec } = require('child_process')
const path = require('path')
const os = require('os');

class MT166 {
    constructor(options) {
        let default_options = {
            port: 2,
            path: (os.type() === 'Linux')? './bin/MT166': (os.type() === 'Windows_NT')? './bin/MT166.exe':'./bin/MT166.exe',
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
            IS_UNAVALIABLE: '-1',
            IS_AVALIABLE: '9'
        }

        this.connected = true
        this.listeners = {}
        this.options = Object.assign(default_options, options)
        this.options.path = `"${path.resolve(__dirname, this.options.path)}"`
        this.queue = []
        this.checkStock = this.checkStock.bind(this)
        this.initNotifications()

        if (options.autoDiscovery) {
            this.execute(this.OP_CODES.PING, true)
        }
    }

    log(msg, error = false) {
        if (this.options.debug) {
            error ? console.error(msg) : console.log(msg);
        }
    }

    async discoverComPorts() {
        if (this.options.debug) {
            console.warn(`Default port #${this.options.port} isnt responding.`)
        }
        for (let i = 0; i < 12; i++) {
            this.log(`Trying to connect in port #${i}...`)
            this.options.port = i
            let connected
            try {
                const portResponse = await this.execute(this.OP_CODES.PING, true)
                connected = portResponse !== -1
            } catch (e) {
                connected = e !== -1
            }
            if (connected) {
                this.log(`Port #${i} is responding!Using that.`)
                this.notify(this.OP_CODES.IS_AVALIABLE)
                return this.connected = true
            }
            this.log(`Port #${i} does not responds.`);
        }
        this.notify(-1)
        return this.connected = false
    }

    initNotifications() {
        this.notifications = {};

        this.notifications[this.OP_CODES.IS_AVALIABLE] = () => {
            if (this.connected) {
                return;
            }
            this.emit('service.avaliable')
            this.connected = true
        }
        this.notifications[this.OP_CODES.IS_UNAVALIABLE] = () => {
            if (!this.connected) {
                return;
            }
            this.emit('service.unavaliable')
            this.connected = false
            if (this.options.autoDiscovery) {
                if (this.discoverIntervalId) {
                    return;
                }
                const intervalTimeout = () => {
                    this.discoverIntervalId = setTimeout(async () => {
                        const connected = await this.discoverComPorts()
                        if (connected) {
                            clearInterval(this.discoverIntervalId)
                            this.discoverIntervalId = null
                            return;
                        }
                        intervalTimeout()
                    }, 2e4);
                }
                intervalTimeout()
            }
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
                console.error(err);
            }
        }
    }

    execute(code, local = false, promise) {
        if (!this.onExecution) {
            this.onExecution = true;
            if(!promise) {
                return new Promise((resolve, reject) => {
                    if (!(this.connected || local)) {
                        return reject(this.OP_CODES.IS_UNAVALIABLE)
                    }
                    const command = this.createCommand(code)
                    this.log(`Executing: ${command}`)
                    exec(command, (e, stdout, stderr) => {
                        this.onExecution = false;
                        const returnCode = +stdout;
                        if (returnCode === 0 && (!local)) {
                            this.notify(code)
                        }
                        if (returnCode === -1) {
                            if (!local) {
                                this.notify(-1)
                            }
                            return this.handleReturn(e, '-1', stderr, resolve, reject, command)
                        }
                        if (code === this.OP_CODES.READING_POSITION || code === this.OP_CODES.FINAL_POSITION) {
                            this.checkStock(() => {
                                this.handleReturn(e, stdout, stderr, resolve, reject, command)
                            });
                        }
                        else {
                            this.handleReturn(e, stdout, stderr, resolve, reject, command)
                        }
    
                        this.checkQueue();
                    })
                })
            }
            else {
                if (!(this.connected || local)) {
                    return promise.reject(this.OP_CODES.IS_UNAVALIABLE)
                }
                const command = this.createCommand(code)
                this.log(`Executing: ${command}`)
                exec(command, (e, stdout, stderr) => {
                    this.onExecution = false;
                    const returnCode = +stdout;
                    if (returnCode === 0 && (!local)) {
                        this.notify(code)
                    }
                    if (returnCode === -1) {
                        if (!local) {
                            this.notify(-1)
                        }
                        return this.handleReturn(e, '-1', stderr, promise.resolve, promise.reject, command)
                    }
                    if (code === this.OP_CODES.READING_POSITION || code === this.OP_CODES.FINAL_POSITION) {
                        this.checkStock(() => {
                            this.handleReturn(e, stdout, stderr, promise.resolve, promise.reject, command)
                        });
                    }
                    else {
                        this.handleReturn(e, stdout, stderr, promise.resolve, promise.reject, command)
                    }

                    this.checkQueue();
                })
            }
        }
        else {
            return this.pushToQueue(code, local);
        }
    }

    pushToQueue(code, local) {
        var res, rej;

        var promise = new Promise((resolve, reject) => {
            res = resolve;
            rej = reject;
        });

        promise.resolve = res;
        promise.reject = rej;

        this.queue.push([code, local, promise]);
        return promise;
    }

    checkQueue() {
        if (this.queue.length > 0) {
            let next = this.queue.shift();
            try {
                this.execute(next[0], next[1], next[2]);
            } catch (err) { console.error(err); }
        }
    }

    createCommand(cmd) {
        return `${this.options.path} ${cmd} ${this.options.port}`
    }

    handleReturn(e, stdout, stderr, resolve, reject, command) {
        stdout = stdout.trim()
        this.log(`${command}: ${stdout}`)
        if (e instanceof Error) {
            this.log(e, true);
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