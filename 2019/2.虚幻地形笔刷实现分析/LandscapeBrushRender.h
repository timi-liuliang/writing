#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Materials/MaterialInterface.h"

class ALandscapeProxy;
class ULandscapeComponent;
class FLandscapeBrushRender : public FGCObject
{
public:
	FLandscapeBrushRender(UMaterialInterface* InBrushMaterial);
	virtual ~FLandscapeBrushRender();

	// Create
	static FLandscapeBrushRender* Create();

	// Tick
	void Tick(FEditorViewportClient* ViewportClient, float DeltaTime, float BrushRadius, float BrushFalloff);

	// On Clikc
	void OnClick(ALandscapeProxy* InHitLandscape, const FVector& InClickPosition);

	// Garbage Collect
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
	// Clear components
	void ClearBrushComponents();

protected:
	// Brush Components
	TSet<ULandscapeComponent*> BrushMaterialComponents;

	// Material
	UMaterialInterface* BrushMaterial = nullptr;

	// Landscape and Click Position
	ALandscapeProxy* LandscapeProxy = nullptr;
	FVector			 LastMousePosition;
};