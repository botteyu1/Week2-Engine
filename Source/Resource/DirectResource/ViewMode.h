#pragma once
#define _TCHAR_DEFINED
#include "Core/HAL/PlatformType.h"
#include "Core/Container/Map.h"

class URasterizer;
class UVertexShader;
class UPixelShader;

enum class EViewModeIndex : uint32
{
	// Lit mode는 추후 업데이트
	//VMI_Lit, 
	//VMI_Unlit,
	VMI_Default, // 전역 레스트라이저를 적용하지 않음
	VMI_Solid,
	VMI_Wireframe,
};

struct FViewMode {
	URasterizer* Rasterizer;
	UVertexShader* VertexShader;
	UPixelShader* PixelShader;
};

// ViewMode에 따라 달라지는 Shader, RasterizerState 등을 관리
class FViewModeManager
{
public:
	FViewModeManager() : CurrentViewMode(EViewModeIndex::VMI_Solid), ViewModes() {}
	~FViewModeManager();

	void Initialize();
	void SetViewMode(EViewModeIndex ViewMode);

	EViewModeIndex GetCurrentViewMode() const { return CurrentViewMode; }

private:
	std::shared_ptr<FViewMode> CreateViewMode(FString RasterizerName, FString VertexShaderName, FString PixelShaderName);

	EViewModeIndex CurrentViewMode;
	TMap<EViewModeIndex, std::shared_ptr<FViewMode>> ViewModes;
};
