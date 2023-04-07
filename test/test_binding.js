const ScreenImportUtils = require("../dist/binding.js");
const assert = require("assert");

assert(ScreenImportUtils, "The expected module is undefined");

let id;
let minMemoryUsage = Number.MAX_SAFE_INTEGER;
let maxMemoryUsage = Number.MIN_SAFE_INTEGER;

function testBasic() {
    const instance = new ScreenImportUtils("mysql://root:root@localhost:3306/easyv_saas");
    assert(instance.importScreen, "The expected method is not defined");

    const now = new Date().getTime()

    instance.importScreen(
        '/media/tantan/办公/work/code/easyv-screen-utils/screen_test/big_json/160.json',
        (err, result) => {
            clearInterval(id);
            console.log("using time: ", new Date().getTime() - now)
            console.log("result: ", err, result)
            console.log("minMemoryUsage: ", minMemoryUsage);
            console.log("maxMemoryUsage: ", maxMemoryUsage);
        }, 11)
}


id = setInterval(() => {
    const finalMemoryUsage = process.memoryUsage().rss;
    if (finalMemoryUsage < minMemoryUsage) {
        minMemoryUsage = finalMemoryUsage;
    }
    if (finalMemoryUsage > maxMemoryUsage) {
        maxMemoryUsage = finalMemoryUsage;
    }
}, 100);

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");

console.log("Tests passed- everything looks OK!");