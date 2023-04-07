const ScreenImportUtils = require("../dist/binding.js");
const assert = require("assert");

assert(ScreenImportUtils, "The expected module is undefined");

function testBasic()
{
    const instance = new ScreenImportUtils("mysql://root:root@localhost:3306/easyv_saas");
    assert(instance.importScreen, "The expected method is not defined");
    instance.importScreen(11,'', () => {
        console.log("importScreen callback called");
    })
}


assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");

console.log("Tests passed- everything looks OK!");