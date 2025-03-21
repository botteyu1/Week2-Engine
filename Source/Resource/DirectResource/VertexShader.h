#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Shader.h"
#include "Core/hal/PlatformType.h"



class UVertexShader :
	public UResource<UVertexShader> , public FShader
{
	friend class UInputLayout;
	
public:
	UVertexShader();
	~UVertexShader();
	
	static std::shared_ptr<UVertexShader> Load(const LPCWSTR& _Path, const FString& _Name, const FString&  _EntryPoint, UINT _VersionHight = 5, UINT _VersionLow = 0)
	{
		std::shared_ptr<UVertexShader> Res = UVertexShader::CreateRes(_Name);
		Res->ShaderLoad(_Path, _EntryPoint, _VersionHight, _VersionLow);
		return Res;
	}

	
	void Setting();
	
private:
	ID3D11VertexShader* ShaderPtr = nullptr;
	
	void ShaderLoad(const LPCWSTR&  _Path, const FString&  _EntryPoint, UINT _VersionHight = 5, UINT _VersionLow = 0);
};
