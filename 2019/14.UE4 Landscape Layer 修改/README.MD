# UE4 Landscape 修改

Landscape主要由 高度、混合比、材质等数据组成。此文我们重点讨论如何快速修改高度与混合比。

## 一.地形创建
1.地形创建与创建普通Actor并没有区别，通过SpawnActor方法实现,创建时可以指定 Transform.
```cpp
LandscapeMaskActor = World->SpawnActor<ALandscape>(NewLandscapeLocation, NewLandscapeRotation);
```
2.设置材质
```cpp
FString NewLandscapeMaterialName = TEXT("/Game/Materials/PCG_LandscapeMask.PCG_LandscapeMask");
LandscapeMaterial = LoadObject<UMaterialInterface>(NULL, *NewLandscapeMaterialName, NULL, LOAD_NoWarn);
LandscapeMaskActor->LandscapeMaterial = LandscapeMaterial.Get();
```
3.导入高度、Layer数据, 指定地形范围及Sections设置。
```cpp
LandscapeMaskActor->Import(FGuid::NewGuid(), MinX, MinY, MaxX, MaxY, NumSubsections, SubsectionSizeQuads, HeightData.GetData(), nullptr, ImportLayers, ELandscapeImportAlphamapType::Additive);
```
4.设置LayerObject
只有设置了 ULandscapeLayerInfoObject 后，地形的Layer数据才可以被修改，地形才能正常显示。
ULandscapeLayerInfoObject可以提前创建保存为资源，被多个地形重复使用。
```cpp
void AssignLayerInfoObjects(ALandscapeProxy* Landscape, int32 LayerIndex)
{
	FSoftObjectPath AssetPath(FString::Printf(TEXT("/Game/PCGPipeline/Mask/SpeedGame_Biome_Layer%d_LayerInfo.SpeedGame_Biome_Layer%d_LayerInfo"), LayerIndex, LayerIndex));
	FAssetData AssetData;
	UAssetManager::Get().GetAssetDataForPath(AssetPath, AssetData);
	UObject* Object = AssetData.GetAsset();
	if (Object)
	{
		ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();
		ULandscapeLayerInfoObject* LandscapeInfoObject = const_cast<ULandscapeLayerInfoObject*>(CastChecked<ULandscapeLayerInfoObject>(Object));

		FLandscapeInfoLayerSettings& LayerSettings = LandscapeInfo->Layers[LayerIndex];
		LayerSettings.LayerInfoObj = LandscapeInfoObject;
		LandscapeInfo->CreateLayerEditorSettingsFor(LandscapeInfoObject);
	}
}
```

## 高度修改
#### 1.格式与高度转换
地形高度数据一般又称高度图，每个顶点的高度由一个uint16来表示，UE4高度转换算法把uint16映射到 [-256, 256]. 然后再和 DrawScale 一起决定了地形的最终渲染效果(在平时的高度修改计算中我们并不考虑DrawScale)。   
代码实现可以参考
[LandscapeDataAccess.h](https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Landscape/Public/LandscapeDataAccess.h) 中 GetLocalHeight 函数的实现
```cpp
#define LANDSCAPE_ZSCALE	(1.0f/128.0f)
const int32 MaxValue = 65535;
const float MidValue = 32768.f;
FORCEINLINE float GetLocalHeight(uint16 Height)
{
	return ((float)Height - MidValue) * LANDSCAPE_ZSCALE;
}
```
#### 2.获取高度法线数据
个人一般通过 [FLandscapeEditDataInterface](https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Landscape/Public/LandscapeEdit.h#L111) 类的 GetHeightDataFast 方法来获取高度及法线数据。其中法线数据是可选项，可以不获取。
```cpp
TArray<uint16> HeightData;

FLandscapeEditDataInterface LandscapeEdit(InLandscape->GetLandscapeInfo());
LandscapeEdit.GetHeightDataFast(MinX, MinY, MaxX, MaxY, HeightData.GetData(), 0);
```

MinX, MinY, MaxX, MaxY 定义下获取高度的范围。以 MinX = 0, MinY = 0, MaxX =16, MaxY = 16] 为例,  要获取的范围为 16x16, 共有17x17个顶点，自然最终返回的数据也是17x17共289个高度值。

