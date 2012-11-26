var path = require('path');
var nodelua = require('../');

console.log('Lua Info:');
console.dir(nodelua.INFO);

var lua = new nodelua.LuaObject();

var func = new nodelua.LuaFunction('test_func', function(a,b){
	console.dir(a+b);
    });
lua.registerFunction(func);


lua.setGlobal("test", 5);

var test_file = path.resolve(__dirname, 'test.lua');
lua.doFile(test_file);

console.log("Global Var 'global_var' From Lua:");
console.dir(lua.getGlobal('global_var'));

var code = "print('Hello, Lua')";
lua.doString(code);

console.log("Status: " + lua.status());
