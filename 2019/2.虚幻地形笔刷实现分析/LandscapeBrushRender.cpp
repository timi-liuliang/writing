#include "LandscapeBrushRender.h"
#include "Materials/MaterialInstanceConstant.h"
#include "EditorViewportClient.h"
#include "Landscape/Classes/LandscapeInfo.h"
#include "Landscape/Classes/LandscapeProxy.h"
#include "Landscape/Classes/LandscapeComponent.h"
#include "Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Landscape/Classes/LandscapeMaterialInstanceConstant.h"
#include "LandscapeRender.h"

static UMaterialInstance* CreateMaterialInstance(UMaterialInterface* BaseMaterial)
{
	ULandscapeMaterialInstanceConstant* MaterialInstance = NewObject<ULandscapeMaterialInstanceConstant>(GetTransientPackage());
	MaterialInstance->bEditorToolUsage = true;
	MaterialInstance->SetParentEditorOnly(BaseMaterial);
	MaterialInstance->PostEditChange();
	return MaterialInstance;
}

FLandscapeBrushRender::FLandscapeBrushRender(UMaterialInterface* InBrushMaterial)
	: BrushMaterial(CreateMaterialInstance(InBrushMaterial))
{
}

FLandscapeBrushRender::~FLandscapeBrushRender()
{
	ClearBrushComponents();
}

// Garbage Collect
void FLandscapeBrushRender::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(BrushMaterial);
}

FLandscapeBrushRender* FLandscapeBrushRender::Create()
{
	UMaterialInstanceConstant* CircleBrushMaterialSmooth = LoadObject<UMaterialInstanceConstant>(nullptr, TEXT("/Engine/EditorLandscapeResources/CircleBrushMaterial_Smooth.CircleBrushMaterial_Smooth"), nullptr, LOAD_None, nullptr);
	return new FPCGBrushRender(CircleBrushMaterialSmooth);
}

void FLandscapeBrushRender::OnClick(ALandscapeProxy* InHitLandscape, const FVector& InClickPosition)
{
	if (LandscapeProxy != InHitLandscape)
	{
		ClearBrushComponents();
		LandscapeProxy = InHitLandscape;
	}

	if(LandscapeProxy)
	{	
		LastMousePosition = LandscapeProxy->LandscapeActorToWorld().InverseTransformPosition(InClickPosition);
	}
}

void FLandscapeBrushRender::Tick(FEditorViewportClient* ViewportClient, float DeltaTime, float BrushRadius, float BrushFalloff)
{
	if (LandscapeProxy)
	{
		ULandscapeInfo* LandscapeInfo = LandscapeProxy->GetLandscapeInfo();
		ALandscapeProxy* Proxy = LandscapeInfo->GetLandscapeProxy();

		const float ScaleXY = FMath::Abs(LandscapeInfo->DrawScale.X);
		const float TotalRadius = BrushRadius / ScaleXY;
		const float Radius = (1.0f - BrushFalloff) * TotalRadius;
		const float Falloff = BrushFalloff * TotalRadius;

		FIntRect Bounds;
		Bounds.Min.X = FMath::FloorToInt(LastMousePosition.X - TotalRadius);
		Bounds.Min.Y = FMath::FloorToInt(LastMousePosition.Y - TotalRadius);
		Bounds.Max.X = FMath::CeilToInt(LastMousePosition.X + TotalRadius);
		Bounds.Max.Y = FMath::CeilToInt(LastMousePosition.Y + TotalRadius);

		// GetComponentsInRegion expects an inclusive max
		TSet<ULandscapeComponent*> NewComponents;
		LandscapeInfo->GetComponentsInRegion(Bounds.Min.X, Bounds.Min.Y, Bounds.Max.X - 1, Bounds.Max.Y - 1, NewComponents);

		// Remove the material from any old components that are no longer in the region
		TSet<ULandscapeComponent*> RemovedComponents = BrushMaterialComponents.Difference(NewComponents);
		for (ULandscapeComponent* RemovedComponent : RemovedComponents)
		{
			if (RemovedComponent->EditToolRenderData.ToolMaterial)
			{
				RemovedComponent->EditToolRenderData.ToolMaterial->RemoveFromRoot();
				RemovedComponent->EditToolRenderData.ToolMaterial = nullptr;
			}

			RemovedComponent->UpdateEditToolRenderData();
		}

		// Set brush material for components in new region
		TSet<ULandscapeComponent*> AddedComponents = NewComponents.Difference(BrushMaterialComponents);
		for (ULandscapeComponent* AddedComponent : AddedComponents)
		{
			AddedComponent->EditToolRenderData.ToolMaterial = UMaterialInstanceDynamic::Create(BrushMaterial, nullptr);
			AddedComponent->EditToolRenderData.ToolMaterial->AddToRoot();
			AddedComponent->UpdateEditToolRenderData();
		}

		BrushMaterialComponents = MoveTemp(NewComponents);

		// Set params for brush material.
		FVector WorldLocation = Proxy->LandscapeActorToWorld().TransformPosition(FVector(LastMousePosition.X, LastMousePosition.Y, 0));

		for (ULandscapeComponent* Component : BrushMaterialComponents)
		{
			UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(Component->EditToolRenderData.ToolMaterial);
			MaterialInstance->SetScalarParameterValue(FName(TEXT("LocalRadius")), Radius);
			MaterialInstance->SetScalarParameterValue(FName(TEXT("LocalFalloff")), Falloff);
			MaterialInstance->SetVectorParameterValue(FName(TEXT("WorldPosition")), FLinearColor(WorldLocation.X, WorldLocation.Y, WorldLocation.Z, ScaleXY));
			MaterialInstance->SetScalarParameterValue("CanPaint", 1.0f);
		}
	}
}

void FLandscapeBrushRender::ClearBrushComponents()
{
	for (ULandscapeComponent* RemovedComponent : BrushMaterialComponents)
	{
		if (RemovedComponent->EditToolRenderData.ToolMaterial)
		{
			RemovedComponent->EditToolRenderData.ToolMaterial->RemoveFromRoot();
			RemovedComponent->EditToolRenderData.ToolMaterial = nullptr;
		}

		RemovedComponent->UpdateEditToolRenderData();
	}

	BrushMaterialComponents.Empty();
}