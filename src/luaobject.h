#ifndef LUAOBJECT_H
#define LUAOBJECT_H

#include <node.h>

#include "utils.h"
#include "luafunction.h"

extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaObject : public node::ObjectWrap {
  static v8::Handle<v8::Value> Push(const v8::Arguments& args);
  static v8::Handle<v8::Value> Pop(const v8::Arguments& args);
  static v8::Handle<v8::Value> GetTop(const v8::Arguments& args);
  static v8::Handle<v8::Value> SetTop(const v8::Arguments& args);
  static v8::Handle<v8::Value> Replace(const v8::Arguments& args);
};

#endif
