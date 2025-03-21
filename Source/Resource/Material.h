#pragma once
#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Resource/DirectResource/Vertexbuffer.h"


class UDepthStencilState;
class URasterizer;
class UBlendState;

class UMaterial : public UResource<UMaterial>
{
public:
	UMaterial();
	virtual ~UMaterial() override;

	static std::shared_ptr<UMaterial> Create(const FString& InName)
	{
		std::shared_ptr<UMaterial> NewRes = CreateRes(InName);
		return NewRes;
	}

	void VertexShader();
	void Rasterizer();
	void PixelShader();
	void Blend();
	void DepthStencil();


	void SetVertexShader(const FString& InValue);
	void SetRasterizer(const FString& InValue);
	void SetPixelShader(const FString& InValue);
	void SetBlendState(const FString& InValue);
	void SetDepthState(const FString& InValue);

	std::shared_ptr<class UVertexShader> GetVertexShader()
	{
		return VertexShaderPtr;
	}

	std::shared_ptr<class UPixelShader> GetPixelShader()
	{
		return PixelShaderPtr;
	}

	void Setting();
	
private:
	std::shared_ptr<UVertexShader> VertexShaderPtr = nullptr;
	std::shared_ptr<UPixelShader> PixelShaderPtr = nullptr;
	std::shared_ptr<URasterizer> RasterizerPtr = nullptr;
	std::shared_ptr<UBlendState> BlendStatePtr = nullptr;
	std::shared_ptr<UDepthStencilState> DepthStencilPtr = nullptr;

};



