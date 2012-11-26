var nodelua = require('../../');

var lua = new nodelua.LuaObject();

lua.setGlobal('js_value', 500);

lua.doString('print(js_value)');

lua.doString('js_value = "something new"');

console.dir(lua.getGlobal('js_value'));