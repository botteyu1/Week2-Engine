#include "FUUIDBillBoard.h"

#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"

#include "Object/World/World.h"
#include "Object/Actor/Actor.h"
#include "Object/Actor/Camera.h"
#include "Object/Assets/AssetManager.h"
#include "Object/Assets/FontAtlasAsset.h"
#include "Object/Assets/TextureAsset.h"
#include "Primitive/PrimitiveHangul.h"

#include "Resource/DirectResource/VertexBuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/ConstantBuffer.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Resource/DirectResource/PixelShader.h"
#include "Resource/DirectResource/InputLayout.h"
#include "Resource/DirectResource/DepthStencilState.h"
#include "Resource/DirectResource/ShaderResourceBinding.h"
#include "Resource/DirectResource/BlendState.h"
#include "Resource/DirectResource/Sampler.h"
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
	UMesh::Find(ResourceName)->Setting(ERenderFlags::None);
	UE_LOG("%x %x", VertexBuffer.GetData(), UVertexBuffer::Find(ResourceName).get()->CPUDataPtr);
}

void FUUIDBillBoard::Flush()
{
	VertexBuffer.Empty();
	IndexBuffer.Empty();
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


	UMesh::Find(ResourceName)->Setting(ERenderFlags::None);
	UVertexShader::Find("Font_VS")->Setting();
	UPixelShader::Find("Font_PS")->Setting();
	USampler::Find("LinearSamplerState")->PSSetting(0);
	UBlendState::Find("TextBlendState")->Setting();
	UDepthStencilState::Find("AlwaysVisibleDepthStencilState")->Setting();
	FontAtlas->GetTexture()->GetResource()->PSSetting(0);
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

	UMesh::Find(ResourceName)->Draw();
}

void FUUIDBillBoard::Create()
{
	FontAtlas = nullptr;
	ResourceName = FString("UUIDText");
	VertexBuffer.Reserve(MaxVerticesPerBatch);
	IndexBuffer.Reserve(MaxIndicesPerBatch);
	
	std::shared_ptr<UVertexBuffer> vb = UVertexBuffer::Create(
		ResourceName, 
		VertexBuffer, 
		UInputLayout::Find("Simple_IL"),  
		true
	);
	UE_LOG("%x %x", VertexBuffer.GetData(), vb.get()->CPUDataPtr);
	UIndexBuffer::Create(ResourceName, IndexBuffer, true);
	UConstantBuffer::Create(ResourceName, sizeof(FFontConstantInfo));
	UMesh::Create(ResourceName, ResourceName, ResourceName);
}