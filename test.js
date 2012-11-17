var addon = require('./build/Release/posixy');

addon.runCallback(function (msg) {
    console.log(msg); // 'hello world'
});
