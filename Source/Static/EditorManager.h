#pragma once
#include <memory>

#include "Core/AbstractClass/Singleton.h"
#include "Core/Math/Vector.h"

class ACamera;
class AActor;
class AGizmoActor;
class SWindow;

class UEditorManager
{
	friend class URenderer;
	friend class FDevice;;
public:
	

	void Init();
	void Release();

	void RegisterInputCallbacks();

	void CreateUUIDTexture();

	void InitMainSWindow();
	void SplitHorizontalSWindow(std::shared_ptr<SWindow>& window);
	void SplitVerticalSWindow(std::shared_ptr<SWindow>& window);
	void RemoveSWindow(std::shared_ptr<SWindow>& window);
    
    inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

	std::shared_ptr<SWindow> GetClickedWindow(
		const FVector& InMouseScreenPos, 
		const std::shared_ptr<SWindow> InSWindow
	);

	//void RenderWindow(const std::shared_ptr<SWindow> InSWindow);



	AGizmoActor* GetGizmo() const {return Gizmo;}
	void SetGizmo(AGizmoActor* InGizmo);

	static FVector4 EncodeUUID(uint32 UUID);
	static uint32 DecodeUUID(FVector4 color);

	void LateTick(float DeltaTime);

	void OnUpdateWindowSize(uint32 Width, uint32 Height);

	void OnResizeComplete();
	FVector4 GetPixel(FVector MPos) const;
	std::shared_ptr<SWindow> SelectedWindow;
	std::shared_ptr<SWindow> GetRootWindow() { return RootWindow; }
private:
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
	AGizmoActor* Gizmo = nullptr;
	std::shared_ptr<SWindow> RootWindow;
	

	std::shared_ptr<class UTexture> UUIDTexture = nullptr;
	//std::shared_ptr<class FTexture> UUIDTextureDepthStecil = nullptr;
};




