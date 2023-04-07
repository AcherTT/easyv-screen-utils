"use strict";
const addon = require('../build/Release/screen_importer-native');
class ScreenImportUtils {
    constructor(dbUrl) {
        this._addonInstance = new addon.ScreenImportUtils(dbUrl);
    }
    importScreen(time, packetPath, callback) {
        return this._addonInstance.importScreen(time, packetPath, callback);
    }
}
module.exports = ScreenImportUtils;
