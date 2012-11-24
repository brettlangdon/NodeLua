{
  "targets": [
    {
      "target_name": "nodelua",
      "sources": [ 
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