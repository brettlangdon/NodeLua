#include <node.h>
#include <v8.h>

#include "luastate.h"

extern "C"{
#include <lua.h>
}

using namespace v8;

void init_info_constants(Handle<Object> target){
  Local<Object> constants = Object::New();
  constants->Set(String::NewSymbol("VERSION"), String::New(LUA_VERSION));
  constants->Set(String::NewSymbol("VERSION_NUM"), Number::New(LUA_VERSION_NUM));
  constants->Set(String::NewSymbol("COPYRIGHT"), String::New(LUA_COPYRIGHT));
  constants->Set(String::NewSymbol("AUTHORS"), String::New(LUA_AUTHORS));
  target->Set(String::NewSymbol("INFO"), constants);
}

void init_status_constants(Handle<Object> target){
  Local<Object> constants = Object::New();
  constants->Set(String::NewSymbol("YIELD"), Number::New(LUA_YIELD));
  constants->Set(String::NewSymbol("ERRRUN"), Number::New(LUA_ERRRUN));
  constants->Set(String::NewSymbol("ERRSYNTAX"), Number::New(LUA_ERRSYNTAX));
  constants->Set(String::NewSymbol("ERRMEM"), Number::New(LUA_ERRMEM));
  constants->Set(String::NewSymbol("ERRERR"), Number::New(LUA_ERRERR));
  target->Set(String::NewSymbol("STATUS"), constants);
}

void init_gc_constants(Handle<Object> target){
  Local<Object> constants = Object::New();
  constants->Set(String::NewSymbol("STOP"), Number::New(LUA_GCSTOP));
  constants->Set(String::NewSymbol("RESTART"), Number::New(LUA_GCRESTART));
  constants->Set(String::NewSymbol("COLLECT"), Number::New(LUA_GCCOLLECT));
  constants->Set(String::NewSymbol("COUNT"), Number::New(LUA_GCCOUNT));
  constants->Set(String::NewSymbol("COUNTB"), Number::New(LUA_GCCOUNTB));
  constants->Set(String::NewSymbol("STEP"), Number::New(LUA_GCSTEP));
  constants->Set(String::NewSymbol("SETPAUSE"), Number::New(LUA_GCSETPAUSE));
  constants->Set(String::NewSymbol("SETSTEPMUL"), Number::New(LUA_GCSETSTEPMUL));
  target->Set(String::NewSymbol("GC"), constants);
}

void init(Handle<Object> target) {
  LuaState::Init(target);
  init_gc_constants(target);
  init_status_constants(target);
  init_info_constants(target);
}
NODE_MODULE(nodelua, init)
