#include "Material.h"

#include "Debug/DebugConsole.h"
#include "Resource/DirectResource/BlendState.h"
#include "Resource/DirectResource/DepthStencilState.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Resource/DirectResource/PixelShader.h"
#include "Resource/DirectResource/Rasterizer.h"

UMaterial::UMaterial()
{
	SetRasterizer("DefaultRasterizer");
	SetBlendState("DefaultBlendState");
	SetDepthState("DefaultDepthStencilState");
}

UMaterial::~UMaterial()
{
}

void UMaterial::VertexShader()
{
	if (nullptr == VertexShaderPtr)
	{
		MsgBoxAssert("존재하지 않는 버텍스 쉐이더를 세팅하려고 했습니다.");
	}

	VertexShaderPtr->Setting();
}

void UMaterial::Rasterizer()
{
	if (nullptr == RasterizerPtr)
	{
		MsgBoxAssert("존재하지 않는 레스터 라이저를 세팅하려고 했습니다.");
	}

	RasterizerPtr->Setting();
}

void UMaterial::PixelShader()
{
	if (nullptr == PixelShaderPtr)
	{
		MsgBoxAssert("존재하지 않는 레스터 라이저를 세팅하려고 했습니다.");
	}

	PixelShaderPtr->Setting();
}

void UMaterial::Blend()
{
	if (nullptr == BlendStatePtr)
	{
		MsgBoxAssert("존재하지 않는 레스터 라이저를 세팅하려고 했습니다.");
	}

	BlendStatePtr->Setting();
}

void UMaterial::DepthStencil()
{
	// if (false == IsDepth)
	// {
	// 	GetContext()->OMSetDepthStencilState(nullptr, 0);
	// 	return;
	// }

	if (nullptr == DepthStencilPtr)
	{
		MsgBoxAssert("존재하지 않는 깊이 체크 세팅을 세팅하려고 했습니다.");
	}

	DepthStencilPtr->Setting();
}

void UMaterial::SetVertexShader(const FString& InValue)
{
	VertexShaderPtr = UVertexShader::Find(InValue);

	if (nullptr == VertexShaderPtr)
	{
		MsgBoxAssert("존재하지 않는 버텍스 쉐이더를 세팅하려고 했습니다.");
	}
}

void UMaterial::SetRasterizer(const FString& InValue)
{
	RasterizerPtr = URasterizer::Find(InValue);

	if (nullptr == RasterizerPtr)
	{
		MsgBoxAssert("존재하지 않는 레스터 라이저를 세팅하려고 했습니다.");
		return;
	}
}

void UMaterial::SetPixelShader(const FString& InValue)
{
	PixelShaderPtr = UPixelShader::Find(InValue);

	if (nullptr == PixelShaderPtr)
	{
		MsgBoxAssert("존재하지 않는 픽셀쉐이더를 세팅하려고 했습니다.");
		return;
	}
}

void UMaterial::SetBlendState(const FString& InValue)
{
	BlendStatePtr = UBlendState::Find(InValue);

	if (nullptr == BlendStatePtr)
	{
		MsgBoxAssert("존재하지 않는 블랜드를 세팅하려고 했습니다.");
		return;
	}
}

void UMaterial::SetDepthState(const FString& InValue)
{
	DepthStencilPtr = UDepthStencilState::Find(InValue);

	if (nullptr == DepthStencilPtr)
	{
		MsgBoxAssert("존재하지 않는 깊이버퍼 세팅을 사용하려고 했습니다.");
		return;
	}
}

 void UMaterial::Setting()
{
	VertexShader();
	Rasterizer();
	PixelShader();
	Blend();
	DepthStencil();

}
