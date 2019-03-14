# 虚幻地形笔刷实现分析
#### 今天我们要讨论的主角是地形笔刷。
![](https://raw.githubusercontent.com/timi-liuliang/writing/master/2019/2/Brush.png)  

#### 首先，我们通过Nsight捕帧分析，确认笔刷的渲染方式   
显然，UE4地形笔刷是通过再次绘制地形块的方式执行笔刷的渲染。
![](https://raw.githubusercontent.com/timi-liuliang/writing/master/2019/2/BrushCapture.png)  

#### 通过分析 FLandscapeBrushCircle_Smooth 类实现，可以发现笔刷的实现主要有两个关键点。

##### 1. 设置地形块笔刷材质
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

##### 2. 实时更新材质参数  
```cpp
for (ULandscapeComponent* Component : BrushMaterialComponents)
{
	UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(Component->EditToolRenderData.ToolMaterial);
	MaterialInstance->SetScalarParameterValue(FName(TEXT("LocalRadius")), Radius);
	MaterialInstance->SetScalarParameterValue(FName(TEXT("LocalFalloff")), Falloff);
	MaterialInstance->SetVectorParameterValue(FName(TEXT("WorldPosition")), FLinearColor(WorldLocation.X, WorldLocation.Y, WorldLocation.Z, ScaleXY));
	MaterialInstance->SetScalarParameterValue("CanPaint", 1.0f);
}
```

##### 3. 设置 GLandscapeEditModeActive 为 true  
![](https://raw.githubusercontent.com/timi-liuliang/writing/master/2019/2/GLandscapeEditModeActive.png)  
图 3. LandscapeEditor 模块，其Runtime逻辑依赖Editor状态。

#### 附件


参考：  
[Applying Decals to Arbitrary Surfaces] Game Programming Gems2. 4.8   
[Decals] https://docs.unrealengine.com/en-us/Resources/ContentExamples/Decals   