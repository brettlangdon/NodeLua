#include <stdlib.h>
#include "utils.h"

char * get_str(v8::Local<v8::Value> val){
  if(!val->IsString()){
    v8::ThrowException(v8::Exception::TypeError(v8::String::New("Argument Must Be A String")));
    return NULL;
  }

  v8::String::AsciiValue val_string(val);
  char * val_char_ptr = (char *) malloc(val_string.length() + 1);
  strcpy(val_char_ptr, *val_string);
  return val_char_ptr;
}


v8::Local<v8::Value> lua_to_value(lua_State* L, int i){
  switch(lua_type(L, i)){
  case LUA_TBOOLEAN:
    return v8::Local<v8::Boolean>::New(v8::Boolean::New((int)lua_toboolean(L, i)));
    break;
  case LUA_TNUMBER:
    return v8::Local<v8::Number>::New(v8::Number::New(lua_tonumber(L, i)));
    break;
  case LUA_TSTRING:
    return v8::String::New((char *)lua_tostring(L, i));
    break;
  case LUA_TTABLE:
    {
      v8::Local<v8::Object> obj = v8::Object::New();
      lua_pushnil(L);
      while(lua_next(L, i-1) != 0){
	v8::Local<v8::Value> key = lua_to_value(L, -2);
	v8::Local<v8::Value> value = lua_to_value(L, -1);
	obj->Set(key, value);
	lua_pop(L, 1);
      }
      return obj;
      break;
    }
  default:
    return v8::Local<v8::Primitive>::New(v8::Undefined());
    break;
  }
}

void push_value_to_lua(lua_State* L, v8::Handle<v8::Value> value){
  if(value->IsString()){
    lua_pushstring(L, get_str(v8::Local<v8::Value>::New(value)));
  }else if(value->IsNumber()){
    int i_value = value->ToNumber()->Value();
    lua_pushinteger(L, i_value);
  }else if(value->IsBoolean()){
    int b_value = (int)value->ToBoolean()->Value();
    lua_pushboolean(L, b_value);
  }else if(value->IsObject()){
    lua_newtable(L);
    v8::Local<v8::Object> obj = value->ToObject();
    v8::Local<v8::Array> keys = obj->GetPropertyNames();
    for(uint32_t i = 0; i < keys->Length(); ++i){
      v8::Local<v8::String> key = keys->Get(i)->ToString();
      v8::Local<v8::Value> val = obj->Get(key);
      push_value_to_lua(L, key);
      push_value_to_lua(L, val);
      lua_settable(L, -3);
    }
  }else{
    lua_pushnil(L);
  }
}
