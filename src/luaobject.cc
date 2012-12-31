#define BUILDING_NODELUA
#include <node.h>
#include <map>
#include "luaobject.h"

using namespace v8;

LuaObject::LuaObject() {};
LuaObject::~LuaObject() {};

std::map<char *, Persistent<Function> > functions;

void LuaObject::Init(Handle<Object> target) {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("LuaObject"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  // Prototype
  tpl->PrototypeTemplate()->Set(String::NewSymbol("doString"),
				FunctionTemplate::New(DoString)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("getGlobal"),
				FunctionTemplate::New(GetGlobal)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("registerFunction"),
				FunctionTemplate::New(RegisterFunction)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("status"),
				FunctionTemplate::New(Status)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("collectGarbage"),
				FunctionTemplate::New(CollectGarbage)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("close"),
				FunctionTemplate::New(Close)->GetFunction());
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

  Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("LuaObject"), constructor);
}

Handle<Value> LuaObject::New(const Arguments& args) {
  HandleScope scope;

  LuaObject* obj = new LuaObject();
  obj->lua_ = lua_open();
  luaL_openlibs(obj->lua_);
  lua_register(obj->lua_, "nodelua", LuaObject::CallFunction);
  obj->Wrap(args.This());

  return args.This();
}



Handle<Value> LuaObject::CollectGarbage(const Arguments& args){
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if(!args[0]->IsNumber()){
    ThrowException(Exception::TypeError(String::New("Argument 1 must be a number, try nodelua.GC")));
    return scope.Close(Undefined());
  }

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  int type = (int)args[0]->ToNumber()->Value();
  int gc = lua_gc(obj->lua_, type, 0);

  return scope.Close(Number::New(gc));
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
