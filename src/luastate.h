#ifndef LUASTATE_H
#define LUASTATE_H

#include <map>
#include <node.h>

#include "utils.h"

extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaState : public node::ObjectWrap{
 public:
  lua_State* lua_;

  static void Init(v8::Handle<v8::Object> target);
  static int CallFunction(lua_State* L);

 private:
  LuaState();
  ~LuaState();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Status(const v8::Arguments& args);
  static v8::Handle<v8::Value> Close(const v8::Arguments& args);
  static v8::Handle<v8::Value> CollectGarbage(const v8::Arguments& args);

  static v8::Handle<v8::Value> DoFileSync(const v8::Arguments& args);
  static v8::Handle<v8::Value> DoFile(const v8::Arguments& args);

  static v8::Handle<v8::Value> DoStringSync(const v8::Arguments& args);
  static v8::Handle<v8::Value> DoString(const v8::Arguments& args);

  static v8::Handle<v8::Value> SetGlobal(const v8::Arguments& args);
  static v8::Handle<v8::Value> GetGlobal(const v8::Arguments& args);

  static v8::Handle<v8::Value> RegisterFunction(const v8::Arguments& args);

};
#endif
