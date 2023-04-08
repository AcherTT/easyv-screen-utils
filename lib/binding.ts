const addon = require('../build/Release/screen_utils_native')

interface ScreenImporterWrap {
  importScreen(packetPath: string, callback: (err: Error, result: number) => void, time: number): string
}

class ScreenImportUtils {
  private _addonInstance: ScreenImporterWrap

  constructor(dbUrl: string) {
    this._addonInstance = new addon.ScreenImportUtils(dbUrl)
  }

  importScreen(packetPath: string, callback: (err: Error, result: number) => void, time: number) {
    return this._addonInstance.importScreen(packetPath, callback, time)
  }

}

export = ScreenImportUtils
