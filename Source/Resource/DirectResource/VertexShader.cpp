#include "VertexShader.h"

#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"

#include "Debug/DebugConsole.h"


UVertexShader::UVertexShader()
{
}

UVertexShader::~UVertexShader()
{
		if (nullptr != ShaderPtr)
	{
		ShaderPtr->Release();
		ShaderPtr = nullptr;
	}
}

void UVertexShader::Setting()
{

	if (nullptr == ShaderPtr)
	{
		MsgBoxAssert("Error: FVertexShader Setting Failed");
	}
	
	// 버텍스버퍼를 여러개 넣어줄수 있다.
	FDevice::Get().GetDeviceContext()->VSSetShader(ShaderPtr, nullptr, 0);
}

void UVertexShader::ShaderLoad(const LPCWSTR& _Path, const FString& _EntryPoint, UINT _VersionHight, UINT _VersionLow)
{	// std::string*
	ID3DBlob* Error = nullptr;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

	SetShaderType(ShaderType::Vertex);
	
	D3DCompileFromFile(_Path, nullptr, nullptr, *_EntryPoint, "vs_5_0", shaderFlags, 0, &BinaryCode, &Error);
	FDevice::Get().GetDevice()->CreateVertexShader(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), nullptr, &ShaderPtr);

}
