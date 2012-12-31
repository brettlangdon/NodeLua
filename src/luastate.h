#ifndef LUASTATE_H
#define LUASTATE_H

#include <node.h>

#include "utils.h"

extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaState : public node::ObjectWrap{
 public:
  static void Init(v8::Handle<v8::Object> target);
  lua_State* lua_;

 private:
  LuaState();
  ~LuaState();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> DoFileSync(const v8::Arguments& args);
  static v8::Handle<v8::Value> DoFile(const v8::Arguments& args);
  static v8::Handle<v8::Value> SetGlobal(const v8::Arguments& args);
};
#endif