对于多Level的World, 场景中会存在多个 ALandscapeProxy*， 所有的这些ALandscapeProxy会共用同一个ULandscapeInfo, 如果我们只想获取某一个ALandscapeProxy的高度数据，可以通过其 LandscapeComponents变量获取范围,然后再通过范围获取高度数据。
```cpp
bool GetLandscapeExtent(int32& MinX, int32& MinY, int32& MaxX, int32& MaxY, ALandscapeProxy* InLandscape)
{
	MinX = MAX_int32;
	MinY = MAX_int32;
	MaxX = MIN_int32;
	MaxY = MIN_int32;

	// Find range of entire landscape
	for (const ULandscapeComponent* Comp : InLandscape->LandscapeComponents)
	{
		Comp->GetComponentExtent(MinX, MinY, MaxX, MaxY);
	}

	return (MinX != MAX_int32);
}
```

#### 3.设置高度数据
相对应的，我们通过 FPCGTerrainEditInterface 的 SetHeightData方法来设置高度数据。

FLandscapeEditDataInterface::SetHeightData 方法主要操作有：
1. 查找位于范围内的的所有LandscapeComponents
2. 根据传入的高度数据计算法线，或者通过传入的法线数据更新法线
3. 更新高度数据与法线数据，这里UE4只用一个象素就存储了高度及法线数据，高度因为精度要求为uint16分别占用了RG通道，法线因为是单位向量，所以用BA通道分别存储了法线的X,Y值。因为高度数据与地形位置的X,Z数据是分开保存的，这里可以推测。用于地形绘制的索引及顶点位置的XZ数据应该是所有地形Component共用的，只有一份（侍验证）。
```cpp
	// Update the texture
	TexData.R = Height >> 8;
	TexData.G = Height & 255;

	// Update normals if we're not on an edge vertex
	FVector Normal = VertexNormals[NormalDataIndex].GetSafeNormal();
	TexData.B = FMath::RoundToInt(127.5f * (Normal.X + 1.0f));
	TexData.A = FMath::RoundToInt(127.5f * (Normal.Y + 1.0f));
```


3. 更新包围盒
4. 所有组件创建高度MipMaps(LOD)
5. 更新碰撞数据(物理)，如果碰撞数据修改影响寻路，则更新寻路数据

##### 警告
SetHeightData 函数只修改了非边缘顶点的法线，如果要更新当前范围内所有顶点的法线，必须扩大传入的范围。

#### 获取Layer数据

#### 设置Layer数据

#### 性能优化

代码实现位于文件 LandscapeEdModePaintTools.cpp 中 
FLandscapeToolStrokePaint

FLandscapeEditDataInterface LandscapeEdit;
	// Updates weightmaps for all layers. Data points to packed data for all layers in the landscape info
	void SetAlphaData(const TSet<ULandscapeLayerInfoObject*>& DirtyLayerInfos, const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, const uint8* Data, int32 Stride, ELandscapeLayerPaintingRestriction PaintingRestriction = ELandscapeLayerPaintingRestriction::None);


碰撞数据
    	TArray<FColor*> CollisionWeightmapMipData;
	TArray<FColor*> SimpleCollisionWeightmapMipData;
	TArray<FColor*> WeightmapTextureMipData;

频繁触发 RemovedLayer操作   
LandscapeEditInterface.cpp Line 2952
```cpp
if (bRemovedLayer)
{
	Component->UpdateMaterialInstances();

	Component->EditToolRenderData.UpdateDebugColorMaterial(Component);

	Component->UpdateEditToolRenderData();
}
```

修改前数据
```
TimeProfiler FPCGCookLinker::ApplyLayers+PCGCookLinker.cpp"+108 ,38.537473s, 99.999939%
TimeProfiler     FPCGTerrainSyncHelper::ImportLayers+PCGTerrainSyncHelper.cpp"+300 ,30.530284s, 79.222282%
TimeProfiler     FPCGTerrainSyncHelper::AddLayerWeight+PCGTerrainSyncHelper.cpp"+345 ,0.017465s, 0.045319%
TimeProfiler     FPCGTerrainSyncHelper::AddLayerWeight+PCGTerrainSyncHelper.cpp"+352 ,7.927910s, 20.571939%
TimeProfiler     FPCGTerrainSyncHelper::AddLayerWeight+PCGTerrainSyncHelper.cpp"+359 ,0.000265s, 0.000686%
```

单Level应用一层纹理耗时约等于 8秒,

