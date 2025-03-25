#pragma once
#include <memory>
#include <string>
#include <queue>

#include "Core/HAL/PlatformType.h"
#include "Core/Math/Vector.h"

struct UObjectInfo
{
	FVector Location;
	FVector Rotation;
	FVector Scale;
	std::string ObjectType;
	std::string objName;
	uint32 bUseTexture;
	uint32 UUID;
};

enum class SWindowType {
	None = 0,
	Window,
	SplitterH,
	SplitterV,
	WorldWindow
};

struct SWindowInfo 
{
	SWindowType type;
	float RectLeft;
	float RectTop;
	float RectRight;
	float RectBottom;
	SWindowInfo* Child;

	// SSplitter
	SWindowInfo* SideLT;
	SWindowInfo* SideRB;
	float SplitPos;

	// SWorldWindow 
	float LocationX;
	float LocationY;
	float LocationZ;
	float RotationX;
	float RotationY;
	float RotationZ;
	float Fov;
	float Scale;
	float Near;
	float Far;
	float Sensitivity;
};

struct UWorldInfo
{
	//UObjectInfo** ObjctInfos;
	std::queue<std::unique_ptr<UObjectInfo>> ObjectInfos;uint32 ActorCount;
	uint32 Version;
	std::string SceneName;
};

namespace json { class JSON; }
class SWindow;

class JsonSaveHelper
{
public:
	// SceneName - 확장자 제외
	static std::unique_ptr<UWorldInfo> LoadScene(const std::string& SceneName);
	static void SaveScene(UWorldInfo WorldInfo);
	
	static std::shared_ptr<SWindow> LoadLayout();
	static void SaveLayout(SWindow* InWindow);
private:
	static std::shared_ptr<SWindow> CreateSWindowNode(
		json::JSON Json,
		std::shared_ptr<SWindow> parent
	);
	static json::JSON CreateSWindowJSON(SWindow* InWindow);
};
