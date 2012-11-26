#define BUILDING_NODELUA
#include <node.h>
#include "luafunction.h"
#include "utils.h"

using namespace v8;

LuaFunction::LuaFunction() {};
LuaFunction::~LuaFunction() {};

void LuaFunction::Init(Handle<Object> target) {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("LuaFunction"));
  tpl->InstanceTemplate()->SetInternalFieldCount(2);
  tpl->InstanceTemplate()->SetAccessor(String::New("name"), GetName, SetName);

  Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("LuaFunction"), constructor);
}

LuaFunction* LuaFunction::unwrap(Handle<Object> handle){
  return ObjectWrap::Unwrap<LuaFunction>(handle);
}

Handle<Value> LuaFunction::New(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 2){
    ThrowException(Exception::TypeError(String::New("Must have 2 arguments")));
    return scope.Close(Undefined());
  }

  if(!args[0]->IsString()){
    ThrowException(Exception::TypeError(String::New("Argument 1 must be a string")));
    return scope.Close(Undefined());
  }

  if(!args[1]->IsFunction()){
    ThrowException(Exception::TypeError(String::New("Argument 2 must be a function")));
    return scope.Close(Undefined());
  }

  LuaFunction* obj = new LuaFunction();
  obj->func_name = get_str(args[0]);
  obj->func_def_ = Persistent<Function>::New(Handle<Function>::Cast(args[1]));
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> LuaFunction::GetName(Local<String> property, const AccessorInfo &info) {
  HandleScope scope;

  LuaFunction* obj = ObjectWrap::Unwrap<LuaFunction>(info.This());
  return scope.Close(String::New(obj->func_name));
}

void LuaFunction::SetName(Local<String> property, Local<Value> value, const AccessorInfo& info) {
}
