#include "ConstantBuffer.h"

#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"

#include "Debug/DebugConsole.h"


UConstantBuffer::UConstantBuffer()
{
}

UConstantBuffer::~UConstantBuffer()
{
}

void UConstantBuffer::ChangeData(const void* _Data, UINT _Size)
{
	if (nullptr == _Data)
	{
		MsgBoxAssert("Error: ChangeData Is Null") ;
		return;
	}

	// 크기가 다른
	if (_Size != BufferInfo.ByteWidth)
	{
		MsgBoxAssert("Error: constant buffer was set with data of different sizes.") ;
		return;
	}

	D3D11_MAPPED_SUBRESOURCE Data = {};
	
	FDevice::Get().GetDeviceContext()->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Data);

	// 그래픽카드에 직접 슬수있는 주소값을 담아줍니다.
	// Data

	if (Data.pData == nullptr)
	{
		MsgBoxAssert("Error: not able to obtain the permission to modify the buffer") ;
		return;
	}

	memcpy_s(Data.pData, BufferInfo.ByteWidth, _Data, BufferInfo.ByteWidth);
	
	FDevice::Get().GetDeviceContext()->Unmap(Buffer, 0);
	
}

void UConstantBuffer::VSSetting(UINT _Slot)
{
	if (nullptr == Buffer)
	{
		MsgBoxAssert("Error: FConstantBuffer Setting Failed") ;
	}

	FDevice::Get().GetDeviceContext()->VSSetConstantBuffers(_Slot, 1, &Buffer);
}

void UConstantBuffer::PSSetting(UINT _Slot)
{
	if (nullptr == Buffer)
	{
		MsgBoxAssert("Error: FConstantBuffer Setting Failed") ;
	}

	FDevice::Get().GetDeviceContext()->PSSetConstantBuffers(_Slot, 1, &Buffer);
}

void UConstantBuffer::CSSetting(UINT _Slot)
{
	if (nullptr == Buffer)
	{
		MsgBoxAssert("Error: FConstantBuffer Setting Failed") ;
	}

	FDevice::Get().GetDeviceContext()->CSSetConstantBuffers(_Slot, 1, &Buffer);
}

void UConstantBuffer::GSSetting(UINT _Slot)
{
	if (nullptr == Buffer)
	{
		MsgBoxAssert("Error: FConstantBuffer Setting Failed") ;
	}

	FDevice::Get().GetDeviceContext()->GSSetConstantBuffers(_Slot, 1, &Buffer);
}

void UConstantBuffer::ResCreate(int _ByteSize)
{
	BufferInfo.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferInfo.ByteWidth = _ByteSize;

	// 안바꿈
	BufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	BufferInfo.Usage = D3D11_USAGE_DYNAMIC;


	
	//                                                                 초기화
	if (S_OK != FDevice::Get().GetDevice()->CreateBuffer(&BufferInfo, nullptr, &Buffer))
	{
		MsgBoxAssert("Error: FConstantBuffer Create Failed") ;
		return;
	}
}
