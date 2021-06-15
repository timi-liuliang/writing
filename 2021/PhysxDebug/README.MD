# Physx 可视化调试
PhysX 是一款由 [Nvidia](https://developer.nvidia.com/gameworks-physx-overview) 开发的实时开源物理引擎，著名的游戏引擎如 Unity3D、Unreal Engine (Version 3 and 4) 都整合了Phsyx 用于底层物理模拟。

## 1. 源码下载
Physx 当前最新版本是 4.1, 可以直接在Github上Clone下载   
[NVIDIAGameWorks/Physx](https://github.com/NVIDIAGameWorks/PhysX)

## 2.调试
当我们完成了PhysX的编译、链接、初始化、刚体创建、场景模拟等等步骤后，要如何观察当前物理场景的模拟结果呢?   
目前常见的方式有两种
1. [Physx Visual Debugger](https://developer.nvidia.com/physx-visual-debugger) 通过外部工具可视化物理场景。
2. [Debug Visualization](https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/DebugVisualization.html) 在引擎中直接渲染物理场景。

### 2.1 PhysX Visual Debugger
Physx Visual Debugger (PVD) 是官方提供的一款简单易用的Physx可视化工具，
通过使用 Physx Visual Debugger (PVD), 我们可以观察、调试、交互应用程序的物理场景。   
[下载](https://developer.nvidia.com/physx-visual-debugger)   
![](pvd_introduction.jpg)

#### 2.1.1 编译选项
Physx在编译时必须开启 PX_SUPPORT_PVD 宏，才能支持 PVD 的调试。在游戏发布时可以关闭该宏。   
[NpScene.cpp, Line[1849-1942]](https://github.com/NVIDIAGameWorks/PhysX/blob/93c6dd21b545605185f2febc8eeacebe49a99479/physx/source/physx/src/NpScene.cpp)
```cpp
void NpScene::simulateOrCollide(PxReal elapsedTime, physx::PxBaseTask* completionTask...)
{
    ...
	
#if PX_SUPPORT_PVD		
		//signal the frame is starting.	
		mScene.getScenePvdClient().frameStart(elapsedTime);
#endif

    ...

#if PX_SUPPORT_PVD
		mScene.getScenePvdClient().updateJoints();			
#endif

    ...
}
```

#### 2.1.2 PVD Connect

#### 2.1.3 Pvd Disconnect

## 参考
[1]. [NVidia](https://github.com/NVIDIAGameWorks/PhysX). [NVIDIA PhysX SDK 4.1](https://houdinitricks.com/creating-attributes-with-python-in-houdini/) (2021)