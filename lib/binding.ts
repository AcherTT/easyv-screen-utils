const addon = require('../build/Release/screen_importer-native')

interface ScreenImporterWrap {
  importScreen(time: string, packetPath: string, callback: () => void): string
}

class ScreenImportUtils {
  private _addonInstance: ScreenImporterWrap

  constructor(dbUrl: string) {
    this._addonInstance = new addon.ScreenImportUtils(dbUrl)
  }

  importScreen(time: string, packetPath: string, callback: () => void) {
    return this._addonInstance.importScreen(time, packetPath, callback)
  }
}

export = ScreenImportUtils
