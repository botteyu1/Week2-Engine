#include "FUUIDBillBoard.h"

#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"

#include "Object/World/World.h"
#include "Object/Actor/Actor.h"
#include "Object/Actor/Camera.h"
#include "Object/Assets/AssetManager.h"
#include "Object/Assets/FontAtlasAsset.h"
#include "Primitive/PrimitiveHangul.h"

#include "Resource/DirectResource/VertexBuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/ConstantBuffer.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Resource/DirectResource/PixelShader.h"
#include "Resource/DirectResource/InputLayout.h"
#include "Resource/Mesh.h"

#include <d3dcompiler.h>


void FUUIDBillBoard::SetFontAtlas(const FString& name) {
	FontAtlas = FontAtlas = UAssetManager::Get().FindAsset<UFontAtlasAsset>(name);
}

void FUUIDBillBoard::UpdateString(const std::wstring& String)
{
	Flush();

	if ( FontAtlas == nullptr ) {
		SetFontAtlas("font_atlas_Pretendard_Kor.fontatlas");
		return;
	}

	uint32 StringLen = static_cast<uint32>(String.size());
	float AspectRatio = FontAtlas->GlyphAspectRatio;
	float Kerning = FontAtlas->Kerning;
	float cursorX = (StringLen - 1) * -AspectRatio * Kerning;

	for (size_t i = 0; i < StringLen; ++i)
	{
		wchar_t c = String[i];

		if (c >= 0xAC00 && c <= 0xD7A3) {
			HangulPrimitive::CreateKoreanQuad(
				c, 
				cursorX, 
				StringLen, 
				FontAtlas,
				VertexBuffer,
				IndexBuffer
			);
		}
		else
		{
			const GlyphInfo& glyph = FontAtlas->GetGlyph(c);

			FVertexSimple vertices[4] =
			{
				{ 0.0f, -AspectRatio + cursorX, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u, glyph.v },
				{ 0.0f, AspectRatio + cursorX, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u + glyph.width, glyph.v },
				{ 0.0f, AspectRatio + cursorX, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u + glyph.width, glyph.v + glyph.height },
				{ 0.0f, -AspectRatio + cursorX, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u, glyph.v + glyph.height }
			};

			for (int j = 0; j < 4; ++j)
			{
				VertexBuffer.Add(vertices[j]);
			}

			uint32 baseIndex = static_cast<uint32>(VertexBuffer.Num()) - 4;
			IndexBuffer.Add(baseIndex + 0);
			IndexBuffer.Add(baseIndex + 1);
			IndexBuffer.Add(baseIndex + 2);
			IndexBuffer.Add(baseIndex + 0);
			IndexBuffer.Add(baseIndex + 2);
			IndexBuffer.Add(baseIndex + 3);

			cursorX += 2 * AspectRatio * Kerning;
		}
	}

	// 버텍스 버퍼 업데이트
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//FDevice::Get().GetDeviceContext()->Map(FontVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//memcpy(mappedResource.pData, VertexBuffer.GetData(), sizeof(FVertexSimple) * VertexBuffer.Num());
	//FDevice::Get().GetDeviceContext()->Unmap(FontVertexBuffer, 0);
	
	// 인덱스 버퍼 업데이트
	//FDevice::Get().GetDeviceContext()->Map(FontIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//memcpy(mappedResource.pData, IndexBuffer.GetData(), sizeof(uint32) * IndexBuffer.Num());
	//FDevice::Get().GetDeviceContext()->Unmap(FontIndexBuffer, 0);

	//UVertexBuffer::Find(ResourceName)->ChangeData<FVertexSimple>(VertexBuffer);
	//UIndexBuffer::Find(ResourceName)->ChangeData(IndexBuffer);
}

void FUUIDBillBoard::Flush()
{
	VertexBuffer = {};
	IndexBuffer = {};
}

