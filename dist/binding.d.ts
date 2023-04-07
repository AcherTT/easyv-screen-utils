declare class ScreenImportUtils {
    private _addonInstance;
    constructor(dbUrl: string);
    importScreen(time: string, packetPath: string, callback: () => void): string;
}
export = ScreenImportUtils;
