# Lua "require" "dofile" “loadfile” 对比
### 前言
在最近的开发工作中，因为对require的误解，产生了一些误用。特记录在案，方便查询交流。

lua 模块是由变量、函数等已知元素组成的 table，因此创建一个模块就是创建一个 table。下面我们创建一个 enemy 模块供测试使用。
```lua
-- enemy.lua
-- create module table
local Enemy = {}

-- public variable
Enemy.life = 100

-- new function (shallow copy)
function Enemy:new()
  local copy = {}
  for k, v in pairs(self) do
    copy[k] = v
  end
  
  return copy;
end

function Enemy:onAttack()
  self.life = self.life - 1
end

return Enemy;
```

## loadfile
loadfile 会加载编译对应的文件，并返回一个封装了文件内容的函数. 但不会执行此函数。

loadfile 返回值类型为 function, 而此函数是否有返回值，则完合依赖于enemy.lua的写法。
```lua
-- file : main.lua
local fun = loadfile("enemy.lua")

-- it should out a function
print(type(fun))

-- call 
local Enemy = fun()
print(Enemy)
```
输出：
```  
Program 'lua53.exe' (pid: 6760).
function
table: 006b9488
Program completed in 0.07 seconds (pid: 6760).
```

## dofile
dofile 是对 loadfile的封装，其不仅加载编译了lua文件，而且执行了loadfile返回的函数。类似于
```lua
-- dofile pseudo code
function dofile(file)
    local fun = loadfile(file)
    return fun()  
end
```

## require
#### require 类似于 dofile, 其不同点在于， require的返回值会被缓存，多次require 返回的结果值是一样的，而每次dofile 都会返回新的结果。
```lua
local enemy1 = require("enemy")
local enemy2 = require("enemy")
local enemy3 = require("enemy")

-- enemy1、enemy2、enemy3 are equal
print(enemy1)
print(enemy2)
print(enemy3)

print("---------------------------")

local enemy4 = dofile("enemy.lua")
local enemy5 = dofile("enemy.lua")
local enemy6 = dofile("enemy.lua")

-- enemy4、enemy5、enemy6 are different
print(enemy4)
print(enemy5)
print(enemy6)
```
输出
```
Program 'lua53.exe' (pid: 11824).
table: 00e891b8
table: 00e891b8
table: 00e891b8
---------------------------
table: 011d0fd8
table: 011d10f0
table: 011d1230
Program completed in 0.05 seconds (pid: 11824).
```

#### require 函数更希望我们把 require的结果当做模板来使用，不要直接修改require的返回值。而是通过拷贝产生新的对像(function Enemy:new())。
```lua
local enemy1 = require("enemy"):new()
local enemy2 = require("enemy"):new()

print(enemy1)
print(enemy2)
```
输出
```
Program 'lua53.exe' (pid: 17156).
table: 008696b8
table: 008695a0
Program completed in 0.11 seconds (pid: 17156).
```

#### require 要达到和dofile一值的结果，比较粗暴的做法直接清空缓存。其缓存位于表package.loaded中
```lua
local enemy1 = require("enemy")
package.loaded["enemy"] = nil

local enemy2 = require("enemy")
package.loaded["enemy"] = nil

local enemy3 = require("enemy")
package.loaded["enemy"] = nil

-- enemy1、enemy2、enemy3 are equals
print(enemy1)
print(enemy2)
print(enemy3)
```
输出
```
Program 'lua53.exe' (pid: 6420).
table: 006e9708
table: 007d2518
table: 007d24c8
Program completed in 0.05 seconds (pid: 6420).
```

然而此种做法与dofile一样，存在的问题就是文件会被多次编译。更好的做法是缓存loadfile的结果，并多次执行。

## 参考
[1]. [Lua模块与包](https://www.runoob.com/lua/lua-modules-packages.html)
