#include "JsonSaveHelper.h"

#include <fstream>
#include <ranges>

#include "Core/EngineStatics.h"
#include "Debug/DebugConsole.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Container/String.h"
#include "Core/Rendering/SWindow.h"
#include "Object/World/World.h"
#include "SimpleJSON/Json.hpp"

using json::JSON;

// SceneName - 확장자 제외
std::unique_ptr<UWorldInfo> JsonSaveHelper::LoadScene(const std::string& SceneName)
{
	std::ifstream Input(SceneName + ".scene");

	if (!Input.is_open())
	{
		UE_LOG("Scene file not found");
		return nullptr;
	}
	std::string Contents;
	Input.seekg(0, std::ios::end);
	Contents.reserve(Input.tellg());
	Input.seekg(0, std::ios::beg);

	Contents.assign(std::istreambuf_iterator<char>(Input), std::istreambuf_iterator<char>());

	JSON Json = JSON::Load(Contents);

	std::unique_ptr<UWorldInfo> WorldInfo = std::make_unique<UWorldInfo>();
     
    WorldInfo->Version = Json["Version"].ToInt();
    WorldInfo->SceneName = Json["SceneName"].ToString();
    WorldInfo->ActorCount = Json["ActorCount"].ToInt();
	UEngineStatics::NextUUID = Json["NextUUID"].ToInt();
	for (JSON& ActorInfo : Json["Actors"].ObjectRange() | std::views::values)
    {
        std::unique_ptr<UObjectInfo> ObjectInfo = std::make_unique<UObjectInfo>();

        JSON Location = ActorInfo["Location"];
        JSON Rotation = ActorInfo["Rotation"];
        JSON Scale = ActorInfo["Scale"];
        ObjectInfo->Location = FVector(
			static_cast<float>(Location[0].ToFloat()), 
			static_cast<float>(Location[1].ToFloat()),
			static_cast<float>(Location[2].ToFloat())
		);
        ObjectInfo->Rotation = FVector(
			static_cast<float>(Rotation[0].ToFloat()), 
			static_cast<float>(Rotation[1].ToFloat()),
			static_cast<float>(Rotation[2].ToFloat())
		);
        ObjectInfo->Scale = FVector(
			static_cast<float>(Scale[0].ToFloat()), 
			static_cast<float>(Scale[1].ToFloat()),
			static_cast<float>(Scale[2].ToFloat())
		);

        ObjectInfo->ObjectType = ActorInfo["Type"].ToString();
		if (ObjectInfo->ObjectType == "StaticMesh") {
			ObjectInfo->objName = ActorInfo["ObjStaticMeshAsset"].ToString();
			ObjectInfo->bUseTexture = ActorInfo["bUseTexture"].ToInt();
		}
		WorldInfo->ObjectInfos.push(std::move(ObjectInfo));
    }
    return WorldInfo;
}

void JsonSaveHelper::SaveScene(UWorldInfo WorldInfo)
{
	if (WorldInfo.SceneName.empty())
		return;
	JSON Json;

	Json["Version"] = WorldInfo.Version;
	Json["NextUUID"] = UEngineStatics::NextUUID;
	Json["ActorCount"] = WorldInfo.ActorCount;
	Json["SceneName"] = WorldInfo.SceneName;

    // for (uint32 i = 0; i < WorldInfo.ActorCount; i++)
	while (!WorldInfo.ObjectInfos.empty())
    {
        const std::unique_ptr<UObjectInfo> ObjectInfo = std::move(WorldInfo.ObjectInfos.front());
		WorldInfo.ObjectInfos.pop();

        std::string Uuid = std::to_string(ObjectInfo->UUID);
        
        Json["Actors"][Uuid]["Location"].append(ObjectInfo->Location.X, ObjectInfo->Location.Y, ObjectInfo->Location.Z);
        Json["Actors"][Uuid]["Rotation"].append(ObjectInfo->Rotation.X, ObjectInfo->Rotation.Y, ObjectInfo->Rotation.Z);
        Json["Actors"][Uuid]["Scale"].append(ObjectInfo->Scale.X, ObjectInfo->Scale.Y, ObjectInfo->Scale.Z);
        Json["Actors"][Uuid]["Type"] = ObjectInfo->ObjectType;
		if (ObjectInfo->ObjectType == "StaticMesh") {
			Json["Actors"][Uuid]["ObjStaticMeshAsset"] = ObjectInfo->objName;
			Json["Actors"][Uuid]["bUseTexture"] = ObjectInfo->bUseTexture;
		}
    }
     
    std::ofstream Output(WorldInfo.SceneName + ".scene");
    
    if (Output.is_open())
    {
        Output << Json;
    }
}

