## UE4 Commandlet 初探

### 概述
在基于UE4的游戏的开发过程中，有很多工作需要以命令行传参的形式去执行。最常见的如  
[Content Cooking](https://docs.unrealengine.com/en-US/Engine/Deployment/Cooking/index.html)
等等。

### 调用方式
commandlet 通过 -run=${commandletname} options 的方式来执行，这里以cook为例,有两种调用方式。
```bat
UE4Editor.exe <GameName or uproject> -run=cook -targetplatform=<Plat1>+<Plat2> [-cookonthefly] [-iterate] [-map=<Map1>+<Map2>]
```
或者
```bat
UE4Editor-Cmd.exe <GameName> -run=cook -targetplatform=<Plat1>+<Plat2> [-cookonthefly] [-iterate] [-map=<Map1>+<Map2>]
```
其中cook在C++中对应的类为UCookCommandlet
### 调试
1.在visual studio 2017中，修改Command Arguments即可


###

### 参考
[1] UE4.[CommandletPlugin](https://github.com/ue4plugins/CommandletPlugin). How to implement a commandlet  
[2] UE4.[UCommandlets](http://api.unrealengine.com/INT/API/Runtime/Engine/Commandlets/UCommandlet/index.html). All Commandlets in UE4  
[3] UE4.[An Introduction to UE4 Plugins](https://wiki.unrealengine.com/An_Introduction_to_UE4_Plugins)   
