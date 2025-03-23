#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include "Primitive/PrimitiveVertices.h"
#include "Core/Container/Array.h"
#include "Core/Math/Matrix.h"

struct FLineConstantInfo
{
	FMatrix ViewProjectionMatrix;
};

class FRenderResourceCollection;
class FLineBatchManager
{
public:
	// 라인 추가 함수
	void AddLine(const FVector& Start, const FVector& End, const  FVector4& Color = FVector4{1.0f,1.0f,1.0f,1.0f}, float Thickness = 1.0f);

	// 배치 처리 함수
	void Flush();

	void MakeWorldGrid(float GridSize, float GridSpacing, const FVector4& GridColor = FVector4{1.0f,1.0f,1.0f,1.0f }, bool bCenterGrid = true);

	FRenderResourceCollection& GetRenderResourceCollection() { return *RenderResourceCollection; }
private:
	TArray<FLineVertexSimple> VertexBuffer;
	TArray<uint32> IndexBuffer;


	// 최대 용량
	UINT MaxVerticesPerBatch = 10000;
	UINT MaxIndicesPerBatch = MaxVerticesPerBatch * 2;


	// 선택적 디버그/시각화 플래그
	bool bShowDebugLines = true;
	bool bShowWorldGrid = true;
	float GridSize = 100.0f;
	float GridSpacing = 10.0f;
	FVector4 GridColor = { 0.3f, 0.3f, 0.3f, 1.0f };


	

	//추후 렌더러로 분리

public:
	void Create();
	void Render();

private:

	// 라인 렌더링을 위한 특수 상태
	ID3D11RasterizerState* LineRasterizerState = nullptr; // 종종 다른 컬링 방식 필요

	
	FLineConstantInfo LineConstantInfo;
	
	std::unique_ptr<FRenderResourceCollection> RenderResourceCollection;
};


