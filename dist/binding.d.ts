declare class ScreenImportUtils {
    private _addonInstance;
    constructor(dbUrl: string);
    importScreen(packetPath: string, callback: (err: Error, result: number) => void, time: number): string;
}
export = ScreenImportUtils;