std::shared_ptr<SWindow> JsonSaveHelper::LoadLayout() {
#ifdef IS_OBJ_VIEWER
	return nullptr;
#else
	//std::string raw = UConfigManager::Get().GetValue("Layout", "SWindow").GetData();
	std::ifstream Input("Config/viewport.json");

	if ( !Input.is_open() ) {
		return nullptr;
	}

	std::string Contents;
	Input.seekg(0, std::ios::end);
	Contents.reserve(Input.tellg());
	Input.seekg(0, std::ios::beg);

	Contents.assign(std::istreambuf_iterator<char>(Input), std::istreambuf_iterator<char>());

	JSON Json = JSON::Load(Contents);

	return CreateSWindowNode(Json, nullptr);
#endif // IS_OBJ_VIEWER
}

std::shared_ptr<SWindow> JsonSaveHelper::CreateSWindowNode(
	json::JSON Json, 
	std::shared_ptr<SWindow> parent = nullptr 
) {

	std::shared_ptr<SWindow> window = nullptr;
	SWindowType type = static_cast<SWindowType>(Json["type"].ToInt());

	if ( type == SWindowType::None ) {
		return window;
	} else if ( type == SWindowType::Window ) {
		window = std::make_shared<SWindow>();
		window->child = std::dynamic_pointer_cast<SSplitter>(
			CreateSWindowNode(Json["Child"], window)
		);
		
	} else if ( type == SWindowType::SplitterH ) {
		std::shared_ptr<SSplitterH> splitter = std::make_shared<SSplitterH>();
		splitter->SideLT = CreateSWindowNode(Json["SideLT"], splitter);
		splitter->SideRB = CreateSWindowNode(Json["SideRB"], splitter);
		splitter->SplitPos = Json["SplitPos"].ToFloat();
		window = splitter;
	} else if ( type == SWindowType::SplitterV ) {
		std::shared_ptr<SSplitterV> splitter = std::make_shared<SSplitterV>();
		splitter->SideLT = CreateSWindowNode(Json["SideLT"], splitter);
		splitter->SideRB = CreateSWindowNode(Json["SideRB"], splitter);
		splitter->SplitPos = Json["SplitPos"].ToFloat();
		window = splitter;
	} else if ( type == SWindowType::WorldWindow ) {

		std::shared_ptr<SWorldWindow> worldWindow = std::make_shared<SWorldWindow>();
		worldWindow->child = nullptr;
		UWorld* world = UEngine::Get().GetWorld();
		FViewportClient* viewportClient = world->AddViewportClient(
			FRect(
				Json["RectLeft"].ToFloat(),
				Json["RectTop"].ToFloat(),
				Json["RectRight"].ToFloat(),
				Json["RectBottom"].ToFloat()
			)
		);
		worldWindow->viewportClient = viewportClient;
		worldWindow->viewportClient->camera->SetActorPosition(
			FVector(
				Json["LocationX"].ToFloat(),
				Json["LocationY"].ToFloat(),
				Json["LocationZ"].ToFloat()
			)
		);
		worldWindow->viewportClient->camera->SetActorRotation(
			FVector(
				Json["RotationX"].ToFloat(),
				Json["RotationY"].ToFloat(),
				Json["RotationZ"].ToFloat()
			)
		);
		worldWindow->viewportClient->camera->FieldOfView = Json["Fov"].ToFloat();
		worldWindow->viewportClient->camera->ZoomSize = Json["Scale"].ToFloat();
		worldWindow->viewportClient->camera->Near = Json["Near"].ToFloat();
		worldWindow->viewportClient->camera->Far = Json["Far"].ToFloat();
		worldWindow->viewportClient->camera->Sensitivity = Json["Sensitivity"].ToFloat();
		window = worldWindow;
	}
	window->parent = parent;
	window->Rect = FRect(
		Json["RectLeft"].ToFloat(),
		Json["RectTop"].ToFloat(),
		Json["RectRight"].ToFloat(),
		Json["RectBottom"].ToFloat()
	);
	return window;
}

