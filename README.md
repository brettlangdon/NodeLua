NodeLua
======

NodeLua is a module to expose Lua bindings to Node.JS.

This is still a work in progress, collaborators welcome.

## Install

Requires Lua 5.1, will *not* work with 5.2

Lua and it's C libraries are required for this module to work.

```bash
npm install nodelua
```

```javascript
var nodelua = require('nodelua');
```

## API
### NodeLua
The `NodeLua` module itself contains the objects `LuaObject`, `LuaFunction`, as well as some constants.
```javascript
var lua = new nodelua.LuaObject()
```

#### -- STATUS
`STATUS` is an object that contains the constants for values returned by `LuaObject.status()`.

`nodelua.STATUS` conatins the following constants:
 * `YIELD: 1`
 * `ERRRUN: 2`
 * `ERRSYNTAX: 3`
 * `ERRMEM: 4`
 * `ERRERR: 5`

#### -- GC
`GC` is an object of constants used for controlling the lua garbage collector.

`nodelua.GC` conatins the following constants:
 * `STOP: 0`
 * `RESTART: 1`
 * `COLLECT: 2`
 * `COUNT: 3`
 * `COUNTB: 4`
 * `STEP: 5`
 * `SETPAUSE: 6`
 * `SETSTEPMUL: 7`

#### -- INFO
`INFO` is an object containing constants with information about the version of lua you are using.

`nodelua.INFO` contains the following constants:
 * `VERSION`
 * `VERSION_NUM`
 * `COPYRIGHT`
 * `AUTHORS`

### LuaFunction
The `LuaFunction` is used to initialize a javascript function for use by lua.

One caveat to using `LuaFunction`s and multiple `LuaObject`s is that `LuaFunction`s regardless of which `LuaObject`
they are registered with are visable to ALL `LuaObject`s.

#### -- LuaFunction(func_name, func)
The constructor for `LuaFunction` requires the `func_name` to use from within Lua (`nodelua('name')`) as well as the function itself `func`.
```javascript
var func = new nodelua.LuaFunction('test', function(){
  console.log('This Is A Test');
  return 42;
});
```

#### -- name
The `name` property of the `LuaFunction` is exposed, but it cannot be changed.


### LuaObject
The `LuaObject` is an object wrapper around a `lua_State` instance.

#### -- doFile(file_name)
The `doFile` method is used to load and execute lua code stored in `file_name`.
```javascript
lua.doFile('test.lua');
```

#### -- doString(lua_code)
The `doString` method is the same as `doFile` except the code is loaded from `lua_code` rather than from a file.
```javascript
lua.doString("print('Hello, Lua')");
```

#### -- setGlobal(name, value)
The `setGlobal` method is used to provide lua with the global variable `name` containing the value `value`.
```javascript
lua.setGlobal('test', 'value');
```

#### -- getGlobal(name)
The `getGlobal` method is used to retrieve either a value set by `setGlobal` or a global variable in any lua code that has been run.
```javascript
console.log(lua.getGlobal('test'));
```

#### -- registerFunction(func)
`registerFunction` is used to expose a `LuaFunction` `func` to lua.
```javascript
var func = new nodelua.LuaFunction('add_them', function(a, b){
  console.log(a+b);
});
lua.registerFunction(func);
lua.doString('nodelua("add_them", 2, 4)');
```
There are a few caveats with `registerFunction`.

For starters in order to invoke the javascript function from within lua you must use an exposed `nodelua` function as opposed to using the functions registered `name`.
```lua
nodelua('add_them', 3, 5)
```
All `LuaFunction`s registered with `registerFunction` is registered globally for all `LuaObject`s regardless of which object is used to register it.

#### -- status()
`status` will return the current status code for lua. The result can be `0` for normal or one of the error codes in `nodelua.STATUS`.
```javascript
if(lua.status() == nodelua.STATUS.ERRSYNTAX){
  console.error('Lua Syntax Error');
}
```

#### -- collectGarbage(GC_CODE)
`collectGarbage` is used to control the lua garbage collector. `GC_CODE` should be one of the codes taken from `nodelua.GC`.
```javascript
lua.collectGarbage(nodelua.GC.COLLECT);
```

#### -- push(value)
Push `value` onto the Lua stack.
```javascript
lua.push(5);
```

#### -- pop(num)
Pop `num` items from the stack. Default is 1.
```javascript
lua.pop(5);
```

#### -- getTop()
Return the number of elements on the Lua stack.
```javascript
var num = lua.getTop();
```

#### -- setTop(index)
Set the top of the Lua stack to `index`.
```javascript
lua.setTop(3);
```

#### -- replace(index)
Replaces the top stack element into the specified `index`
```javascript
lua.repalce(3);
```

#### -- close()
`close` should be used whenever you have finished using a `LuaObject`. This will simply call `lua_close` on the `lua_State` for that object.

## Example
See `./examples/`.
```javascript
var nodelua = require('nodelua');
var lua = new nodelua.LuaObject();

var func = new nodelua.LuaFunction('add_them', function(a, b){
  return a + b;
});

lua.doFile('some_file.lua');
console.dir(lua.getGlobal('some_var'));
```

## License
The MIT License (MIT)
Copyright (c) 2012 Brett Langdon <brett@blangdon.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
