#define BUILDING_NODELUA
#include <node.h>
#include "luaobject.h"

using namespace v8;

LuaObject::LuaObject() {};
LuaObject::~LuaObject() {};

void LuaObject::Init(Handle<Object> target) {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("LuaObject"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("doFile"),
				FunctionTemplate::New(DoFile)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("getGlobal"),
				FunctionTemplate::New(GetGlobal)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("setGlobal"),
				FunctionTemplate::New(SetGlobal)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("close"),
				FunctionTemplate::New(Close)->GetFunction());

  Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("LuaObject"), constructor);
}

Handle<Value> LuaObject::New(const Arguments& args) {
  HandleScope scope;

  LuaObject* obj = new LuaObject();
  obj->lua_ = lua_open();
  luaL_openlibs(obj->lua_);
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> LuaObject::Close(const Arguments& args){
  HandleScope scope;
  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  lua_close(obj->lua_);
  return scope.Close(Undefined());
}

Handle<Value> LuaObject::DoFile(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("Argument 1 Must Be A String")));
    return scope.Close(Undefined());
  }

  v8::String::AsciiValue arg_name(args[0]);
  char *file_name = (char *) malloc(arg_name.length() + 1);
  strcpy(file_name, *arg_name);

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  if(luaL_dofile(obj->lua_, file_name)){
    char buf[1000];
    sprintf(buf, "Execution Of File %s Has Failed:\n%s\n", file_name, lua_tostring(obj->lua_, -1));
    ThrowException(Exception::TypeError(String::New(buf)));
    return scope.Close(Undefined());
  }

  return scope.Close(Undefined());
}


Handle<Value> LuaObject::GetGlobal(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("Argument 1 Must Be A String")));
    return scope.Close(Undefined());
  }

  v8::String::AsciiValue arg_name(args[0]);
  char *global_name = (char *) malloc(arg_name.length() + 1);
  strcpy(global_name, *arg_name);

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  lua_getglobal(obj->lua_, global_name);

  if(lua_isnumber(obj->lua_, -1)){
    return scope.Close(Number::New((int)lua_tonumber(obj->lua_, -1)));
  }else if(lua_isstring(obj->lua_, -1)){
    return scope.Close(String::New((char *)lua_tostring(obj->lua_, -1)));
  }else if(lua_isboolean(obj->lua_, -1)){
    return scope.Close(Boolean::New((int)lua_toboolean(obj->lua_, -1)));
  }

  return scope.Close(Undefined());
}


Handle<Value> LuaObject::SetGlobal(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 2){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsString()) {
    ThrowException(Exception::TypeError(String::New("Argument 1 Must Be A String")));
    return scope.Close(Undefined());
  }

  v8::String::AsciiValue arg_name(args[0]);
  char *global_name = (char *) malloc(arg_name.length() + 1);
  strcpy(global_name, *arg_name);

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());

  if(args[1]->IsString()){
      v8::String::AsciiValue value(args[1]);
      char *value_str = (char *) malloc(value.length() + 1);
      strcpy(value_str, *value);
      lua_pushstring(obj->lua_, value_str);
  }else if(args[1]->IsNumber()){
    int value = args[1]->ToNumber()->Value();
    lua_pushinteger(obj->lua_, value);
  }else if(args[1]->IsBoolean()){
    int value = (int)args[1]->ToBoolean()->Value();
    lua_pushboolean(obj->lua_, value);
  }else{
    lua_pushnil(obj->lua_);
  }
  lua_setglobal(obj->lua_, global_name);

  return scope.Close(Undefined());
}
