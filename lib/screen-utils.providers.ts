import { ScreenUtilsOptions } from "./screen-utils.interface"
const addon = require('../build/Release/screen_utils_native')

interface ScreenImporterWrap {
  importScreen(
    packetPath: string,
    callback: (err: Error, result: number) => void, time: number
  ): string
}

// 单例模式
export class ScreenUtilsProviders {
  private static _instance: ScreenUtilsProviders
  private addonInstance: ScreenImporterWrap

  private constructor(...args: Array<string | number>) {
    this.addonInstance = new addon.ScreenImportUtils(...args)
  }

  static getInstance(options: ScreenUtilsOptions) {
    if (!this._instance) {
      this._instance = new ScreenUtilsProviders(
        options.host,
        options.user,
        options.password,
        options.database,
        options.port,
      )
    }
    return this._instance
  }

  async importScreen(packetPath: string, time: number) {
    return new Promise((resolve, reject) => {
      this.addonInstance.importScreen(packetPath, (err: Error, result: number) => {
        if (err) {
          reject(err)
        } else {
          resolve(result)
        }
      }, time)
    })
  }
}