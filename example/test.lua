print("Calling JS Function 'test_func' From Lua");
print(nodelua("test_func", 3, 5))

global_var = 'this is a global variable from lua'

print("Global Var 'test' From JS: " .. test)