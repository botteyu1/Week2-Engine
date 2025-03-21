#include "FLineBatchManager.h"
#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"
#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"

#include "Object/Actor/Camera.h"
#include "Object/World/World.h"

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
#include "Resource/RenderResourceCollection.h"


void FLineBatchManager::AddLine(const FVector& Start, const FVector& End, const FVector4& Color, float Thickness)
{
	// 버텍스 버퍼에 두 정점 추가


	uint32 Index = VertexBuffer.Num();


	//TO DO 자유롭게 라인 크기 늘리게 하기
	if (Index + 2 >= MaxVerticesPerBatch)
	{
		UE_LOG("Max Line");
		return;
	}

	VertexBuffer.Add({ (Start), Color });
	VertexBuffer.Add({ (End) , Color });

	// 인덱스 버퍼에 두 인덱스 추가
	IndexBuffer.Add(Index);
	IndexBuffer.Add(Index + 1);

	// 버퍼가 가득 차서 플러시가 필요한지 확인
	if (VertexBuffer.Num() >= MaxVerticesPerBatch)
	{
		Flush();
	}
}

void FLineBatchManager::Flush()
{
}

void FLineBatchManager::DrawWorldGrid(float GridSize, float GridSpacing, const FVector4& GridColor, bool bCenterGrid)
{

	VertexBuffer.Empty();
	IndexBuffer.Empty();


	VertexBuffer.Reserve(MaxVerticesPerBatch);
	IndexBuffer.Reserve(MaxIndicesPerBatch);

	// 그리드 크기 및 라인 개수 계산
	int32 LineCount = FMath::CeilToInt(GridSize / GridSpacing) + 1;
	float HalfGridSize = GridSize * 0.5f;
	float StartOffset = bCenterGrid ? -HalfGridSize : 0.0f;
	float EndOffset = bCenterGrid ? HalfGridSize : GridSize;

	// X축(전방/후방) 라인 추가 (X-Y 평면, Z=0)
	for (int32 i = 0; i < LineCount; ++i)
	{
		float Position = StartOffset + i * GridSpacing;
		// Y 위치를 변경하면서 X축을 따라 그리기
		FVector StartPoint(StartOffset, Position, 0.0f); // X-Y 평면
		FVector EndPoint(EndOffset, Position, 0.0f);     // X-Y 평면

		// Y=0인 라인은 X축이므로 강조
		FVector4 LineColor = (FMath::Abs(Position) < KINDA_SMALL_NUMBER)
			? FVector4::RED // X축 강조
			: GridColor;

		AddLine(StartPoint, EndPoint, LineColor);
	}

	// Y축(좌/우) 라인 추가 (X-Y 평면, Z=0)
	for (int32 i = 0; i < LineCount; ++i)
	{
		float Position = StartOffset + i * GridSpacing;
		// X 위치를 변경하면서 Y축을 따라 그리기
		FVector StartPoint(Position, StartOffset, 0.0f); // X-Y 평면
		FVector EndPoint(Position, EndOffset, 0.0f);     // X-Y 평면

		// X=0인 라인은 Y축이므로 강조
		FVector4 LineColor = (FMath::Abs(Position) < KINDA_SMALL_NUMBER)
			? FVector4::GREEN // Y축 강조
			: GridColor;

		AddLine(StartPoint, EndPoint, LineColor);
	}

	// 원점 강조와 Z축 표시
	if (bCenterGrid)
	{
		// Z축 표시 (위쪽 방향)
		AddLine(FVector(0, 0, 0), FVector(0, 0, GridSpacing * 10.0f), FVector4::BLUE);

		// 원점에 작은 마커 추가 (옵션)
		float MarkerSize = GridSpacing * 0.25f;
		AddLine(FVector(-MarkerSize, 0, 0), FVector(MarkerSize, 0, 0), FVector4::MAGENTA);
		AddLine(FVector(0, -MarkerSize, 0), FVector(0, MarkerSize, 0), FVector4::CYAN);
	}
}

void FLineBatchManager::Render()
{
	//return;
	if (VertexBuffer.Num() == 0)
		return;

	LineConstantInfo.ViewProjectionMatrix = FMatrix::Transpose(UEngine::Get().GetWorld()->GetCamera()->GetViewProjectionMatrix());


	RenderResourceCollection.Render(ERenderFlags::None);

}

void FLineBatchManager::Create()
{
	
	FVertexBuffer::Create("LineVertexBuffer", VertexBuffer , true);
	FIndexBuffer::Create("LineIndexBuffer", IndexBuffer , true);
	std::shared_ptr<FMesh> Mesh =  FMesh::Create("LineBatchMesh" , "LineVertexBuffer", "LineIndexBuffer", D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	FVertexShader::Load(L"Shaders\\ShaderLine_VS.hlsl", "ShaderLine_VS", "ShaderLine_VS");
	
	FPixelShader::Load(L"Shaders\\ShaderLine_PS.hlsl", "ShaderLine_PS", "ShaderLine_PS");
	

	// 라인 렌더링을 위한 래스터라이저 상태 설정
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; // 라인은 양면을 볼 수 있도록 컬링 없음
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = TRUE; // 안티앨리어싱된 라인 활성화

	
	FRasterizer::Create("LineRasterizerState", rasterizerDesc);

	
	std::shared_ptr<FMaterial> Material = FMaterial::Create("LineBatchMaterial");
	Material->SetVertexShader("ShaderLine_VS");
	Material->SetPixelShader("ShaderLine_PS");
	Material->SetRasterizer("LineRasterizerState");
	

	RenderResourceCollection.SetConstantBufferBinding("LineConstantInfo",&LineConstantInfo, 1,true,false);

	RenderResourceCollection.SetMesh(Mesh);
	RenderResourceCollection.SetMaterial(Material);
}


