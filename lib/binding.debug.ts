const addon = require('../build/Debug/screen_utils_native')

interface ScreenImporterWrap {
    importScreen(packetPath: string, callback: (err: Error, result: number) => void, time: number): string
}

class ScreenImportUtils {
    private _addonInstance: ScreenImporterWrap

    constructor(
        host: string,
        user: string,
        password: string,
        database: string,
        port: number,
    ) {
        this._addonInstance = new addon.ScreenImportUtils(...arguments)
    }

    importScreen(packetPath: string, callback: (err: Error, result: number) => void, time: number) {
        return this._addonInstance.importScreen(packetPath, callback, time)
    }

}

export = ScreenImportUtils
