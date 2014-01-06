{
  "targets": [
    {
      "target_name": "nodelua",
      "variables": {
        "lua_include": "<!(find /usr/include /usr/local/include $NODELUA_INCLUDE -name lua.h | sed s/lua.h//)"
        },
      "sources": [
        "src/utils.cc",
        "src/luastate.cc",
	"src/nodelua.cc"
	],
      "include_dirs": [
        "<@(lua_include)",
        ],
      "libraries": [
        "<!(echo $NODELUA_FLAGS)",
        "<!(pkg-config --libs-only-l --silence-errors lua || pkg-config --libs-only-l --silence-errors lua5.1 || echo '')",
        "-ldl"
	]
    }
  ]
}
