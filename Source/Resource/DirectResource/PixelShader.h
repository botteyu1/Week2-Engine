#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Shader.h"
#include "Core/hal/PlatformType.h"


class UPixelShader :
	public UResource<UPixelShader> , public FShader
{
public:
	UPixelShader();
	~UPixelShader();

	
	static std::shared_ptr<UPixelShader> Load(const LPCWSTR& _Path, const FString& _Name, const FString&  _EntryPoint, UINT _VersionHight = 5, UINT _VersionLow = 0)
	{
		std::shared_ptr<UPixelShader> Res = UPixelShader::CreateRes(_Name);
		Res->ShaderLoad(_Path, _EntryPoint, _VersionHight, _VersionLow);
		return Res;
	}
	
	void Setting();
	
private:
	ID3D11PixelShader* ShaderPtr = nullptr;
	
	void ShaderLoad(const LPCWSTR&  _Path, const FString&  _EntryPoint, UINT _VersionHight = 5, UINT _VersionLow = 0);
};
