#define BUILDING_NODELUA
#include "luastate.h"

using namespace v8;

uv_async_t async;

struct do_file_baton{
  Persistent<Function> callback;
  char* file_name;
  bool error;
  char msg[1000];
  LuaState* state;
};

void do_file(uv_work_t *req){
  do_file_baton* baton = static_cast<do_file_baton*>(req->data);

  if(luaL_dofile(baton->state->lua_, baton->file_name)){
    baton->error = true;
    sprintf(baton->msg, "Exception In File %s Has Failed:\n%s\n", baton->file_name, lua_tostring(baton->state->lua_, -1));
  }
}

void do_file_after(uv_work_t *req){
  HandleScope scope;

  do_file_baton* baton = (do_file_baton *)req->data;

  Local<Value> argv[2];
  const int argc = 2;

  if(baton->error){
    argv[0] = String::NewSymbol(baton->msg);
    argv[1] = Local<Value>::New(Undefined());
  } else{
    argv[0] = Local<Value>::New(Undefined());
    if(lua_gettop(baton->state->lua_)){
      argv[1] = lua_to_value(baton->state->lua_, -1);
    } else{
      argv[1] = Local<Value>::New(Undefined());
    }
  }

  TryCatch try_catch;

  baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);

  baton->callback.Dispose();
  delete baton;
  delete req;

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
}

LuaState::LuaState(){};
LuaState::~LuaState(){};

void LuaState::Init(Handle<Object> target){
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("LuaState"));
  tpl->InstanceTemplate()->SetInternalFieldCount(2);

  tpl->PrototypeTemplate()->Set(String::NewSymbol("doFileSync"),
				FunctionTemplate::New(DoFileSync)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("doFile"),
				FunctionTemplate::New(DoFile)->GetFunction());

  tpl->PrototypeTemplate()->Set(String::NewSymbol("setGlobal"),
				FunctionTemplate::New(SetGlobal)->GetFunction());

  Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("LuaState"), constructor);
}

Handle<Value> LuaState::New(const Arguments& args){
  HandleScope scope;

  if(!args.IsConstructCall()) {
    return ThrowException(Exception::TypeError(String::New("Use the new operator to create instances of this object.")));
  }

  LuaState* obj = new LuaState();
  obj->lua_ = lua_open();
  luaL_openlibs(obj->lua_);
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> LuaState::DoFileSync(const Arguments& args){
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("doFileSync Takes Only 1 Argument")));
    return scope.Close(Undefined());
  }

  if(!args[0]->IsString()){
    ThrowException(Exception::TypeError(String::New("doFileSync Argument 1 Must Be A String")));
    return scope.Close(Undefined());
  }

  char* file_name = get_str(args[0]);

  LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
  if(luaL_dofile(obj->lua_, file_name)){
    char buf[1000];
    sprintf(buf, "Exception Of File %s Has Failed:\n%s\n", file_name, lua_tostring(obj->lua_, -1));
    ThrowException(Exception::Error(String::New(buf)));
    return scope.Close(Undefined());
  }

  if(lua_gettop(obj->lua_)){
    return scope.Close(lua_to_value(obj->lua_, -1));
  } else{
    return scope.Close(Undefined());
  }
}

Handle<Value> LuaState::DoFile(const Arguments& args){
  HandleScope scope;

  if(args.Length() < 1){
    ThrowException(Exception::TypeError(String::New("doFile Requires At Least 1 Argument")));
    return scope.Close(Undefined());
  }

  if(!args[0]->IsString()){
    ThrowException(Exception::TypeError(String::New("doFile Argument 1 Must Be A String")));
    return scope.Close(Undefined());
  }

  if(args.Length() > 2 && !args[1]->IsFunction()){
    ThrowException(Exception::TypeError(String::New("doFile Argument 2 Must Be A Function")));
    return scope.Close(Undefined());
  }

  char* file_name = get_str(args[0]);
  Persistent<Function> callback;
  if(args.Length() > 1){
    callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
  }

  LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());
  do_file_baton* baton = new do_file_baton();
  baton->file_name = file_name;
  baton->callback = callback;
  baton->state = obj;
  obj->Ref();

  uv_work_t *req = new uv_work_t;
  req->data = baton;
  uv_queue_work(uv_default_loop(), req, do_file, do_file_after);

  return scope.Close(Undefined());
}

Handle<Value> LuaState::SetGlobal(const Arguments& args) {
  HandleScope scope;

  if(args.Length() < 2){
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  char *global_name = get_str(args[0]);

  LuaState* obj = ObjectWrap::Unwrap<LuaState>(args.This());

  push_value_to_lua(obj->lua_, args[1]);
  lua_setglobal(obj->lua_, global_name);

  return scope.Close(Undefined());
}
