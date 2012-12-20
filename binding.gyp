{
  "targets": [
    {
      "target_name": "nodelua",
      "variables": {
        "lua_include": "<!(find /usr/include /usr/local/include -name lua.h | sed s/lua.h//)",
	"lua_version": "<!(lua -v 2>&1 | grep -oP '\d\.\d')"
        },
      "sources": [
      		 "src/utils.cc",
		 "src/luafunction.cc",
        "src/luaobject.cc",
	"src/nodelua.cc"
	],
      "include_dirs": [
        "<@(lua_include)",
        ],
      "libraries": [
        "<!(pkg-config --libs-only-l --silence-errors lua || pkg-config --libs-only-l --silence-errors lua5.1)",
        "-ldl"
	]
    }
  ]
}