void JsonSaveHelper::SaveLayout(SWindow* InWindow) {
#ifdef IS_OBJ_VIEWER
	return;
#else
	JSON Json = CreateSWindowJSON(InWindow);
	//UConfigManager::Get().SetValue("Layout", "SWindow", Json.dump());
	std::ofstream Output("Config/viewport.json");

	if ( Output.is_open() ) {
		Output << Json;
	}
#endif
}

json::JSON JsonSaveHelper::CreateSWindowJSON(SWindow* InWindow) {
	JSON Json;
	if ( InWindow == nullptr ) {
		Json["type"] = static_cast<int>(SWindowType::None);
		return Json;
	}
	Json["type"] = static_cast<int>(SWindowType::Window);
	Json["RectLeft"] = InWindow->Rect.Left;
	Json["RectTop"] = InWindow->Rect.Top;
	Json["RectRight"] = InWindow->Rect.Right;
	Json["RectBottom"] = InWindow->Rect.Bottom;
	{
		SSplitterH* splitter = dynamic_cast<SSplitterH*>(InWindow);
		if (splitter != nullptr) {
			Json["type"] = static_cast<int>(SWindowType::SplitterH);
			Json["SideLT"] = CreateSWindowJSON(splitter->SideLT.get());
			Json["SideRB"] = CreateSWindowJSON(splitter->SideRB.get());
			Json["SplitPos"] = splitter->SplitPos;
		}
	}

	{
		SSplitterV* splitter = dynamic_cast<SSplitterV*>(InWindow);
		if ( splitter != nullptr ) {
			Json["type"] = static_cast<int>(SWindowType::SplitterV);
			Json["SideLT"] = CreateSWindowJSON(splitter->SideLT.get());
			Json["SideRB"] = CreateSWindowJSON(splitter->SideRB.get());
			Json["SplitPos"] = splitter->SplitPos;
		}
	}

	{
		SWorldWindow* worldWindow = dynamic_cast<SWorldWindow*>(InWindow);
		if ( worldWindow != nullptr ) {
			ACamera* cam = worldWindow->viewportClient->camera;
			Json["type"] = static_cast<int>(SWindowType::WorldWindow);
			Json["LocationX"] = cam->GetActorPosition().X;
			Json["LocationY"] = cam->GetActorPosition().Y;
			Json["LocationZ"] = cam->GetActorPosition().Z;
			Json["RotationX"] = cam->GetActorRotation().X;
			Json["RotationY"] = cam->GetActorRotation().Y;
			Json["RotationZ"] = cam->GetActorRotation().Z;
			Json["Fov"] = cam->FieldOfView;
			Json["Scale"] = cam->ZoomSize;
			Json["Near"] = cam->Near;
			Json["Far"] = cam->Far;
			Json["Sensitivity"] = cam->Sensitivity;
		}
	}

	Json["Child"] = CreateSWindowJSON(InWindow->child.get());
	return Json;
}
