#include "UParticleSubUVComponent.h"
#include "Core/Engine.h"
#include "Core/Rendering/FDevice.h"
#include "Core/Rendering/URenderer.h"
#include "Object/Actor/Actor.h"
#include "Object/World/World.h"
#include "Object/Actor/Camera.h"

UParticleSubUVComponent::UParticleSubUVComponent()
{
	bLoop = true;
	bIsPlaying = true;
	bIsPicked = false;
	PlayRate = 1.0f;
	CurrentTime = 0.0f;
	CurrentFrame = 0;



	TotalFrames = 0;
	FrameWidth = 1.0f / NumColumns;
	FrameHeight = 1.0f / NumRows;

	GetRenderResourceCollection().SetMesh("Quad");
	GetRenderResourceCollection().SetMaterial("SubUVMaterial");
	GetRenderResourceCollection().SetConstantBufferBinding("SubUVVertexConstants", &GetVertexConstantsData(), 0, true, false);
	GetRenderResourceCollection().SetConstantBufferBinding("SubUVPixelConstants", &GetPixelConstantsData(), 0, false, true);
	GetRenderResourceCollection().SetTextureBinding("SubUVTexture", 1, false, true);
	GetRenderResourceCollection().SetSamplerBinding("LinearSamplerState", 0, false, true);

	PixelConstants.UVOffsetAndSize = FVector4(0.0f, 0.0f, FrameWidth, FrameHeight);
	PixelConstants.AnimationParams = FVector4(0.0f, static_cast<float>(TotalFrames), 0.0f, 0.0f);
	PixelConstants.ColorModifier = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void UParticleSubUVComponent::BeginPlay()
{
	USceneComponent::BeginPlay();

	if (NumRows > 0 && NumColumns > 0)
	{
		FrameWidth = 1.0f / NumColumns;
		FrameHeight = 1.0f / NumRows;
		TotalFrames = NumRows * NumColumns;
	}
}

void UParticleSubUVComponent::Tick(float DeltaTime)
{
	USceneComponent::Tick(DeltaTime);

	if (bIsPlaying && TotalFrames > 0)
	{
		CurrentTime += DeltaTime * PlayRate;

		float frameCount = CurrentTime * TotalFrames;

		if (bLoop)
		{
			// 루핑 애니메이션
			CurrentFrame = static_cast<uint32>(frameCount) % TotalFrames;
		}
		else
		{
			CurrentFrame = static_cast<uint32>(frameCount);
			if (CurrentFrame >= TotalFrames)
			{
				CurrentFrame = TotalFrames - 1;
				bIsPlaying = false;
			}
		}

		// 현재 프레임의 UV 오프셋 계산
		uint32 row = CurrentFrame / NumColumns;
		uint32 col = CurrentFrame % NumColumns;

		PixelConstants.UVOffsetAndSize.X = col * FrameWidth;
		PixelConstants.UVOffsetAndSize.Y = row * FrameHeight;
		PixelConstants.AnimationParams.X = static_cast<float>(CurrentFrame);
		PixelConstants.AnimationParams.W = CurrentTime;
	}

	Render();
}

void UParticleSubUVComponent::CalculateModelMatrix(FMatrix& OutMatrix)
{
	ACamera* cam = UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft);

	FVector cameraPosition = cam->GetActorTransform().GetPosition();


	FVector objectPosition = GetWorldTransform().GetPosition();
	FVector objectScale = GetWorldTransform().GetScale();

	FVector lookDir = (cameraPosition - objectPosition).GetSafeNormal();
	FVector right = FVector(0, 0, 1).Cross(lookDir).GetSafeNormal();

	FVector up = lookDir.Cross(right).GetSafeNormal();
	FMatrix rotationMatrix;

	rotationMatrix.X = FVector4(lookDir.X, lookDir.Y, lookDir.Z, 0);
	rotationMatrix.Y = FVector4(right.X, right.Y, right.Z, 0);
	rotationMatrix.Z = FVector4(up.X, up.Y, up.Z, 0);
	rotationMatrix.W = FVector4(0, 0, 0, 1);

	FMatrix positionMatrix = FMatrix::GetTranslateMatrix(objectPosition);
	FMatrix scaleMatrix = FMatrix::GetScaleMatrix(objectScale);

	OutMatrix = scaleMatrix * rotationMatrix * positionMatrix;

	return;
}

void UParticleSubUVComponent::Render()
{
	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (Renderer == nullptr)
	{
		return;
	}

	FMatrix ModelMatrix;
	CalculateModelMatrix(ModelMatrix);

	const FMatrix& ViewProjectionMatrix = UEngine::Get().GetWorld()->GetCamera(EViewPortSplitter::TopLeft)->GetViewProjectionMatrix();

	FMatrix MVP = FMatrix::Transpose(
		ModelMatrix *
		ViewProjectionMatrix
	);

	VertexConstants.MVP = MVP;

	GetRenderResourceCollection().Render();
}

void UParticleSubUVComponent::Play()
{
	bIsPlaying = true;
}

void UParticleSubUVComponent::Pause()
{
	bIsPlaying = false;
}

void UParticleSubUVComponent::Stop()
{
	bIsPlaying = false;
	CurrentTime = 0.0f;
	CurrentFrame = 0;
}

void UParticleSubUVComponent::SetPlayRate(float NewRate)
{
	PlayRate = NewRate;
}

void UParticleSubUVComponent::SetLooping(bool bShouldLoop)
{
	bLoop = bShouldLoop;
}

FVector4 UParticleSubUVComponent::GetCurrentFrameUV() const
{
	if (TotalFrames == 0)
	{
		return FVector4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	// 현재 프레임의 행과 열 계산
	uint32 row = CurrentFrame / TextureInfo.NumColumns;
	uint32 col = CurrentFrame % TextureInfo.NumColumns;

	float u1 = col * TextureInfo.FrameWidth;
	float v1 = row * TextureInfo.FrameHeight;
	float u2 = u1 + TextureInfo.FrameWidth;
	float v2 = v1 + TextureInfo.FrameHeight;

	return FVector4(u1, v1, u2, v2);
}
