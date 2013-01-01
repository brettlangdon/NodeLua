var nodelua = require('../../');

var lua = new nodelua.LuaState('functions');

lua.registerFunction('add', function(a, b){
	return a + b;
    });

lua.doStringSync("print('Result in Lua: ' .. add(10, 5))");

var lua_two = new nodelua.LuaState('two');
// this will throw an error
lua_two.doStringSync("add(10, 5)");