纹理加载、数据计算、InvalidComponentData 对性能影响较低，主要的耗时函数为
FLandscapeEditDataInterface::SetAlphaData, 占比超过98%
```
FLandscapeEditDataInterface LandscapeEdit(LandscapeInfo);
LandscapeEdit.SetAlphaData(InfoObject, MinX, MinY, MaxX, MaxY, TotalWeight.GetData(), 0, ELandscapeLayerPaintingRestriction::None, true, false);
```

分块后 应用River_Layer， 耗时在1秒以内。 

## 避免使用 ULandscapeComponent::FillLayer
FillLayer会导致其它Layer为空，触发RemovedLayer操作。导致各种数据结构重建。

FPCGTerrainSyncHelper::ImportLayers 函数耗时：
修改前  29.8秒
禁用FillLayer后， 15。26 秒

Tile 大小对效率的影响：
单Level，Layer修改总耗时
8x8     7.996s
16x16   4.373s
32x32   4.634s
64x64	6.828s

地形高度修改触发

1.重新计算包围盒
2.重新计算各顶点法线
使用该顶点的所有面法线相加求平均
```cpp
	FVector* VertexNormals = nullptr;
	if (InCalcNormals)
	{
		// Calculate the normals for each of the two triangles per quad.
		// Note that the normals at the edges are not correct because they include normals
		// from triangles outside the current area. They are not updated
		VertexNormals = new FVector[NumVertsX*NumVertsY];
		FMemory::Memzero(VertexNormals, NumVertsX*NumVertsY * sizeof(FVector));

		// Need to consider XYOffset for XY displacemented map
		FVector2D* XYOffsets = new FVector2D[NumVertsX*NumVertsY];
		FMemory::Memzero(XYOffsets, NumVertsX*NumVertsY * sizeof(FVector2D));
		GetXYOffsetDataFast(X1, Y1, X2, Y2, XYOffsets, 0);

		for (int32 Y = 0; Y < NumVertsY - 1; Y++)
		{
			for (int32 X = 0; X < NumVertsX - 1; X++)
			{
				FVector Vert00 = FVector(XYOffsets[(X + 0) + NumVertsX*(Y + 0)].X, XYOffsets[(X + 0) + NumVertsX*(Y + 0)].Y, ((float)InData[(X + 0) + InStride*(Y + 0)] - 32768.0f) * LANDSCAPE_ZSCALE) * DrawScale;
				FVector Vert01 = FVector(XYOffsets[(X + 0) + NumVertsX*(Y + 0)].X, XYOffsets[(X + 0) + NumVertsX*(Y + 0)].Y + 1.0f, ((float)InData[(X + 0) + InStride*(Y + 1)] - 32768.0f) * LANDSCAPE_ZSCALE) * DrawScale;
				FVector Vert10 = FVector(XYOffsets[(X + 0) + NumVertsX*(Y + 0)].X + 1.0f, XYOffsets[(X + 0) + NumVertsX*(Y + 0)].Y, ((float)InData[(X + 1) + InStride*(Y + 0)] - 32768.0f) * LANDSCAPE_ZSCALE) * DrawScale;
				FVector Vert11 = FVector(XYOffsets[(X + 0) + NumVertsX*(Y + 0)].X + 1.0f, XYOffsets[(X + 0) + NumVertsX*(Y + 0)].Y + 1.0f, ((float)InData[(X + 1) + InStride*(Y + 1)] - 32768.0f) * LANDSCAPE_ZSCALE) * DrawScale;

				FVector FaceNormal1 = ((Vert00 - Vert10) ^ (Vert10 - Vert11)).GetSafeNormal();
				FVector FaceNormal2 = ((Vert11 - Vert01) ^ (Vert01 - Vert00)).GetSafeNormal();

				// contribute to the vertex normals.
				VertexNormals[(X + 1 + NumVertsX*(Y + 0))] += FaceNormal1;
				VertexNormals[(X + 0 + NumVertsX*(Y + 1))] += FaceNormal2;
				VertexNormals[(X + 0 + NumVertsX*(Y + 0))] += FaceNormal1 + FaceNormal2;
				VertexNormals[(X + 1 + NumVertsX*(Y + 1))] += FaceNormal1 + FaceNormal2;
			}
		}

		delete[] XYOffsets;
	}
```
3.更新碰撞数据
4.重新生成LOD数据

#### 侍续

1. 地形LOD的实现细节
2. 高度数据与Layer数据在渲染中的使用细节

