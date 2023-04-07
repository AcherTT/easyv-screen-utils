"use strict";
const addon = require('../build/Release/screen_importer-native');
class ScreenImportUtils {
    constructor(dbUrl) {
        this._addonInstance = new addon.ScreenImportUtils(dbUrl);
    }
    importScreen(packetPath, callback, time) {
        return this._addonInstance.importScreen(packetPath, callback, time);
    }
}
module.exports = ScreenImportUtils;
