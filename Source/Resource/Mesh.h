#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "DirectResource/Shader.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Debug/DebugConsole.h"

class UMesh : public UResource<UMesh>
{
public:
	UMesh() = default;

	static std::shared_ptr<UMesh> Create(const FString& InName, D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		return Create(InName, InName, InName, Topology);
	}

	static std::shared_ptr<UMesh> Create(
		const FString& InName, const FString& VertexName, const FString& IndexName
		, D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	)
	{
		std::shared_ptr<UMesh> Res = CreateRes(InName);
		Res->VertexBuffer = UVertexBuffer::Find(VertexName);
		Res->IndexBuffer = UIndexBuffer::Find(IndexName);
		Res->Topology = Topology;

		if (nullptr == Res->VertexBuffer
			|| nullptr == Res->IndexBuffer)
		{
			MsgBoxAssert("매쉬를 만드는데 실패했습니다.");
		}

		return Res;
	}

	static std::shared_ptr<UMesh> Create(
		const FString& InName, const std::shared_ptr<UVertexBuffer>& InVertex, const std::shared_ptr<UIndexBuffer>& InIndex,
		D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	)
	{
		std::shared_ptr<UMesh> Res = CreateRes(InName);
		Res->VertexBuffer = InVertex;
		Res->IndexBuffer = InIndex;
		Res->Topology = Topology;

		if (nullptr == Res->VertexBuffer
			|| nullptr == Res->IndexBuffer)
		{
			MsgBoxAssert("매쉬를 만드는데 실패했습니다.");
		}

		return Res;
	}

	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY InTopology)
	{
		Topology = InTopology;
	}

	void Setting();
	void Draw();

	std::shared_ptr<UVertexBuffer> GetVertexBuffer()
	{
		return VertexBuffer;
	}

	std::shared_ptr<UIndexBuffer> GetIndexBuffer()
	{
		return IndexBuffer;
	}
	
private:
	std::shared_ptr<UVertexBuffer> VertexBuffer = nullptr;
	std::shared_ptr<UIndexBuffer> IndexBuffer = nullptr;
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
