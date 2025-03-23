#pragma once
#define _TCHAR_DEFINED  // MYTCHAR 재정의 에러 때문
#include <d3dcommon.h>
#include "Core/Container/String.h"


enum class ShaderType
{
	None = 0, 
	Vertex, 
	Hull,
	Tessellator,
	Domain,
	Geometry,
	Pixel, 
	Compute,
	Max
};

class FShader
{
public:

	// constrcuter destructer
	FShader() = default;
	~FShader()= default;

	// delete Function
	FShader(const FShader& _Other) = delete;
	FShader(FShader&& _Other) noexcept = delete;
	FShader& operator=(const FShader& _Other) = delete;
	FShader& operator=(FShader&& _Other) noexcept = delete;

	ShaderType GetShaderType() 
	{
		return ShaderTypeValue;
	}
	
	void SetShaderType(ShaderType _ShaderTypeValue) 
	{
		 ShaderTypeValue = _ShaderTypeValue;
	}
	

protected:
	// 쉐이더의 바이너리 코드에 대한 포인터
	ID3DBlob* BinaryCode = nullptr;
	FString EntryName = "";
	
private:
	ShaderType ShaderTypeValue = ShaderType::None;
};
