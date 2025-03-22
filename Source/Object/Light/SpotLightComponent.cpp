#include "SpotLightComponent.h"
#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"
#include "Object/World/World.h"
#include "Object/Actor/Camera.h"
#include "Primitive/UGeometryGenerator.h"
#include "Resource/Mesh.h"
#include "Resource/DirectResource/InputLayout.h"

USpotLightComponent::USpotLightComponent()
	: InnerConeAngle(PI / 6.0f)  // 30도
	, OuterConeAngle(PI / 4.0f)  // 45도
	, ConeHeight(AttenuationRadius)
{
	// 스포트라이트 시각화를 위한 콘 메시 생성
	CreateLightVisualizationMesh();

	// 기본 색상 설정
	SetLightColor(FVector4(1.0f, 0.8f, 0.2f, 1.0f));

	// 기본 강도 설정e
	SetIntensity(1.0f);

	//UGeometryGenerator::CreateRadialCone(ConeHeight, OuterConeAngle, 16, &VertexBuffer, &IndexBuffer);


	UVertexBuffer::Create(
		"SpotLightGuideVertexBuffer", 
		VertexBuffer, 
		UInputLayout::Find("Simple_IL"),
		true
	);
	UIndexBuffer::Create("SpotLightGuideIndexBuffer", IndexBuffer, true);
	std::shared_ptr<UMesh> Mesh = UMesh::Create("SpotLightGuideMesh", "SpotLightGuideVertexBuffer", "SpotLightGuideIndexBuffer", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//GuideMesh->SetMesh("SpotLightGuideMesh");
	//GuideMesh->SetMaterial("DebugMaterial");
	GetRenderResourceCollection().SetConstantBufferBinding("FConstantsComponentData", &ConstantsComponentData, 0, true, false);
	GetRenderResourceCollection().SetMesh(Mesh);
	GetRenderResourceCollection().SetMaterial("DebugMaterial");
}

USpotLightComponent::~USpotLightComponent()
{
}

void USpotLightComponent::BeginPlay()
{
	ULightComponent::BeginPlay();
}

void USpotLightComponent::Tick(float DeltaTime)
{
	ULightComponent::Tick(DeltaTime);

	// 조명 위치 및 방향 업데이트
	SpotLightData.Position = FVector4(GetComponentLocation(), 1.0f);
	SpotLightData.Direction = FVector4(GetForwardVector(), 0.0f);
	SpotLightData.Color = LightColor * Intensity;
	SpotLightData.InnerConeAngle = InnerConeAngle;
	SpotLightData.OuterConeAngle = OuterConeAngle;
	SpotLightData.AttenuationRadius = AttenuationRadius;
	SpotLightData.Intensity = Intensity;
	SpotLightData.CastShadows = bCastShadows ? 1 : 0;

	Render();
}

void USpotLightComponent::Render()
{
	if (!bLightEnabled)
		return;

	ULightComponent::Render();

	// 스포트라이트 데이터를 셰이더에 전달하는 코드
	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (Renderer)
	{
		// 스포트라이트 데이터를 상수 버퍼에 업데이트
		// 이 부분은 렌더러 구현에 따라 달라질 수 있음
		FMatrix ModelMatrix;
		ModelMatrix = GetWorldTransform().GetMatrix();

		const FMatrix& ViewProjectionMatrix = UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft)->GetViewProjectionMatrix();

		FMatrix MVP = FMatrix::Transpose(
			ModelMatrix *
			ViewProjectionMatrix
		);

		FConstantsComponentData& Data = GetConstantsComponentData();

		Data.MVP = MVP;
		Data.bUseVertexColor = true;

		GetRenderResourceCollection().Render(ERenderFlags::None);
		//GuideMesh->Render();
	}
}

void USpotLightComponent::SetAttenuationRadius(float InRadius)
{
	AttenuationRadius = InRadius;
	ConeHeight = InRadius;
	// 시각화 메시 업데이트
	UpdateLightVisualization();
}

void USpotLightComponent::SetInnerConeAngle(float InAngle)
{
	InnerConeAngle = InAngle;

	// 내부 각도는 외부 각도보다 클 수 없음
	if (InnerConeAngle > OuterConeAngle)
	{
		OuterConeAngle = InnerConeAngle;
	}

	// 시각화 메시 업데이트
	UpdateLightVisualization();
}

void USpotLightComponent::SetOuterConeAngle(float InAngle)
{
	OuterConeAngle = InAngle;

	// 외부 각도는 내부 각도보다 작을 수 없음
	if (OuterConeAngle < InnerConeAngle)
	{
		InnerConeAngle = OuterConeAngle;
	}

	// 시각화 메시 업데이트
	UpdateLightVisualization();
}

void USpotLightComponent::UpdateLightVisualization()
{
	// 시각화 메시 업데이트
	CreateLightVisualizationMesh();
}

void USpotLightComponent::CreateLightVisualizationMesh()
{
	// 스포트라이트 시각화를 위한 콘 메시 생성
	VertexBuffer.Empty();
	IndexBuffer.Empty();
	UGeometryGenerator::CreateRadialCone(ConeHeight, OuterConeAngle, 16, &VertexBuffer, &IndexBuffer);
}

FBoxSphereBounds USpotLightComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	// 원뿔 바닥 반지름 계산
	float radius = ConeHeight * tanf(OuterConeAngle);

	// 원뿔 중심점 (원뿔 높이의 절반 위치)
	FVector center = FVector(0, 0, ConeHeight / 2.0f);

	// 원뿔을 포함하는 구의 반지름 계산
	float sphereRadius = FMath::Sqrt(radius * radius + ConeHeight * ConeHeight);

	// 로컬 바운드 생성
	FBoxSphereBounds localBounds(center, FVector(radius, radius, ConeHeight), sphereRadius);

	// 월드 변환 적용
	return localBounds.TransformBy(LocalToWorld);
}
