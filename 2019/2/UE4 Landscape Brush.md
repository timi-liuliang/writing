# 虚幻地形笔刷
### 今天我们要讨论的主角(Brush)就是下图的这位。

### 首先，我们通过Nsight捕帧分析，确认笔刷的渲染方式   
显然，UE4地形笔刷是通过再次绘制地形块的方式执行笔刷的渲染。

### 通过分析 FLandscapeBrushCircle_Smooth 类实现，可以发现笔刷的实现主要有两个关键点。

#### 1. 设置地形块笔刷材质
```cpp
// Set brush material for components in new region
TSet<ULandscapeComponent*> AddedComponents = NewComponents.Difference(BrushMaterialComponents);
for (ULandscapeComponent* AddedComponent : AddedComponents)
{
	AddedComponent->EditToolRenderData.ToolMaterial = UMaterialInstanceDynamic::Create(BrushMaterial, nullptr);
	AddedComponent->EditToolRenderData.ToolMaterial->AddToRoot();
	AddedComponent->UpdateEditToolRenderData();
}
```

#### 2. 设置 GLandscapeEditModeActive 为 true  
主要是因为LandscapeEditor 模块，其Runtime代码逻辑依赖Editor实现。存在很多如下代码：
```cpp
```

参考：  
[Applying Decals to Arbitrary Surfaces] Game Programming Gems2. 4.8
[Decals] https://docs.unrealengine.com/en-us/Resources/ContentExamples/Decals