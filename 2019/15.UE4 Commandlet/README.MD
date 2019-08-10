## UE4 Commandlet 初探

### 概述
在基于UE4的游戏的开发过程中，有很多工作需要以命令行传参的形式去执行。最常见的如   
[Content Cooking](https://docs.unrealengine.com/en-US/Engine/Deployment/Cooking/index.html)、
[CompileAllBluePrints](https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Editor/UnrealEd/Public/Commandlets/CompileAllBlueprintsCommandlet.h)、
[ImportAssets](https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Editor/UnrealEd/Public/Commandlets/ImportAssetsCommandlet.h) 等等。

### 快速入门
我们可以通过学习官方的[CommandletPlugin](https://github.com/ue4plugins/CommandletPlugin)插件快速入门Commandlet的编写。   
其核心内容是，实现一个继承自UCommandlet的类，并覆写其 Main 函数。
```cpp
UCLASS()
class UHelloWorldCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UHelloWorldCommandlet();

public:
	//~ UCommandlet interface
	virtual int32 Main(const FString& Params) override;
};
```
CommandletPlguin中的其它代码与内容都是服务于UE4的插件机制，与Commandlet机制无关
### 注意事项
##### 1.源码位置
UCommandlet的调用代码主要位于FEngineLoop::PreInit函数中
![Source Code](https://github.com/timi-liuliang/writing/blob/master/2019/15.UE4%20Commandlet/commandlet_source_code.png?raw=true)
##### 2.名称关联  
UE4通过类名称查找对应的Commandlet类，不区分大小写。例：
HelloWorld 对应 UHelloWorldCommandlet
```cpp
// 添加 "Commandlet" 后缀
if (Token.StartsWith(TEXT("run=")))
{
  Token = Token.RightChop(4);
  if (!Token.EndsWith(TEXT("Commandlet")))
  {
    Token += TEXT("Commandlet");
  }
}
```
```cpp
// 根据名称查找对应的Commandlet类
UClass* CommandletClass = FindObject<UClass>(ANY_PACKAGE,*Token,false);
```
##### 3.启用插件
插件编译后，默认未被启用;切记启用插件后再调用Commandlet, 否则会导致查找Commandlet失败。
##### 4.加载顺序  
由于Commandlet的调用位于 FEngineLoop::PreInit() 函数中，则此Commandlet所在插件的加载顺序特别重要。
以[CommandletPlugin.uplugin](https://github.com/ue4plugins/CommandletPlugin/blob/master/CommandletPlugin.uplugin) 为例，其 LoadingPhase 默认值为
```ini
"LoadingPhase" : "Default"
```
太晚加载插件会导致无法找到 UCommandlet类，无法正常执行 Commandlet。

更多LoadingPhase值请参考 [ELoadingPhase::Type](https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Projects/Public/ModuleDescriptor.h) 枚举。

##### 5.配置  
UCommandlet的配置会影响调用流程，不合理的配置会导致程序崩溃。
```cpp
UHelloWorldCommandlet::UHelloWorldCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = false;
}
```

### 调用方式
commandlet 通过 -run=${commandletname} options 的方式来执行，有两种调用方式。
```bat
UE4Editor-Win64-Debug.exe <GameName or uproject> -run=HelloWorld
```
或者
```bat
UE4Editor-Win64-Debug-Cmd.exe <GameName> -run=HelloWorld
```
### 调试
1.在visual studio 2017中，修改Command Arguments即可
![](https://github.com/timi-liuliang/writing/blob/master/2019/15.UE4%20Commandlet/debug_commandlet.png?raw=true)

### 参考
[1] UE4.[CommandletPlugin](https://github.com/ue4plugins/CommandletPlugin). How to implement a commandlet  
[2] UE4.[UCommandlets](http://api.unrealengine.com/INT/API/Runtime/Engine/Commandlets/UCommandlet/index.html). All Commandlets in UE4  
[3] UE4.[An Introduction to UE4 Plugins](https://wiki.unrealengine.com/An_Introduction_to_UE4_Plugins)   
