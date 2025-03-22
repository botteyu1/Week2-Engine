#include "GizmoHandle.h"

#include "Object/Actor/Camera.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/World/World.h"
#include "Static/EditorManager.h"


AGizmoHandle::AGizmoHandle()
{
	// !NOTE : Z방향으로 서있음
	// 
	
	// z

	RootComponent = AddComponent<USceneComponent>();

	UCylinderComp* ZArrow = AddComponent<UCylinderComp>();
	ZArrow->SetupAttachment(RootComponent);
	ZArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(0.1f, 0.1f, 1.0f)));
	ZArrow->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	//CylinderComponents.Add(ZArrow);

	// x
	UCylinderComp* XArrow = AddComponent<UCylinderComp>();
	XArrow->SetupAttachment(RootComponent);
	XArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 90, 0.0f), FVector(1.0f, 0.1f, 0.1f)));
	XArrow->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
	//CylinderComponents.Add(XArrow);


	// y
	UCylinderComp* YArrow = AddComponent<UCylinderComp>();
	YArrow->SetupAttachment(RootComponent);
	YArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90, 0.0f, 0.0f), FVector(0.1f, 1.0f, 0.1f)));
	YArrow->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
	//CylinderComponents.Add(YArrow);


	//RootComponent->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 45.0f, 0.0f), FVector(1, 1, 1)));
	
	//UEngine::Get().GetWorld()->AddRenderComponent(ZArrow);
	//UEngine::Get().GetWorld()->AddRenderComponent(XArrow);
	//UEngine::Get().GetWorld()->AddRenderComponent(YArrow);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(XArrow);
	UEngine::Get().GetWorld()->AddZIgnoreComponent(YArrow);

	SetActive(false);  
}	

void AGizmoHandle::BeginPlay()
{
	Super::BeginPlay();

}

void AGizmoHandle::Tick(float DeltaTime)
{

	SetScaleByDistance();
	
	AActor::Tick(DeltaTime);

	if (SelectedAxis != ESelectedAxis::None)
	{
		if (AActor* Actor = UEngine::Get().GetEditor()->GetSelectedActor())
		{
			// 마우스의 커서 위치를 가져오기
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);
			
			RECT Rect;
			GetClientRect(UEngine::Get().GetWindowHandle(), &Rect);
			int ScreenWidth = Rect.right - Rect.left;
			int ScreenHeight = Rect.bottom - Rect.top;
			
			// 커서 위치를 NDC로 변경
			float PosX = 2.0f * pt.x / ScreenWidth - 1.0f;
			float PosY = -2.0f * pt.y / ScreenHeight + 1.0f;
			
			// Projection 공간으로 변환
			FVector4 RayOrigin {PosX, PosY, 0.0f, 1.0f};
			FVector4 RayEnd {PosX, PosY, 1.0f, 1.0f};
			
			// View 공간으로 변환
			FMatrix InvProjMat = UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft)->GetProjectionMatrix().Inverse();
			RayOrigin = InvProjMat.TransformVector4(RayOrigin);
			RayOrigin.W = 1;
			RayEnd = InvProjMat.TransformVector4(RayEnd);
			RayEnd *= UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft)->GetFar();  // 프러스텀의 Far 값이 적용이 안돼서 수동으로 곱함
			RayEnd.W = 1;
			
			// 마우스 포인터의 월드 위치와 방향
			FMatrix InvViewMat = UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft)->GetViewMatrix().Inverse();
			RayOrigin = InvViewMat.TransformVector4(RayOrigin);
			RayOrigin /= RayOrigin.W = 1;
			RayEnd = InvViewMat.TransformVector4(RayEnd);
			RayEnd /= RayEnd.W = 1;
			FVector RayDir = (RayEnd - RayOrigin).GetSafeNormal();
	
			// 액터와의 거리
			float Distance = FVector::Distance(RayOrigin, Actor->GetActorTransform().GetPosition());
			
			// Ray 방향으로 Distance만큼 재계산


			FVector Result = RayDir * Distance;
			Result += RayOrigin;

			// 액터의 월드 위
			FTransform AT = Actor->GetActorTransform();

			DoTransform(AT, Result, Actor);
			
		}
	}
}

