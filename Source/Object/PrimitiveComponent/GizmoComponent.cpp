#include "GizmoComponent.h"
#include "Object/Gizmo/GizmoActor.h"


UGizmoComponent::UGizmoComponent()
{
	//SetMaterial(TEXT("AlwaysVisibleMaterial"));
	SetMesh(TEXT("GizmoArrow"), false);
	bCanBeRendered = true;
}

void UGizmoComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGizmoComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UGizmoComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}

void UGizmoComponent::Render()
{
	Super::Render();
}

void UGizmoComponent::OnChangedGizmoType(EGizmoType Gizmo)
{
	switch (Gizmo)
	{
	case EGizmoType::Translate:
		SetMesh(TEXT("GizmoArrow"), false);
		break;
	case EGizmoType::Rotate:
		SetMesh(TEXT("GizmoRotation"), false);
		break;
	case EGizmoType::Scale:
		SetMesh(TEXT("GizmoScale"), false);
		break;
	case EGizmoType::Max:
		break;
	default:
		break;
	}
}