void FUUIDBillBoard::CalculateModelMatrix(FMatrix& OutMatrix)
{
	ACamera* cam = UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft);

	FVector cameraPosition = cam->GetActorTransform().GetPosition();

	FVector objectPosition = TargetObject->GetWorldTransform().GetPosition() + FVector(0.0f, 0.0f, 1.0f);
	FVector objectScale(0.2f, 0.2f, 0.2f);

	FVector lookDir = (objectPosition - cameraPosition).GetSafeNormal();
	FVector right = FVector(0, 0, 1).Cross(lookDir).GetSafeNormal();
	FVector up = lookDir.Cross(right).GetSafeNormal();

	FMatrix rotationMatrix;

	// 회전 행렬 구성
	rotationMatrix.X = FVector4(lookDir.X, lookDir.Y, lookDir.Z, 0);
	rotationMatrix.Y = FVector4(right.X, right.Y, right.Z, 0);
	rotationMatrix.Z = FVector4(up.X, up.Y, up.Z, 0);
	rotationMatrix.W = FVector4(0, 0, 0, 1);

	FMatrix positionMatrix = FMatrix::GetTranslateMatrix(objectPosition);
	FMatrix scaleMatrix = FMatrix::GetScaleMatrix(objectScale);

	OutMatrix = scaleMatrix * rotationMatrix * positionMatrix;
	return;
}

void FUUIDBillBoard::SetTarget(AActor* Target)
{
	TargetObject = Target->GetRootComponent();
	
	//std::wstring string = L"UUID:";
	std::wstring string = L"유유아이디 :";
	UpdateString(string.append(std::to_wstring(Target->GetUUID())));
	//std::wstring string = L"가츄괘퓌덤굥맑욳낢귉 TEST ENGLISH!";
	//UpdateString(string);
}

void FUUIDBillBoard::Render()
{
	if (VertexBuffer.Num() == 0 || !FEngineShowFlags::Get().GetSingleFlag(EEngineShowFlags::SF_BillboardText) || !TargetObject)
		return;


	// 파이프라인 상태 설정
	//UINT stride = sizeof(FVertexSimple);
	//UINT offset = 0;

	// 기본 셰이더랑 InputLayout을 설정

	//DeviceContext->IASetVertexBuffers(0, 1, &FontVertexBuffer, &stride, &offset);
	//DeviceContext->IASetIndexBuffer(FontIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//DeviceContext->IASetPrimitiveTopology(PrimitiveTopology);

	//DeviceContext->VSSetShader(FontVertexShader, nullptr, 0);
	//DeviceContext->PSSetShader(FontPixelShader, nullptr, 0);
	//DeviceContext->IASetInputLayout(FontInputLayout);

	//DeviceContext->RSSetState(RasterizerState);
	//DeviceContext->OMSetBlendState(BlendState, BlendFactor, 0xffffffff);
	//DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);

	//UVertexBuffer::Find(ResourceName)->Setting();
	//UIndexBuffer::Find(ResourceName)->Setting();
	UMesh::Find(ResourceName)->Setting(ERenderFlags::None);
	// Billboard
	FMatrix ModelMatrix;
	CalculateModelMatrix(ModelMatrix);

	// 버텍스 쉐이더에 상수 버퍼를 설정


	D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
	FFontConstantInfo Constants;
	URenderer* Renderer = UEngine::Get().GetRenderer();

	Constants.ViewProjectionMatrix = FMatrix::Transpose(
		ModelMatrix
		* UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft)->GetViewProjectionMatrix()
	);
	UConstantBuffer::Find(ResourceName)->ChangeData(Constants);
	UConstantBuffer::Find(ResourceName)->VSSetting(0);

	//// D3D11_MAP_WRITE_DISCARD는 이전 내용을 무시하고 새로운 데이터로 덮어쓰기 위해 사용
	//DeviceContext->Map(FontConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
	//{
	//	memcpy(ConstantBufferMSR.pData, &Constants, sizeof(FFontConstantInfo));
	//}
	//DeviceContext->Unmap(FontConstantBuffer, 0);

	UMesh::Find(ResourceName)->Draw();
	//DeviceContext->DrawIndexed((UINT)IndexBuffer.Num(), 0, 0);
}

void FUUIDBillBoard::Create()
{
	FontAtlas = nullptr;
	ResourceName = FString("UUIDText");
	VertexBuffer.Reserve(MaxVerticesPerBatch);
	IndexBuffer.Reserve(MaxIndicesPerBatch);
	UE_LOG("%x", VertexBuffer.GetData());
	UVertexBuffer::Create(
		ResourceName, 
		VertexBuffer, 
		UInputLayout::Find("Simple_IL"),  
		true
	);
	UIndexBuffer::Create(ResourceName, IndexBuffer, true);
	UConstantBuffer::Create(ResourceName, sizeof(FFontConstantInfo));
	UMesh::Create(ResourceName, ResourceName, ResourceName);

	// Blend State
	// Blend
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

}