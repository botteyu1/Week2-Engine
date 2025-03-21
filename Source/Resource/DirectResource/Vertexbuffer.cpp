#include "Vertexbuffer.h"
#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"
#include "Debug/DebugConsole.h"
#include "d3d11.h"
#include "Core/Rendering/FDevice.h"




void FVertexBuffer::Setting() const
{
	if (nullptr == Buffer)
	{
		UE_LOG("Error: Vertexbuffer Setting Failed");
	}

	if (bIsDynamic == false)
	{
		FDevice::Get().GetDeviceContext()->IASetVertexBuffers(0, 1, &Buffer, &VertexSize, &Offset);
	}
	else
	{
		// 버텍스 버퍼 업데이트
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (Buffer == nullptr)
		{
			MsgBoxAssert("Error: Vertexbuffer Setting Failed");
			return;
		}

		FDevice::Get().GetDeviceContext()->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, CPUDataPtr, VertexSize * VertexCount);
		FDevice::Get().GetDeviceContext()->Unmap(Buffer, 0);

		FDevice::Get().GetDeviceContext()->IASetVertexBuffers(0, 1, &Buffer, &VertexSize, &Offset);
	}
}

void FVertexBuffer::ResCreate(const void* _Data, size_t _VertexSize, size_t _VertexCount)
{
	VertexSize = static_cast<UINT>(_VertexSize);
	VertexCount = static_cast<UINT>(_VertexCount);

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = _Data;

	BufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferInfo.ByteWidth = static_cast<UINT>(VertexSize * VertexCount);

	BufferInfo.CPUAccessFlags = 0;
	BufferInfo.Usage = D3D11_USAGE_DEFAULT;

	if (S_OK != FDevice::Get().GetDevice()->CreateBuffer(&BufferInfo, &Data, &Buffer))
	{
		MsgBoxAssert("Error: Vertexbuffer Create Failed") ;
		return;
	}
}

void FVertexBuffer::ResCreateDynamic(const void* _Data, size_t _VertexSize, size_t _VertexCount)
{

	CPUDataPtr = _Data;
	
	VertexSize = static_cast<UINT>(_VertexSize);
	VertexCount = static_cast<UINT>(_VertexCount);
	
	BufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferInfo.ByteWidth = static_cast<UINT>(VertexSize * _VertexCount);

	BufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	BufferInfo.Usage = D3D11_USAGE_DYNAMIC;

	

	if (S_OK != FDevice::Get().GetDevice()->CreateBuffer(&BufferInfo, nullptr, &Buffer))
	{
		MsgBoxAssert("Error: Vertexbuffer Create Failed") ;
		return;
	}
}
