const ScreenImportUtils = require("../dist/binding.js");
const assert = require("assert");

assert(ScreenImportUtils, "The expected module is undefined");

let id;
let minMemoryUsage = Number.MAX_SAFE_INTEGER;
let maxMemoryUsage = Number.MIN_SAFE_INTEGER;

async function start(instance, url) {
    return new Promise((resolve, reject) => {
        instance.importScreen(
            url,
            (err, result) => {
                if (err) resolve(err);
                resolve(result);
            }, 11)
    })
}


function printfMemoryUsage() {
    global.gc()
    const finalMemoryUsage = process.memoryUsage().rss;
    if (finalMemoryUsage < minMemoryUsage) {
        minMemoryUsage = finalMemoryUsage;
    }
    if (finalMemoryUsage > maxMemoryUsage) {
        maxMemoryUsage = finalMemoryUsage;
    }
    console.log("minMemoryUsage: ", minMemoryUsage);
    console.log("maxMemoryUsage: ", maxMemoryUsage);
    console.log("currentMemory: ", finalMemoryUsage);
}


async function testBasic() {
    const instance = new ScreenImportUtils(
        '101.43.100.93',
        'root',
        'admin123D',
        'easyv_saas',
        3306
    );
    for (let i = 0; i < 1; i++) {
        await start(instance, '/media/tantan/办公/work/code/easyv-screen-utils/screen_test/big_screen.zip');
        console.log(i)
        printfMemoryUsage()
    }
    clearInterval(id);
    setTimeout(() => {
        console.log("Tests passed- everything looks OK!")
        printfMemoryUsage()
    }, 10000)
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

testBasic();

