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

### Environment Variables
These two environment variables should really only be used when there are installation problems

* `NODELUA_INCLUDE` - additional directory to search for lua.h in. example: `NODELUA_INCLUDE=/opt/lua`
* `NODELUA_FLAGS` - additional library flags to use. example: `NODELUA_FLAGS=-llua5.1`

## Installation Problems
To try and narrow down where the error is coming from try running the following commands:
```bash
$ find /usr/include /usr/local/include -name lua.h | sed s/lua.h//
/usr/include/lua5.1/
$ pkg-config --libs-only-l --silence-errors lua || pkg-config --libs-only-l --silence-errors lua5.1
-llua5.1
```

If instead they show nothing or an error then there are a few possible explanations:

* Lua Libraries are not installed
 * This can be remedied with something like `[sudo] apt-get install liblua5.1-dev`
* Lua Libraries are not in an expected location `/usr/include/` or `/usr/local/include`
 * This can be solved by setting install time environment variables `NODELUA_INCLUDE` and `NODELUA_FLAGS`
 * `NODELUA_INCLUDE="/path/where/lua.h/is/" NODELUA_FLAGS="-llua5.1" npm install nodelua`


## API
### NodeLua
The `NodeLua` module itself contains the object `LuaState` as well as some constants.
```javascript
var lua = new nodelua.LuaState('lua')
```

#### -- STATUS
`STATUS` is an object that contains the constants for values returned by `LuaState.status()` or `LuaState.statusSync()`.

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

### LuaState
The `LuaState` is an object wrapper around a `lua_State` instance.

#### -- new LuaState(name)
When creating a new `LuaState` you must provide it with a name, this is to help stop conflicts between registering functions.
You should provide unique names to each `LuaState` instance.

#### -- getName()
Returns the name provided when creating creating the `LuaState`

#### -- doFile(file_name, callback)
The `doFile` method is used to load and execute lua code stored in `file_name`.
```javascript
lua.doFile('test.lua', function(error, ret_value){
    if(!error && ret_value){
      console.dir(ret_value);
    } else{
      console.error(error);
    }
  });
```

#### -- doFileSync(file_name)
This is the synchronous version of `doFile`, any value returned from the script is returned.
```javascript
var ret_value = lua.doFileSync('test.lua');
console.dir(ret_value);
```

#### -- doString(lua_code, callback)
The `doString` method is the same as `doFile` except the code is loaded from `lua_code` rather than from a file.
```javascript
lua.doString("print('Hello, Lua')", function(error, ret_value){
    if(!error && ret_value){
      console.dir(ret_value);
    } else{
      console.error(error);
    }
  });
```

####  -- doStringSync(lua_code)
This is the synchronous version of `doString`, any value returned from the script is returned.
```javascript
var ret_value = lua.doString("return 5");
console.dir(ret_value);
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

#### -- registerFunction(name, func)
`registerFunction` is used to expose a javascript function to lua.
```javascript
lua.registerFunction('add_them', function(a, b){
  return a + b;
});
var ret_value = lua.doStringSync('return add_them(2, 4)');
console.dir(ret_value);
```

#### -- status(callback)
`status` will return the current status code for lua. The result can be `0` for normal or one of the error codes in `nodelua.STATUS`.
```javascript
lua.status(function(code){
    if(code == nodelua.STATUS.ERRSYNTAX){
      console.error('Lua Syntax Error');
    }
  });
```

#### -- statusSync()
This is the synchronous version of `status`
```javascript
var code = lua.statusSync();
console.dir(code);
```

#### -- collectGarbage(GC_CODE, callback)
`collectGarbage` is used to control the lua garbage collector. `GC_CODE` should be one of the codes taken from `nodelua.GC`.
```javascript
lua.collectGarbage(nodelua.GC.COLLECT, function(code){
    console.dir(code);
  });
```

#### -- collectGarbageSync(GC_CODE)
This is the synchronous version of `collectGarbage`.
```javascript
var code = lua.collectGarbageSync(nodelua.GC.COLLECT);
console.dir(code);
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
lua.replace(3);
```

#### -- close()
`close` should be used whenever you have finished using a `LuaState`. This will simply call `lua_close` on the `lua_State` for that object.

## Example
See `./examples/`.
```javascript
var nodelua = require('nodelua');
var lua = new nodelua.LuaState('example');

lua.registerFunction('add_them', function(a, b){
  return a + b;
});

lua.doFile('some_file.lua', function(error, ret_value){
    console.dir(lua.getGlobal('some_var'));
  });
```

## License
The MIT License (MIT)
Copyright (c) 2012 Brett Langdon <brett@blangdon.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
