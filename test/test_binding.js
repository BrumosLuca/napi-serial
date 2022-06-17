const myAddon = require("../build/Release/serial_addon-native");
const assert = require("assert");

assert(myAddon, "The expected function is undefined");

function RunTest(){

    myAddon.open();

    let res = myAddon.getData();
    assert.notStrictEqual(res, undefined, "FAIL return array" );

    let s = myAddon.changeSigma(+"10");

    let res1 = myAddon.getData();
    assert.notStrictEqual(res1, undefined, "FAIL return array" );
} 

function RunTest2(){

    console.log(myAddon.openDeviceInterface());
    sleep(1000);

    for (let index = 0; index < 10; index++) {

        console.log( myAddon.getData()); 
      
        sleep(1000);
    }
}


function sleep(delay) {
    var start = new Date().getTime();
    while (new Date().getTime() < start + delay);
}

assert.doesNotThrow(RunTest, undefined, "TEST FAILED through excpetion");

console.log("\n\n\n --- TEST OK! ---");
