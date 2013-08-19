#ifndef LUAUTILS_H
#define LUAUTILS_H

#include <string.h>
#include <node.h>

extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

char * get_str(v8::Local<v8::Value> val);
v8::Local<v8::Value> lua_to_value(lua_State* L, int);
void push_value_to_lua(lua_State* L, v8::Handle<v8::Value> value);
#endif
