var path = require('path');
var nodelua = require('../../');

var lua = new nodelua.LuaState('config');

// set a default value
lua.setGlobal('js_value', 'this is js_value');
console.log('js_value: ' + lua.getGlobal('js_value'));

console.log('Processing Config.lua');
var config = path.resolve(__dirname, 'config.lua');
lua.doFile(config, function(){
	console.log('js_value: ' + lua.getGlobal('js_value'));
	console.log('val_one: ' + lua.getGlobal('val_one'));
	console.log('val_two: ' + lua.getGlobal('val_two'));
	console.log('val_three: ' + lua.getGlobal('val_three'));
    });