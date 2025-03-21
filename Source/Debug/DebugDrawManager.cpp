#include "DebugDrawManager.h"
#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"
#include "Object/World/World.h"
#include "Object/Actor/Camera.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/PixelShader.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Resource/DirectResource/InputLayout.h"
#include "Resource/DirectResource/BlendState.h"
#include "Resource/DirectResource/Rasterizer.h"
#include "Resource/DirectResource/DepthStencilState.h"
#include "Resource/DirectResource/ConstantBuffer.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Core/Math/BoxSphereBounds.h"

using namespace std;

UDebugDrawManager::UDebugDrawManager()
{
	VertexBuffer.SetNum(100);
	IndexBuffer.SetNum(100);
}

UDebugDrawManager::~UDebugDrawManager()
{
	ClearDebug();
}

void UDebugDrawManager::Initialize()
{
	FVertexBuffer::Create(TEXT("DebugVertexBuffer"), VertexBuffer, true);
	FIndexBuffer::Create(TEXT("DebugIndexBuffer"), IndexBuffer, true);
	ClearDebug();

	std::shared_ptr<FMesh> Mesh = FMesh::Create("DebugBatchMesh", "DebugVertexBuffer", "DebugIndexBuffer", D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; // 라인은 양면을 볼 수 있도록 컬링 없음
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = TRUE; // 안티앨리어싱된 라인 활성화

	FRasterizer::Create(TEXT("DebugRasterizerState"), rasterizerDesc);

	std::shared_ptr<FMaterial> Material = FMaterial::Create("DebugBatchMaterial");
	Material->SetVertexShader(TEXT("ShaderLine_VS"));
	Material->SetPixelShader(TEXT("ShaderLine_PS"));
	Material->SetRasterizer(TEXT("DebugRasterizerState"));

	RenderResourceCollection.SetConstantBufferBinding(TEXT("DebugConstantInfo"), &DebugConstantInfo, 1, true, false);

	RenderResourceCollection.SetMesh(Mesh);
	RenderResourceCollection.SetMaterial(Material);
}

void UDebugDrawManager::DrawBoxBrackets(const FBox InActor, const FTransform& LocalToWorld, const FVector4& Color, float LifeTime)
{
}

void UDebugDrawManager::DrawOBBBoxFromLocalMinMax(const FVector& LocalMin, const FVector& LocalMax, const FTransform& LocalToWorld, const FVector4& Color, float LifeTime)
{
	const FTransform WorldTransform = LocalToWorld;
	const FMatrix ModelMatrix = WorldTransform.GetMatrix();
	// 로컬 공간에서 중심과 반 크기(Extent)를 계산
	const FVector LocalCenter = (LocalMin + LocalMax) * 0.5f;
	const FVector LocalExtent = (LocalMax - LocalMin) * 0.5f;

	// 로컬 공간의 8개 정점 계산
	const FVector v0 = LocalCenter + FVector(-LocalExtent.X, -LocalExtent.Y, -LocalExtent.Z);
	const FVector v1 = LocalCenter + FVector(LocalExtent.X, -LocalExtent.Y, -LocalExtent.Z);
	const FVector v2 = LocalCenter + FVector(LocalExtent.X, LocalExtent.Y, -LocalExtent.Z);
	const FVector v3 = LocalCenter + FVector(-LocalExtent.X, LocalExtent.Y, -LocalExtent.Z);
	const FVector v4 = LocalCenter + FVector(-LocalExtent.X, -LocalExtent.Y, LocalExtent.Z);
	const FVector v5 = LocalCenter + FVector(LocalExtent.X, -LocalExtent.Y, LocalExtent.Z);
	const FVector v6 = LocalCenter + FVector(LocalExtent.X, LocalExtent.Y, LocalExtent.Z);
	const FVector v7 = LocalCenter + FVector(-LocalExtent.X, LocalExtent.Y, LocalExtent.Z);

	const FVector WorldV0 = ModelMatrix.TransformVector(v0);
	const FVector WorldV1 = ModelMatrix.TransformVector(v1);
	const FVector WorldV2 = ModelMatrix.TransformVector(v2);
	const FVector WorldV3 = ModelMatrix.TransformVector(v3);
	const FVector WorldV4 = ModelMatrix.TransformVector(v4);
	const FVector WorldV5 = ModelMatrix.TransformVector(v5);
	const FVector WorldV6 = ModelMatrix.TransformVector(v6);
	const FVector WorldV7 = ModelMatrix.TransformVector(v7);

	// 바닥면 (아래 사각형) 선 연결
	DrawLine(WorldV0, WorldV1, Color, LifeTime);
	DrawLine(WorldV1, WorldV2, Color, LifeTime);
	DrawLine(WorldV2, WorldV3, Color, LifeTime);
	DrawLine(WorldV3, WorldV0, Color, LifeTime);

	// 천장면 (위 사각형) 선 연결
	DrawLine(WorldV4, WorldV5, Color, LifeTime);
	DrawLine(WorldV5, WorldV6, Color, LifeTime);
	DrawLine(WorldV6, WorldV7, Color, LifeTime);
	DrawLine(WorldV7, WorldV4, Color, LifeTime);

	// 수직 엣지 연결
	DrawLine(WorldV0, WorldV4, Color, LifeTime);
	DrawLine(WorldV1, WorldV5, Color, LifeTime);
	DrawLine(WorldV2, WorldV6, Color, LifeTime);
	DrawLine(WorldV3, WorldV7, Color, LifeTime);
}

void UDebugDrawManager::DrawBoundingBox(const FVector& LocalMin, const FVector& LocalMax, const FTransform& LocalToWorld, const FVector4& Color, float LifeTime)
{
	const FTransform WorldTransform = LocalToWorld;
	const FMatrix ModelMatrix = WorldTransform.GetMatrix();
	// 로컬 공간에서 중심과 반 크기(Extent)를 계산
	const FVector LocalCenter = (LocalMin + LocalMax) * 0.5f;
	const FVector LocalExtent = (LocalMax - LocalMin) * 0.5f;

	// 로컬 공간의 8개 정점 계산
	const FVector v0 = LocalCenter + FVector(-LocalExtent.X, -LocalExtent.Y, -LocalExtent.Z);
	const FVector v1 = LocalCenter + FVector(LocalExtent.X, -LocalExtent.Y, -LocalExtent.Z);
	const FVector v2 = LocalCenter + FVector(LocalExtent.X, LocalExtent.Y, -LocalExtent.Z);
	const FVector v3 = LocalCenter + FVector(-LocalExtent.X, LocalExtent.Y, -LocalExtent.Z);
	const FVector v4 = LocalCenter + FVector(-LocalExtent.X, -LocalExtent.Y, LocalExtent.Z);
	const FVector v5 = LocalCenter + FVector(LocalExtent.X, -LocalExtent.Y, LocalExtent.Z);
	const FVector v6 = LocalCenter + FVector(LocalExtent.X, LocalExtent.Y, LocalExtent.Z);
	const FVector v7 = LocalCenter + FVector(-LocalExtent.X, LocalExtent.Y, LocalExtent.Z);

	TArray<FVector> vertices;
	const FVector WorldV0 = ModelMatrix.TransformVector4(FVector4(v0, 1));
	const FVector WorldV1 = ModelMatrix.TransformVector4(FVector4(v1, 1));
	const FVector WorldV2 = ModelMatrix.TransformVector4(FVector4(v2, 1));
	const FVector WorldV3 = ModelMatrix.TransformVector4(FVector4(v3, 1));
	const FVector WorldV4 = ModelMatrix.TransformVector4(FVector4(v4, 1));
	const FVector WorldV5 = ModelMatrix.TransformVector4(FVector4(v5, 1));
	const FVector WorldV6 = ModelMatrix.TransformVector4(FVector4(v6, 1));
	const FVector WorldV7 = ModelMatrix.TransformVector4(FVector4(v7, 1));

	FVector NewMin = FVector::ZeroVector;
	FVector NewMax = FVector::ZeroVector;

	vertices.Add(WorldV0);
	vertices.Add(WorldV1);
	vertices.Add(WorldV2);
	vertices.Add(WorldV3);
	vertices.Add(WorldV4);
	vertices.Add(WorldV5);
	vertices.Add(WorldV6);
	vertices.Add(WorldV7);

	FVector::CaculateMinMax(vertices, NewMin, NewMax);

	DrawAABBBox(NewMin, NewMax, FVector4::BLUE, LifeTime);

	// 바닥면 (아래 사각형) 선 연결
	DrawLine(WorldV0, WorldV1, Color, LifeTime);
	DrawLine(WorldV1, WorldV2, Color, LifeTime);
	DrawLine(WorldV2, WorldV3, Color, LifeTime);
	DrawLine(WorldV3, WorldV0, Color, LifeTime);

	// 천장면 (위 사각형) 선 연결
	DrawLine(WorldV4, WorldV5, Color, LifeTime);
	DrawLine(WorldV5, WorldV6, Color, LifeTime);
	DrawLine(WorldV6, WorldV7, Color, LifeTime);
	DrawLine(WorldV7, WorldV4, Color, LifeTime);

	// 수직 엣지 연결
	DrawLine(WorldV0, WorldV4, Color, LifeTime);
	DrawLine(WorldV1, WorldV5, Color, LifeTime);
	DrawLine(WorldV2, WorldV6, Color, LifeTime);
	DrawLine(WorldV3, WorldV7, Color, LifeTime);
}

void UDebugDrawManager::DrawAABBBox(const FVector& InMin, const FVector& InMax, const FVector4& Color, float LifeTime)
{
	// 8개의 정점 계산
	FVector v0 = FVector(InMin.X, InMin.Y, InMin.Z);
	FVector v1 = FVector(InMax.X, InMin.Y, InMin.Z);
	FVector v2 = FVector(InMax.X, InMax.Y, InMin.Z);
	FVector v3 = FVector(InMin.X, InMax.Y, InMin.Z);
	FVector v4 = FVector(InMin.X, InMin.Y, InMax.Z);
	FVector v5 = FVector(InMax.X, InMin.Y, InMax.Z);
	FVector v6 = FVector(InMax.X, InMax.Y, InMax.Z);
	FVector v7 = FVector(InMin.X, InMax.Y, InMax.Z);

	// 바닥면 (아래 사각형) 선 연결
	DrawLine(v0, v1, Color);
	DrawLine(v1, v2, Color);
	DrawLine(v2, v3, Color);
	DrawLine(v3, v0, Color);

	// 천장면 (위 사각형) 선 연결
	DrawLine(v4, v5, Color);
	DrawLine(v5, v6, Color);
	DrawLine(v6, v7, Color);
	DrawLine(v7, v4, Color);

	// 수직 엣지 연결
	DrawLine(v0, v4, Color);
	DrawLine(v1, v5, Color);
	DrawLine(v2, v6, Color);
	DrawLine(v3, v7, Color);
}

void UDebugDrawManager::DrawSphere(const FVector& Center, float Radius, const FVector4& Color, float LifeTime)
{
}

void UDebugDrawManager::DrawBoxSphereBounds(const FVector& Origin, const FVector& BoxExtent, float SphereRadius, const FVector4& Color, float LifeTime)
{

}

void UDebugDrawManager::DrawLine(const FVector& Start, const FVector& End, const FVector4& Color, float LifeTime)
{
	uint32 Index = VertexBuffer.Num();
	
	VertexBuffer.Add({ Start, Color });
	VertexBuffer.Add({ End, Color });

	IndexBuffer.Add(Index);
	IndexBuffer.Add(Index + 1);
}

void UDebugDrawManager::DrawPoint(const FVector& Point, const FVector4& Color, float LifeTime)
{
}

void UDebugDrawManager::Render()
{
	if (VertexBuffer.Num() == 0)
	{
		return;
	}

	//RenderResourceCollection.GetMesh()->GetVertexBuffer()->SetVertexCount(VertexBuffer.Num());
	//RenderResourceCollection.GetMesh()->GetIndexBuffer()->SetIndexCount(IndexBuffer.Num());

	DebugConstantInfo.ViewProjectionMatrix = FMatrix::Transpose(UEngine::Get().GetWorld()->GetCamera()->GetViewProjectionMatrix());
	RenderResourceCollection.Render(ERenderFlags::None);

	ClearDebug();
}

void UDebugDrawManager::ClearDebug()
{
	VertexBuffer.Empty();
	IndexBuffer.Empty();
}
