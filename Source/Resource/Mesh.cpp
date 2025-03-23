#include "Mesh.h"
#include "Core/Engine.h"
#include "Core/Utils/Utils.h"
#include "Core/Rendering/FDevice.h"
#include "Resource/DirectResource/InputLayout.h"


void UMesh::Setting(ERenderFlags renderFlags)
{
	if (nullptr == VertexBuffer)
	{
		MsgBoxAssert("매쉬가 세팅되어 있지 않습니다.");
		return;
	}
	VertexBuffer->Setting();
	VertexBuffer->GetLayout()->Setting(renderFlags);

	FDevice::Get().GetDeviceContext()->IASetPrimitiveTopology(Topology);

	if (nullptr == IndexBuffer)
	{
		MsgBoxAssert("매쉬가 세팅되어 있지 않습니다.");
		return;
	}
	IndexBuffer->Setting();
}

void UMesh::Draw()
{
	
	FDevice::Get().GetDeviceContext()->DrawIndexed(IndexBuffer->GetIndexCount(), 0, 0);
}

