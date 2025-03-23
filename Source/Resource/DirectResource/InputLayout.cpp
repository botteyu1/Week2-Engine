#include "InputLayout.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Core/Rendering/FDevice.h"
#include "Debug/DebugConsole.h"


//D3D11_INPUT_ELEMENT_DESC UInputLayout:: LayoutDesc[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//	};

UInputLayout::~UInputLayout()
{
	if (nullptr != LayOut)
	{
		LayOut->Release();
		LayOut = nullptr;
	}
}

void UInputLayout::ResCreate(
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& InDesc,
	UVertexShader* InShader
)
{
	//const std::vector<D3D11_INPUT_ELEMENT_DESC>& Infos = _Buffer->VertexInfoPtr->Infos;
	LayoutDesc = InDesc;
	OutputDebugString(std::to_wstring(sizeof(D3D11_INPUT_ELEMENT_DESC) * LayoutDesc.size()).c_str());
	HRESULT hr = FDevice::Get().GetDevice()->CreateInputLayout(
		LayoutDesc.data(),
		LayoutDesc.size(),
		InShader->BinaryCode->GetBufferPointer(),
		InShader->BinaryCode->GetBufferSize(),
		&LayOut);

	if (hr != S_OK || nullptr == LayOut)
	{
		MsgBoxAssert("Error: FInputLayout Create Failed") ;
	}
}

void UInputLayout::ResCreate(
	UVertexBuffer* InBuffer,
	UVertexShader* InShader
) {
	//const std::vector<D3D11_INPUT_ELEMENT_DESC>& Infos = _Buffer->VertexInfoPtr->Infos;
	LayoutDesc = InBuffer->GetLayout()->LayoutDesc;
	OutputDebugString(std::to_wstring(sizeof(D3D11_INPUT_ELEMENT_DESC) * LayoutDesc.size()).c_str());
	HRESULT hr = FDevice::Get().GetDevice()->CreateInputLayout(
		LayoutDesc.data(),
		LayoutDesc.size(),
		InShader->BinaryCode->GetBufferPointer(),
		InShader->BinaryCode->GetBufferSize(),
		&LayOut);

	if ( hr != S_OK || nullptr == LayOut ) {
		MsgBoxAssert("Error: FInputLayout Create Failed");
	}
}

void UInputLayout::Setting(ERenderFlags)
{
	if (nullptr == LayOut)
	{
		MsgBoxAssert("Error: FInputLayout Create Failed") ;
	}

	// 버텍스버퍼를 여러개 넣어줄수 있다.
	FDevice::Get().GetDeviceContext()->IASetInputLayout(LayOut);
}
