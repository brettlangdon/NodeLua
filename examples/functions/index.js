var nodelua = require('../../');

var lua = new nodelua.LuaObject();

var add_them = new nodelua.LuaFunction('add_them', function(a, b){
	console.log('Adding ' + a + ' and ' + b + ' in js');
	return a + b;
    });
lua.registerFunction(add_them);

// Functionas are registered globally
// for all LuaObjects
var lua_two = new nodelua.LuaObject();
lua_two.doString("print('Result in Lua: ' .. nodelua('add_them', 10, 5))");

var subtract_them = new nodelua.LuaFunction('subtract_them', function(a, b){
	console.log('Subtracting ' + a + ' and ' + b + ' in js');
	return a - b;
    });

lua_two.registerFunction(subtract_them);
lua.doString("print('Result in Lua: ' .. nodelua('subtract_them', 10, 5))");