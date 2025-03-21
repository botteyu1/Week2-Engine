#include "ViewMode.h"
#include "Core/Engine.h"
#include "Core/Rendering/URenderer.h"
#include "Resource/RenderResourceCollection.h"

void FViewModeManager::Initialize()
{
	CurrentViewMode = EViewModeIndex::VMI_Default;
}

FViewModeManager::~FViewModeManager()
{
	// 래스터라이저 상태 해제
	for (auto& pair : ViewModes)
	{
		if (pair.Value)
		{
			pair.Value.reset();
			//pair.Value = nullptr;
		}
	}
	ViewModes.Empty();

}

std::shared_ptr<FViewMode> FViewModeManager::CreateViewMode(
	FString RasterizerName = FString(), 
	FString VertexShaderName = FString(),
	FString PixelShaderName = FString()
)
{	
	std::shared_ptr<FViewMode> resource = std::make_shared<FViewMode>();
	if ( !RasterizerName.IsEmpty() )
		resource->Rasterizer = FRasterizer::Find(RasterizerName).get();
	if ( !VertexShaderName.IsEmpty() )
		resource->VertexShader = FVertexShader::Find(VertexShaderName).get();
	if ( !PixelShaderName.IsEmpty() )
		resource->PixelShader = FPixelShader::Find(PixelShaderName).get();

	return resource;
}

void FViewModeManager::SetViewMode(EViewModeIndex ViewMode)
{
	CurrentViewMode = ViewMode;
	URenderer* renderer = UEngine::Get().GetRenderer();
	switch(CurrentViewMode) {
	case EViewModeIndex::VMI_Default:
		renderer->renderFlags = ERenderFlags::None; break;
	case EViewModeIndex::VMI_Solid:
		renderer->renderFlags = ERenderFlags::None; break;
	case EViewModeIndex::VMI_Wireframe:
		renderer->renderFlags = ERenderFlags::Wirefame; break;
	}
}