void AGizmoHandle::SetScaleByDistance()
{
	FTransform MyTransform = GetActorTransform();
	
	// 액터의 월드 위치
	FVector actorWorldPos = MyTransform.GetPosition();

	FTransform CameraTransform = UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft)->GetActorTransform();
	
	// 카메라의 월드 위치
	FVector cameraWorldPos = CameraTransform.GetPosition();

	// 거리 계산
	float distance = (actorWorldPos - cameraWorldPos).Length();

	float baseScale = 1.0f;    // 기본 스케일
	float scaleFactor = distance * 0.1f; // 거리에 비례하여 스케일 증가

	// float minScale = 1.0f;     // 최소 스케일
	// float maxScale = 1.0f;     // 최대 스케일
	// float scaleFactor = clamp(1.0f / distance, minScale, maxScale);

	MyTransform.SetScale(scaleFactor, scaleFactor, scaleFactor);
	// SetActorTransform(MyTransform);
}

void AGizmoHandle::SetActive(bool bActive)
{
	/*bIsActive = bActive;
	for (auto& Cylinder : CylinderComponents)
	{
		Cylinder->SetCanBeRendered(bActive);
	}*/
}

const char* AGizmoHandle::GetTypeName()
{
	return "GizmoHandle";
}

void AGizmoHandle::DoTransform(FTransform& AT, FVector Result, AActor* Actor )
{
 //	const FVector& AP = AT.GetPosition();

 //	if (SelectedAxis == ESelectedAxis::X)
 //	{
 //		switch (GizmoType)
 //		{
 //		case EGizmoType::Translate:
 //			AT.SetPosition({ Result.X, AP.Y, AP.Z });
 //			break;
 //		case EGizmoType::Rotate:
 //			AT.RotatePitch(Result.X);
 //			break;
 //		case EGizmoType::Scale:
 //			AT.AddScale({ Result.X * .1f, 0, AP.Z * .1f });
 //			break;
 //		}
 //	}
 //	else if (SelectedAxis == ESelectedAxis::Y)
 //	{
 //		switch (GizmoType)
 //		{
 //		case EGizmoType::Translate:
 //			AT.SetPosition({ AP.X, Result.Y, AP.Z });
 //			break;
 //		case EGizmoType::Rotate:
 //			AT.RotateRoll(Result.Y);
 //			break;
 //		case EGizmoType::Scale:
 //			AT.AddScale({ 0, Result.Y * .1f, 0 });
 //			break;
 //		}
 //	}
 //	else if (SelectedAxis == ESelectedAxis::Z)
 //	{
 //		switch (GizmoType)
 //		{
 //		case EGizmoType::Translate:
 //			AT.SetPosition({ AP.X, AP.Y, Result.Z });
 //			break;
 //		case EGizmoType::Rotate:
 //			AT.RotatePitch(-Result.Z);
 //			break;
 //		case EGizmoType::Scale:
 //			AT.AddScale({0, 0, Result.Z * .1f });
 //			break;
 //		}
 //	}
 //	Actor->SetActorTransform(AT);
	//const FVector& AP = AT.GetPosition();

	//if (SelectedAxis == ESelectedAxis::X)
	//{
	//	switch (GizmoType)
	//	{
	//	case EGizmoType::Translate:
	//		AT.SetPosition({ Result.X, AP.Y, AP.Z });
	//		break;
	//	case EGizmoType::Rotate:
	//		AT.RotatePitch(Result.X);
	//		break;
	//	case EGizmoType::Scale:
	//		AT.AddScale({ Result.X * .1f, 0, AP.Z * .1f });
	//		break;
	//	}
	//}
	//else if (SelectedAxis == ESelectedAxis::Y)
	//{
	//	switch (GizmoType)
	//	{
	//	case EGizmoType::Translate:
	//		AT.SetPosition({ AP.X, Result.Y, AP.Z });
	//		break;
	//	case EGizmoType::Rotate:
	//		AT.RotateRoll(Result.Y);
	//		break;
	//	case EGizmoType::Scale:
	//		AT.AddScale({ 0, Result.Y * .1f, 0 });
	//		break;
	//	}
	//}
	//else if (SelectedAxis == ESelectedAxis::Z)
	//{
	//	switch (GizmoType)
	//	{
	//	case EGizmoType::Translate:
	//		AT.SetPosition({ AP.X, AP.Y, Result.Z });
	//		break;
	//	case EGizmoType::Rotate:
	//		AT.RotatePitch(-Result.Z);
	//		break;
	//	case EGizmoType::Scale:
	//		AT.AddScale({0, 0, Result.Z * .1f });
	//		break;
	//	}
	//}
	//Actor->SetActorTransform(AT);
}

