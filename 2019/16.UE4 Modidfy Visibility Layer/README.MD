# UE4 地形挖洞数据的导入与导出

Landscape主要由 高度、Layer(混合比)、材质等数据组成。此文我们关注如何在地形上挖洞。

## 一.编辑器挖洞操作
1.打开UE4编辑器
2.进入Landscape EdMode 面板
3.选择 Sculpt/Visibility工具
4.在ViewPort视口中使用Brush修改地形

## 二.程序视角
同地形的混合权重一样，地形挖洞修改的也是LayerInfoObject数据。只是挖洞使用了一个编辑器模式下独有的静态全局 ULandscapeLayerInfoObject.  
所有的挖洞操作都是通过操作VisibilityLayer实现的 
![](https://github.com/timi-liuliang/writing/blob/master/2019/16.UE4%20Modidfy%20Visibility%20Layer/Visibility.png?raw=true)

### 1.基础函数
为了执行VisiblityLayer数据的导入与导出，我们需要一些辅助函数   
1.首先，是纹理存储函数
```cpp
template<typename T>
void SaveImageInternal(const FString& FilePathName, int32 Width, int32 Height, const TArray<T>& RawData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	// Set Raw Data
	ImageWrapper->SetRaw(RawData.GetData(), RawData.Num() * sizeof(T), Width, Height, ERGBFormat::Gray, sizeof(T) * 8);

	// Save
	const TArray<uint8>& TempData = ImageWrapper->GetCompressed();
	FFileHelper::SaveArrayToFile(TempData, *FilePathName);
}


void SaveImage(const FString& FilePathName, int32 Width, int32 Height, const TArray<uint8>& RawData)
{
	SaveImageInternal<uint8>(FilePathName, Width, Height, RawData);
}
```
2.其次我们需要获取地形的范围，从而可以确定导出纹理大小
```cpp
bool GetLandscapeExtent(int32& MinX, int32& MinY, int32& MaxX, int32& MaxY, const TArray<ULandscapeComponent*>& Components)
{
	MinX = MAX_int32;
	MinY = MAX_int32;
	MaxX = MIN_int32;
	MaxY = MIN_int32;

	// Find range of entire landscape
	for (const ULandscapeComponent* Comp : Components)
	{
		Comp->GetComponentExtent(MinX, MinY, MaxX, MaxY);
	}

	return (MinX != MAX_int32);
}
```
3.最后，是ULandscapeLayerInfoObject的获取与设置函数
```cpp
bool GetLandscapeLayerData(int32& MinX, int32& MinY, int32& MaxX, int32& MaxY, TArray<uint8>& LayerData, ALandscapeProxy* InLandscape, ULandscapeLayerInfoObject* LayerInfoObject)
{
	if (LayerInfoObject)
	{
		MinX = MAX_int32;
		MinY = MAX_int32;
		MaxX = -MAX_int32;
		MaxY = -MAX_int32;
		if (GetLandscapeExtent(MinX, MinY, MaxX, MaxY, InLandscape->LandscapeComponents))
		{
			FLandscapeEditDataInterface LandscapeEdit(InLandscape->GetLandscapeInfo());
			LayerData.AddZeroed((MaxX - MinX + 1) * (MaxY - MinY + 1));
			LandscapeEdit.GetWeightDataFast(LayerInfoObject, MinX, MinY, MaxX, MaxY, LayerData.GetData(), 0);

			return true;
		}
	}

	return false;
}

void ImportLayer(ALandscapeProxy* LandscapeActor, ULandscapeLayerInfoObject* LayerInfoObject, const TArray<uint8>& NewData)
{
	ULandscapeInfo* LandscapeInfo = LandscapeActor->GetLandscapeInfo();
	if (LayerInfoObject)
	{
		int32 MinX = MAX_int32;
		int32 MinY = MAX_int32;
		int32 MaxX = -MAX_int32;
		int32 MaxY = -MAX_int32;
		TArray<uint8> OrigData;
		if (GetLandscapeLayerData(MinX, MinY, MaxX, MaxY, OrigData, LandscapeActor, LayerInfoObject))
		{
			if (OrigData.Num() == NewData.Num())
			{
				FLandscapeEditDataInterface LandscapeEdit(LandscapeInfo);
				LandscapeEdit.SetAlphaData(InfoObject, MinX, MinY, MaxX, MaxY, NewData.GetData(), 0, ELandscapeLayerPaintingRestriction::None, true, false);

				// Invalid Component Data
				UpdateLandscapeCollisionAndNavigation(MinX, MinY, MaxX, MaxY, LandscapeInfo);

				InvalidComponents(LandscapeActor->LandscapeComponents);
			}
		}
	}
}
```

### 1.数据导出
1. 通过 FLandscapeEditDataInterface 的 GetWeightDataFast接口，获取VisibilityLayer的数据(格式：TArray<uint8>)。
2. 存储为深度为8的png格式
```cpp

bool ExportVisibilityLayer(ALandscapeProxy* LandscapeActor, const FString& LayerPathName)
{
	if (LandscapeActor)
	{
		ULandscapeInfo* LandscapeInfo = LandscapeActor->GetLandscapeInfo();
		ULandscapeLayerInfoObject* InfoObject = ALandscapeProxy::VisibilityLayer;
		if (InfoObject)
		{
			int32 MinX = MAX_int32;
			int32 MinY = MAX_int32;
			int32 MaxX = -MAX_int32;
			int32 MaxY = -MAX_int32;
			TArray<uint8> LayerData;
			if (GetLandscapeLayerData(MinX, MinY, MaxX, MaxY, LayerData, LandscapeActor, InfoObject))
			{
				SaveImage(LayerPathName, MaxX - MinX + 1, MaxY - MinY + 1, LayerData);

				return true;
			}
		}
	}

	return false;
}

```

### 2.数据导入
1.通过FLandscapeEditDataInterface的SetAlphaData接口可以修改VisibilityData的数据
```cpp
```
数据修改后一定要Invalid所有的地形Components, 更新物理，寻路以及LandscapeGrass 等数据
```cpp

```


## 参考
1. Epic Games (1998-2019) .[ Landscape Outdoor Terrain](https://docs.unrealengine.com/en-US/Engine/Landscape/index.html)
