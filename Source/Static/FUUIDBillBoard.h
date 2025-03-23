#pragma once

#include "Object/Actor/Actor.h"
#include "Object/USceneComponent.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Container/Array.h"
#include "Debug/EngineShowFlags.h"

class UMesh;
class UDepthStencilState;
class FShaderResourceBinding;
class FConstantBufferBinding;
class URasterizer;
class UBlendState;

struct FFontConstantInfo
{
	FMatrix ViewProjectionMatrix;
};

class UFontAtlasAsset;

class FUUIDBillBoard
{
public:
	void Create();
	void Render();
	void SetTarget(AActor* Target);
	void UpdateString(const std::wstring& String);
	void SetFontAtlas(const FString& name);
private:
	void CalculateModelMatrix(FMatrix& OutMatrix);
	void Flush();
private:
	USceneComponent* TargetObject;

	TArray<FVertexSimple> VertexBuffer;
	TArray<uint32> IndexBuffer;

	// 최대 용량
	UINT MaxVerticesPerBatch = 256;
	UINT MaxIndicesPerBatch = 384;

	bool bShowDebugLines = true;
	bool bShowWorldGrid = true;

	UFontAtlasAsset* FontAtlas;
	FString ResourceName;

	float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};