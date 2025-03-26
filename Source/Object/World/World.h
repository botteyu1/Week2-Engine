#pragma once
#include "Core/Container/Array.h"
#include "Core/Container/Set.h"
#include "Core/Math/Vector.h"
#include "Core/UObject/Object.h"
#include "Core/UObject/ObjectMacros.h"
#include "Core/Utils/JsonSavehelper.h"
#include "Core/Rendering/FViewport.h"
#include "Debug/DebugConsole.h"
#include "Object/ObjectFactory.h"
#include "Object/Actor/StaticMesh.h"
#include "Object/Actor/Camera.h"
#include "Object/Actor/Light.h"
#include "Core/EngineEnum.h"
#include <map>

class UPrimitiveComponent;

//enum class EViewPortSplitter
//{
//	TopLeft,
//	TopRight,
//	BottomLeft,
//	BottomRight,
//	None,
//};

struct FViewportClient {
	ACamera* camera;
	FViewport viewport;
	FViewportClient(ACamera* camera, FViewport viewport) : camera(camera), viewport(viewport) {};
	void PrepareRender();
};

struct FLightInfo {

};

class UWorld :public UObject
{
	DECLARE_CLASS(UWorld, UObject)

public:
	UWorld() = default;

public:
	void InitWorld();

	//std::shared_ptr<SWindow> GetClickedWindow(
	//	const FVector& InMouseScreenPos, 
	//	const std::shared_ptr<SWindow> InSWindow
	//);

	//void RenderWindow(const std::shared_ptr<SWindow> InSWindow);

	void BeginPlay();
	void Tick(float DeltaTime);
	void LateTick(float DeltaTime);

	void OnDestroy();

	template <typename T>
		requires std::derived_from<T, AActor>
	T* SpawnActor();


	AStaticMesh* SpawnStaticMeshActor(FString meshType, bool texture = false);

	bool DestroyActor(AActor* InActor);
	
	void Render();

	//void DisplayPickingTexture(URenderer& Renderer);
	void RenderMainTexture(URenderer& Renderer);

	void ClearWorld();
	void LoadWorld(const char* InSceneName);
	void SaveWorld();

	//void AddZIgnoreComponent(UPrimitiveComponent* InComponent);
	//void RemoveZIgnoreComponent(UPrimitiveComponent* InComponent) {ZIgnoreRenderComponents.Remove(InComponent); }
	//bool ContainsZIgnoreComponent(UPrimitiveComponent* InComponent) {return ZIgnoreRenderComponents.Find(InComponent) != -1; }
	
	// render
	void AddRenderComponent(class UPrimitiveComponent* Component); 
	void RemoveRenderComponent(UPrimitiveComponent* Component);

	// camera
	//inline ACamera* GetCamera(EViewPortSplitter InType) const { return CameraMap[InType]; }
	inline FViewportClient* AddViewportClient(FRect InRect) {
		FViewportClient* viewportClient = new FViewportClient(
			SpawnActor<ACamera>(),
			FViewport(InRect)
		);
		viewportClient->camera->Viewport = &(viewportClient->viewport);
		ViewportClients.Add(viewportClient);
		return viewportClient;
	};
	inline void RemoveViewportClient(FViewportClient* viewportClient) { 
		ViewportClients.Remove(viewportClient); 
		DestroyActor(viewportClient->camera);
		delete viewportClient;
	}
	// 현재 렌더되는 카메라의 getter, 렌더 루프 바깥에서 쓰면 nullptr 반환
	inline ACamera* GetCameraRenderFocused() const { return CameraRenderFocused; }
	inline ACamera* GetCameraFocused() const { return CameraFocused; }
	inline void SetFocusCamera(ACamera* InCamera) {
		CameraFocused = InCamera;
	}

	inline ALight* GetLight() const { return Light; }

	void RayCasting(const FVector& MouseNDCPos);

	void PickByPixel(const FVector& MousePos);

	TArray<AActor*>& GetActors() { return Actors; }

	float& GetGridSizePtr() { return GridSize; }

	void OnChangedGridSize();

	float GetGridSize() const { return GridSize; }

	//inline const TMap<EViewPortSplitter, ACamera*> GetCameraMap() const { return CameraMap; }

	TArray<FViewportClient*> ViewportClients;

	bool HasBegunPlay() const { return bIsBeginPlay; }

private:
	UWorldInfo GetWorldInfo() const;

	//ACamera* Camera = nullptr;
	//TMap<EViewPortSplitter, ACamera*> CameraMap;
	
	ALight* Light = nullptr;
	//std::shared_ptr<SWindow> RootWindow;
	ACamera* CameraRenderFocused = nullptr;
	ACamera* CameraFocused = nullptr;
	float GridSize = 100.0f;

public:
	FString SceneName;
	uint32 Version = 1;
	
protected:
	TArray<AActor*> Actors;
	//TArray<UPrimitiveComponent*> ZIgnoreRenderComponents;
	TArray<AActor*> ActorsToSpawn;
	TArray<AActor*> PendingDestroyActors; // TODO: 추후에 TQueue로 변경
	std::map<ERenderQueue,TSet<UPrimitiveComponent*>> RenderQueueComponents;

	bool bIsBeginPlay = false;

// Editor Only
public:
	//TArray<class ULayer*> Layers;




	TArray<AActor*> ActiveGroupActors;
// End Editor Only
public:
	//class ULevel* Level = nullptr;
	//class ULineBatchComponent* LineBatcher;

	//TArray<FVector> ViewLocationRenderedLastFrame;
	//TArray<FWorldCachedViewInfo> CachedViewInfoRenderedLastFrame;

	//class AGameModeBase* GameMode = nullptr;
	//class AGameStateBase* GameState = nullptr;
	//class UGameInstance* GameInstance = nullptr;

private:
	//TArray<class AController> Controllers;
	//TArray<class APlayerController> PlayerControllers;
	//TArray<class ACameraActor> CameraActors;

	//class FTimeManger* TimeManager = nullptr;

// Editor Only
public:
	//class FSceneInterface* Scene = nullptr;
	//TArray<FLevelViewportInfo> EditorViews;
// End Editor Only


};

template <typename T>
	requires std::derived_from<T, AActor>
T* UWorld::SpawnActor()
{
	T* Actor = FObjectFactory::ConstructObject<T>();
	
	//if (UWorld* World = UEngine::Get().GetWorld())
	//{
		Actor->SetWorld(this);
		Actors.Add(Actor);
		ActorsToSpawn.Add(Actor);

		if(bIsBeginPlay)
		{
			Actor->BeginPlay();
		}


		return Actor;
	//}

	//UE_LOG("Actor Construction Failed. World is nullptr");
	//return nullptr;
}