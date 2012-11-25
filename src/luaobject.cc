#define BUILDING_NODELUA
#include <node.h>
#include "luaobject.h"
#include "utils.h"

using namespace v8;

LuaObject::LuaObject() {};
LuaObject::~LuaObject() {};

Local<Object> LuaObject::functions = Object::New();

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

  Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("LuaObject"), constructor);
}

Handle<Value> LuaObject::New(const Arguments& args) {
  HandleScope scope;

  LuaObject* obj = new LuaObject();
  obj->functions = Object::New();
  obj->lua_ = lua_open();
  luaL_openlibs(obj->lua_);
  lua_register(obj->lua_, "nodelua", obj->LuaFunction);
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

  if(args[1]->IsString()){
      String::AsciiValue value(args[1]);
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


Handle<Value> LuaObject::RegisterFunction(const Arguments& args){
  HandleScope scope;

  if(args.Length() < 2){
    ThrowException(Exception::TypeError(String::New("Must Have 2 Arguments")));
    return scope.Close(Undefined());
  }

  if(!args[0]->IsString()){
    ThrowException(Exception::TypeError(String::New("Argument 1 Must Be A String")));
    return scope.Close(Undefined());
  }
  
  if(!args[1]->IsFunction()){
    ThrowException(Exception::TypeError(String::New("Argument 2 Must Be A Function")));
    return scope.Close(Undefined());
  }

  LuaObject* obj = ObjectWrap::Unwrap<LuaObject>(args.This());
  obj->functions->Set(args[0]->ToString(), args[1]);

  return scope.Close(Undefined());
}

int LuaObject::LuaFunction(lua_State *L){
  int n = lua_gettop(L);
  if(n < 1){
    lua_pushstring(L, "must have at least 1 argument");
    lua_error(L);
  }

  if (!lua_isstring(L, 1)) {
    lua_pushstring(L, "argument 1 must be a string");
    lua_error(L);
  }

  Local<String> func_name = String::New((char *)lua_tostring(L, 1));

  const unsigned argc = n - 1;
  Local<Value>* argv = new Local<Value>[argc];
  int i;
  for(i = 1; i <= n; ++i){
    argv[i-1] = lua_to_value(L, i+1);
  }

  Local<Function> func = Local<Function>::Cast(LuaObject::functions->Get(func_name));
  func->Call(Context::GetCurrent()->Global(), argc, argv);

  return 0;   
}
