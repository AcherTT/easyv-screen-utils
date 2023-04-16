"use strict";
const addon = require('../build/Debug/screen_utils_native');
class ScreenImportUtils {
    _addonInstance;
    constructor(host, user, password, database, port) {
        this._addonInstance = new addon.ScreenImportUtils(...arguments);
    }
    importScreen(packetPath, callback, time) {
        return this._addonInstance.importScreen(packetPath, callback, time);
    }
}
module.exports = ScreenImportUtils;
