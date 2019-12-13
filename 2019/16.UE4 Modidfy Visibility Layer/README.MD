# UE4 地形挖洞

Landscape主要由 高度、Layer(混合比)、材质等数据组成。此文我们关注如何在地形上挖洞。

## 一.编辑器挖洞操作
1.打开UE4编辑器
2.进入Landscape EdMode 面板
3.选择 Sculpt/Visibility工具
4.在ViewPort视口中使用Brush修改地形

## 二.程序视角
同地形的混合权重一样，地形挖洞修改的也是LayerInfoObject数据。只是挖洞使用了一个编辑器模式下独有的静态全局 ULandscapeLayerInfoObject.
```cpp
```

## 参考
1. Epic Games (1998-2019) .[ Landscape Outdoor Terrain](https://docs.unrealengine.com/en-US/Engine/Landscape/index.html)
