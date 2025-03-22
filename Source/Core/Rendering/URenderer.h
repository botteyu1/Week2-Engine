#pragma once

#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#include <d3d11.h>

#include "Core/Math/Vector.h"

struct FVertexSimple;
struct FVector4;

class ACamera;

class URenderer
{
public:
	friend class FLineBatchManager;
	
private:
  //   struct alignas(16) FConstantsComponentDatas
  //   {
  //       FMatrix MVP;
  //       FVector4 Color;
		// // true인 경우 Vertex Color를 사용하고, false인 경우 Color를 사용합니다.
  //       uint32 bUseVertexColor;
  //       FVector Padding;
  //   };
	
	// struct alignas(16) FPickingConstants
	// {
	// 	FVector4 UUIDColor;
	// };
	//
	// struct alignas(16) FDepthConstants{
	// 	unsigned int DepthOffset;
	// 	int nearPlane;
	// 	int farPlane;
	// };
public:
	
    /** Renderer를 초기화 합니다. */
    void Create(HWND hWindow);

    /** Renderer에 사용된 모든 리소스를 해제합니다. */
    void Release();

    //void CreateShader();

  //  void CreateConstantBuffer();

    void ReleaseConstantBuffer();

    /** 렌더링 파이프라인을 준비 합니다. */
    // void Prepare() const;

    /** 셰이더를 준비 합니다. */
    //void PrepareShader() const;

	void Render(class FRenderResourceCollection& InRenderResourceCollection);

    /** PrimitiveComponent를 초기화 합니다. */
    // void RenderPrimitiveInternal(UPrimitiveComponent& PrimitiveComp) const;

	void LoadTexture(const wchar_t* texturePath);
	void LoadTextures();
	ID3D11ShaderResourceView* FontTextureSRV = nullptr;
	ID3D11SamplerState* FontSamplerState = nullptr;

    

protected:
    /** 뎁스 스텐실 상태를 생성합니다. */
	//void CreateDepthStencilState();
	

	/** 뎁스 스텐실 리소스는 디바이스 뎁스 설정은 여기서 날린다. */
	//void ReleaseDepthStencilBuffer();
protected:



    //ID3D11Buffer* ConstantBuffer = nullptr;                 // 쉐이더에 데이터를 전달하기 위한 상수 버퍼
	//ID3D11DepthStencilState* DepthStencilState = nullptr;   // DepthStencil 상태(깊이 테스트, 스텐실 테스트 등 정의)

	
    // Shader를 렌더링할 때 사용되는 변수들
	
	ID3D11VertexShader* FontVertexShader = nullptr;       // Vertex 데이터를 처리하는 Vertex 셰이더
	ID3D11PixelShader* FontPixelShader = nullptr;         // Pixel의 색상을 결정하는 Pixel 셰이더

    //ID3D11InputLayout* SimpleInputLayout = nullptr;         // Vertex 셰이더 입력 레이아웃 정의
    unsigned int Stride = 0;                                // Vertex 버퍼의 각 요소 크기

    ID3D11DepthStencilState* GizmoDepthStencilState = nullptr; // 기즈모용 스텐실 스테이트. Z버퍼 테스트 하지않고 항상 앞에렌더
	

	D3D_PRIMITIVE_TOPOLOGY CurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;



	//D3D_PRIMITIVE_TOPOLOGY CurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;


protected:
	// 피킹용 버퍼들
	// ID3D11Texture2D* PickingFrameBuffer = nullptr;                 // 화면 출력용 텍스처
	// ID3D11RenderTargetView* PickingFrameBufferRTV = nullptr;       // 텍스처를 렌더 타겟으로 사용하는 뷰
	//ID3D11Buffer* ConstantPickingBuffer = nullptr;                 // 뷰 상수 버퍼
	
	//ID3D11PixelShader* PickingPixelShader = nullptr;         // Pixel의 색상을 결정하는 Pixel 셰이더
	//ID3D11Buffer* ConstantsDepthBuffer = nullptr;

	//ID3D11DepthStencilState* IgnoreDepthStencilState = nullptr;   // DepthStencil 상태(깊이 테스트, 스텐실 테스트 등 정의)

public:
	//피킹용 함수들	
    // void ReleasePickingFrameBuffer();
    // void CreatePickingTexture(HWND hWnd);
    // void PrepareZIgnore();
    // void PreparePicking();
	// FLOAT PickingClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; //
	// void PreparePickingShader() const;
	// void UpdateConstantPicking(FVector4 UUIDColor) const;
    // void UpdateConstantDepth(int Depth) const;

    // void PrepareMain();

	// FVector4 GetPixel(FVector MPos);

	// void RenderPickingTexture();

};
