#include <node.h>
#include <v8.h>

#include "luaobject.h"

using namespace v8;


void init(Handle<Object> target) {
  LuaObject::Init(target);
}
NODE_MODULE(nodelua, init)
