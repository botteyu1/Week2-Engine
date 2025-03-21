#include "IndexBuffer.h"
#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"
#include "Debug/DebugConsole.h"
#include "d3d11.h"

#include "Core/Rendering/FDevice.h"

void UIndexBuffer::Setting() const
{
	// ID3D11Buffer* Arr[1];

	if (nullptr == Buffer)
	{
		MsgBoxAssert("Error: FIndexBuffer Setting Failed");
	}

	if (bIsDynamic == false)
	{
		// 버텍스버퍼를 여러개 넣어줄수 있다.
		FDevice::Get().GetDeviceContext()->IASetIndexBuffer(Buffer, Format, Offset);
	}
	else
	{
		// 버텍스 버퍼 업데이트
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		// 인덱스 버퍼 업데이트
		FDevice::Get().GetDeviceContext()->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, CPUDataPtr, IndexSize * IndexCount);
		FDevice::Get().GetDeviceContext()->Unmap(Buffer, 0);

		
		FDevice::Get().GetDeviceContext()->IASetIndexBuffer(Buffer, DXGI_FORMAT_R32_UINT, 0);
	}
}

void UIndexBuffer::ResCreate(const void* _Data, size_t _IndexCount)
{
	IndexCount = static_cast<UINT>(_IndexCount);

	BufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferInfo.ByteWidth = static_cast<UINT>(IndexSize * IndexCount);
	BufferInfo.CPUAccessFlags = 0;
	BufferInfo.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = _Data;

	if (S_OK != FDevice::Get().GetDevice()->CreateBuffer(&BufferInfo, &Data, &Buffer))
	{
		MsgBoxAssert("Error: FIndexBuffer Create Failed") ;
		return;
	}
}

void UIndexBuffer::ResCreateDynamic(const void* _Data, size_t _IndexCount)
{

	CPUDataPtr = _Data;
	
	IndexCount = static_cast<UINT>(_IndexCount);

	BufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferInfo.ByteWidth = static_cast<UINT>(IndexSize * IndexCount);
	BufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferInfo.Usage = D3D11_USAGE_DYNAMIC;

	if (S_OK != FDevice::Get().GetDevice()->CreateBuffer(&BufferInfo, nullptr, &Buffer))
	{
		MsgBoxAssert("Error: FIndexBuffer Create Failed") ;
		return;
	}
}
