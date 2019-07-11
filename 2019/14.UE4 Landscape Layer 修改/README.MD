# UE4 Landscape Layer 数据修改

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