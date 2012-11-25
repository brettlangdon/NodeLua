{
  "targets": [
    {
      "target_name": "nodelua",
      "sources": [ 
      	"src/utils.cc",
        "src/luaobject.cc",
	"src/nodelua.cc"
	],
      "libraries": [
        "-llua",
	"-ldl"
	]
    }
  ]
}