declare class ScreenImportUtils {
    private _addonInstance;
    constructor(host: string, user: string, password: string, database: string, port: number);
    importScreen(packetPath: string, callback: (err: Error, result: number) => void, time: number): string;
}
export = ScreenImportUtils;
