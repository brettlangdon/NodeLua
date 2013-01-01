var nodelua = require('../../');

var lua = new nodelua.LuaState('simple');

lua.setGlobal('js_value', 500);

lua.doString('print("js_value: " .. js_value)', function(){
	lua.doString('js_value = "something new"', function(){
		console.log('js_value: ' + lua.getGlobal('js_value'));
	    });
    });