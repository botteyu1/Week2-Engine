#pragma once
#include "Core/HAL/PlatformType.h"
#include <d3dcompiler.h>
#include "Object/USceneComponent.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/ConstantBuffer.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Resource/DirectResource/PixelShader.h"
#include "Resource/DirectResource/InputLayout.h"
#include "Resource/DirectResource/BlendState.h"
#include "Resource/DirectResource/DepthStencilState.h"
#include "Resource/DirectResource/Rasterizer.h"
#include "Resource/RenderResourceCollection.h"
#include "Resource/DirectResource/ShaderResourceBinding.h"


struct FSubUVTextureInfo
{
	ID3D11ShaderResourceView* TextureSRV;
	uint32 NumRows;
	uint32 NumColumns;
	uint32 TotalFrames;
	float FrameWidth;
	float FrameHeight;
};

// 버텍스 셰이더용 상수 버퍼
struct FSubUVVertexConstantsData
{
	FMatrix MVP;
};

// 픽셀 셰이더용 상수 버퍼
struct FSubUVPixelConstantsData
{
	FVector4 UVOffsetAndSize;   // xy: 현재 프레임의 UV 오프셋, zw: 프레임 크기
	FVector4 AnimationParams;   // x: 현재 프레임 인덱스, y: 총 프레임 수, z: 블렌딩 팩터, w: 경과 시간
	FVector4 ColorModifier;     // rgb: 색상 조절, a: 투명도
};

class UParticleSubUVComponent : public USceneComponent
{
	DECLARE_CLASS(UParticleSubUVComponent, USceneComponent)

public:
	UParticleSubUVComponent();

	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void CalculateModelMatrix(FMatrix& OutMatrix);
	void Render();

	// SubUV 애니메이션 관련 메서드
	void Play();
	void Pause();
	void Stop();
	void SetPlayRate(float NewRate);
	void SetLooping(bool bShouldLoop);

	// 현재 프레임의 UV 좌표 반환
	FVector4 GetCurrentFrameUV() const;
	FSubUVVertexConstantsData& GetVertexConstantsData() { return VertexConstants; }
	FSubUVPixelConstantsData& GetPixelConstantsData() { return PixelConstants; }
	FRenderResourceCollection& GetRenderResourceCollection() { return RenderResourceCollection; }

private:
	bool bLoop;
	bool bIsPlaying;
	float PlayRate;
	float CurrentTime;
	uint32 CurrentFrame;
	uint32 TotalFrames;

	uint32 NumRows = 16;
	uint32 NumColumns = 16;
	float FrameWidth;
	float FrameHeight;

	FSubUVTextureInfo TextureInfo;
	FRenderResourceCollection RenderResourceCollection;
	FSubUVVertexConstantsData VertexConstants;
    FSubUVPixelConstantsData PixelConstants;

	std::shared_ptr<UVertexBuffer> VertexBuffer = nullptr;
	std::shared_ptr<UIndexBuffer> IndexBuffer = nullptr;
	std::shared_ptr<UInputLayout> InputLayout = nullptr;
	std::shared_ptr<UConstantBuffer> ConstantBuffer = nullptr;
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//class std::shared_ptr<class FPixelShader> PixelShader = nullptr;
	//class std::shared_ptr<class FVertexShader> VertexShader = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	std::shared_ptr<UBlendState> BlendState = nullptr;
	std::shared_ptr<UDepthStencilState> DepthStencilStat = nullptr;
	std::shared_ptr<URasterizer> Rasterizer = nullptr;
};
