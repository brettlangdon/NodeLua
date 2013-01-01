#define BUILDING_NODELUA
#include <node.h>
#include <map>
#include "luaobject.h"

using namespace v8;
void LuaObject::Init(Handle<Object> target) {
  tpl->PrototypeTemplate()->Set(String::NewSymbol("push"),
				FunctionTemplate::New(Push)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("pop"),
				FunctionTemplate::New(Pop)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("getTop"),
				FunctionTemplate::New(GetTop)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("setTop"),
				FunctionTemplate::New(SetTop)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("replace"),
				FunctionTemplate::New(Replace)->GetFunction());
}

Handle<Value> LuaObject::Push(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());

  push_value_to_lua(obj->lua_, args[0]);

  return scope.Close(Undefined());
}

Handle<Value> LuaObject::Pop(const Arguments& args) {
  HandleScope scope;

  int pop_n = 1;
  if(args.Length() > 0 && args[0]->IsNumber()){
    pop_n = (int)args[0]->ToNumber()->Value();
  }

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  lua_pop(obj->lua_, pop_n);

  return scope.Close(Undefined());
}

Handle<Value> LuaObject::GetTop(const Arguments& args) {
  HandleScope scope;

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  int n = lua_gettop(obj->lua_);

  return scope.Close(Number::New(n));
}

Handle<Value> LuaObject::SetTop(const Arguments& args) {
  HandleScope scope;

  int set_n = 0;
  if(args.Length() > 0 && args[0]->IsNumber()){
    set_n = (int)args[0]->ToNumber()->Value();
  }

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  lua_settop(obj->lua_, set_n);

  return scope.Close(Undefined());
}

Handle<Value> LuaObject::Replace(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("Must Have 1 Argument")));
    return scope.Close(Undefined());
  }

  if(!args[0]->IsNumber()){
    ThrowException(Exception::TypeError(String::New("Argument 1 Must Be A Number")));
    return scope.Close(Undefined());
  }

  int index = (int)args[0]->ToNumber()->Value();

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  lua_replace(obj->lua_, index);

  return scope.Close(Undefined());
}
