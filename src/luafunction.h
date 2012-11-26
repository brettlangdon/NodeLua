#ifndef LUAFUNCTION_H
#define LUAFUNCTION_H

#include <node.h>

extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaFunction : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  static LuaFunction* unwrap(v8::Handle<v8::Object> handle);

  char *func_name;
  v8::Handle<v8::Function> func_def_;
 private:
  LuaFunction();
  ~LuaFunction();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> GetName(v8::Local<v8::String> property, const v8::AccessorInfo &info);
  static void SetName(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);  
};

#endif
