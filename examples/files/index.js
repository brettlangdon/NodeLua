var nodelua = require('../../');

var luastate = new nodelua.LuaState('files');

luastate.setGlobal('test', 'some value');

var file_name = __dirname + '/test.lua';
luastate.doFile(file_name, function(err, ret){
	if(!err && ret){
	    console.log(ret);
	} else{
	    console.error(err);
	}
    });