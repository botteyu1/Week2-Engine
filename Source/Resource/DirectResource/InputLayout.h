#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include <memory>

#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Resource/RenderResourceCollection.h"


class UInputLayout:
public UResource<UInputLayout> 
{
public:
	UInputLayout() = default;
	~UInputLayout() override;
	
	static D3D11_INPUT_ELEMENT_DESC LayoutDesc[];


	//인풋 레이아웃에는 INPUT_ELEMENT_DESC와 버텍스 쉐이더가 필요하나 일단 인풋 정보는 고정값
	static std::shared_ptr<UInputLayout> Create(const FString&  _Name, std::shared_ptr<class UVertexShader> _Shader)
	{
		std::shared_ptr<UInputLayout> Res = UInputLayout::CreateRes(_Name);
		Res->ResCreate(_Shader);
		return Res;
	}

	void ResCreate(
	std::shared_ptr<UVertexShader> _Shader
);

	void Setting(ERenderFlags);
	
private:
	
	
	ID3D11InputLayout* LayOut = nullptr;
	
};
