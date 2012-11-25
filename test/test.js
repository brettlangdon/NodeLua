#!/usr/bin/env node
    var path = require('path');
var nodelua = require('../');

var lua = new nodelua.LuaObject();

var test_file = path.resolve(__dirname, 'test.lua');
lua.doFile(test_file);

lua.setGlobal("test", "value");
lua.setGlobal("one", 1);
lua.setGlobal("two", true);
lua.setGlobal("none", null);

console.dir(lua.getGlobal("one"));
console.dir(lua.getGlobal("test"));
console.dir(lua.getGlobal("none"));
console.dir(lua.getGlobal("two"));

lua.close();