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
  tpl->PrototypeTemplate()->Set(String::NewSymbol("doFile"),
				FunctionTemplate::New(DoFile)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("doString"),
				FunctionTemplate::New(DoString)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("getGlobal"),
				FunctionTemplate::New(GetGlobal)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("setGlobal"),
				FunctionTemplate::New(SetGlobal)->GetFunction());
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

Handle<Value> LuaObject::Close(const Arguments& args){
  HandleScope scope;
  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  lua_close(obj->lua_);
  return scope.Close(Undefined());
}

Handle<Value> LuaObject::Status(const Arguments& args){
  HandleScope scope;
  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  int status = lua_status(obj->lua_);
  
  return scope.Close(Number::New(status));
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

Handle<Value> LuaObject::DoFile(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  char *file_name = get_str(args[0]);

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  if(luaL_dofile(obj->lua_, file_name)){
    char buf[1000];
    sprintf(buf, "Execution Of File %s Has Failed:\n%s\n", file_name, lua_tostring(obj->lua_, -1));
    ThrowException(Exception::TypeError(String::New(buf)));
    return scope.Close(Undefined());
  }

  return scope.Close(Undefined());
}


Handle<Value> LuaObject::DoString(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  char *lua_code = get_str(args[0]);

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  if(luaL_dostring(obj->lua_, lua_code)){
    char buf[1000];
    sprintf(buf, "Execution Of Lua Code Has Failed:\n%s\n", lua_tostring(obj->lua_, -1));
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

  char *global_name = get_str(args[0]);

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  lua_getglobal(obj->lua_, global_name);

  Local<Value> val = lua_to_value(obj->lua_, -1);

  return scope.Close(val);
}


Handle<Value> LuaObject::SetGlobal(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 2){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  char *global_name = get_str(args[0]);

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());

  push_value_to_lua(obj->lua_, args[1]);
  lua_setglobal(obj->lua_, global_name);

  return scope.Close(Undefined());
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


Handle<Value> LuaObject::RegisterFunction(const Arguments& args){
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("Must Have 1 Argument")));
    return scope.Close(Undefined());
  }

  if(!args[0]->IsObject()){
    ThrowException(Exception::TypeError(String::New("Argument 1 Must Be An Object")));
    return scope.Close(Undefined());
  }

  Handle<Object> handle = Handle<Object>::Cast(args[0]);
  class LuaFunction* func = LuaFunction::unwrap(handle);

  functions[func->func_name] = func->func_def_;
  return scope.Close(Undefined());
}

int LuaObject::CallFunction(lua_State *L){
  int n = lua_gettop(L);
  if(n < 1){
    lua_pushstring(L, "must have at least 1 argument");
    lua_error(L);
  }

  if (!lua_isstring(L, 1)) {
    lua_pushstring(L, "argument 1 must be a string");
    lua_error(L);
  }

  char * func_name = (char *)lua_tostring(L, 1);

  const unsigned argc = n - 1;
  Local<Value>* argv = new Local<Value>[argc];
  int i;
  for(i = 1; i <= n; ++i){
    argv[i-1] = lua_to_value(L, i+1);
  }

  Handle<Value> ret_val = Undefined();

  std::map<char *,Persistent<Function> >::iterator iter;
  for(iter = functions.begin(); iter != functions.end(); iter++){
    if(strcmp(iter->first, func_name) == 0){
      Persistent<Function> func = iter->second;
      ret_val = func->Call(Context::GetCurrent()->Global(), argc, argv);
      break;
    }
  }
  
  push_value_to_lua(L, ret_val);
  return 1;
}
