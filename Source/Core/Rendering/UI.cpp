#include "UI.h"

#include "FDevice.h"
#include "Core/Engine.h"
#include "Core/Input/PlayerInput.h"
#include "Debug/DebugConsole.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
#include "Object/Actor/Camera.h"
#include "Object/Actor/Cone.h"
#include "Object/Actor/Cube.h"
#include "Object/Actor/Cylinder.h"
#include "Object/Actor/Sphere.h"
#include "Object/Actor/SpotLight.h"
#include "Object/Light/SpotLightComponent.h"
#include "Object/World/World.h"
#include "Static/EditorManager.h"
#include "Static/FUUIDBillBoard.h"
#include "Resource/DirectResource/ViewMode.h"
#include "Object/Actor/StaticMesh.h"
#include "Resource/Mesh.h"
#include "Core/UObject/UObjectIterator.h"
#include "Object/Assets/TextureAsset.h"
#include <Object/PrimitiveComponent/TextureComponent.h>
// #include "FDevice.h"
// #include "FViewModeManager.h"
// #include "Core/Engine.h"
// #include "Core/Input/PlayerInput.h"
// #include "Debug/DebugConsole.h"
// #include "Debug/EngineShowFlags.h"
// #include "ImGui/imgui_impl_dx11.h"
// #include "ImGui/imgui_impl_win32.h"
// #include "ImGui/imgui_internal.h"
// #include "Object/Actor/Camera.h"
// #include "Object/Actor/Cone.h"
// #include "Object/Actor/Cube.h"
// #include "Object/Actor/Cylinder.h"
// #include "Object/Actor/Sphere.h"
// #include "Object/Actor/Sphere.h"
// #include "Object/Actor/SpotLight.h"
// #include "Object/Light/SpotLightComponent.h"
// #include "Object/World/World.h"
// #include "Static/UEditorManager.h"
// #include "Static/FUUIDBillBoard.h"
#include "Object/Assets/AssetManager.h"


void UI::Initialize(HWND hWnd, const FDevice& Device, UINT ScreenWidth, UINT ScreenHeight)
{
    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // 기본 폰트 크기 설정
    io.FontGlobalScale = 1.0f;
    io.DisplaySize = ScreenSize;
    //io.WantSetMousePos = true;
    // ImGui Backend 초기화
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(Device.GetDevice(), Device.GetDeviceContext());

	ScreenSize = ImVec2(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
    InitialScreenSize = ScreenSize;
    bIsInitialized = true;
    
    io.DisplaySize = ScreenSize;

    PreRatio = GetRatio();
    CurRatio = GetRatio();
}

void UI::Update()
{
    POINT mousePos;
    if (GetCursorPos(&mousePos)) {
        HWND hwnd = GetActiveWindow();
        ScreenToClient(hwnd, &mousePos);

        ImVec2 CalculatedMousePos = ResizeToScreenByCurrentRatio(ImVec2(mousePos.x, mousePos.y));
        ImGui::GetIO().MousePos = CalculatedMousePos;
        //UE_LOG("MousePos: (%.1f, %.1f), DisplaySize: (%.1f, %.1f)\n",CalculatedMousePos.x, CalculatedMousePos.y, GetRatio().x, GetRatio().y);
    }
	  
    
    // ImGui Frame 생성
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (bWasWindowSizeUpdated)
    {
        PreRatio = CurRatio;
        CurRatio = GetRatio();
        UE_LOG("Current Ratio: %f, %f", CurRatio.x, CurRatio.y);
    }
#if IS_OBJ_VIEWER
	RenderViewerPanel();
#else
	RenderOutLiner();
	RenderControlPanel();
	RenderPropertyWindow();
	RenderShowFlagsPanel();
	RenderViewModePanel();

	Debug::ShowConsole(bWasWindowSizeUpdated, PreRatio, CurRatio);
#endif

    // ImGui 렌더링
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    bWasWindowSizeUpdated = false;
}


void UI::Shutdown() const
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UI::OnUpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
    // ImGUI 리소스 다시 생성
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();
   // ImGui 창 크기 업데이트
	//ScreenSize = ImVec2(static_cast<float>(InScreenWidth), static_cast<float>(InScreenHeight));

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(InScreenWidth), static_cast<float>(InScreenHeight));

    bWasWindowSizeUpdated = true;
}

void UI::RenderControlPanel()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove;

	ImGui::Begin("Jungle Control Panel", nullptr, window_flags);

	if (bWasWindowSizeUpdated)
	{
		auto* Window = ImGui::GetCurrentWindow();
		ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

		ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetWindowSize(ImVec2(DisplaySize.x * 0.25f, DisplaySize.y * 0.4f));
	}
#if IS_OBJ_VIEWER
#else
    ImGui::Text("Hello, Jungle World!");
    ImGui::Text("FPS: %.3f (%.2f ms)", ImGui::GetIO().Framerate , 1000.0f / ImGui::GetIO().Framerate);

    RenderMemoryUsage();
    RenderPrimitiveSelection();
    RenderCameraSettings();
	RenderGridSettings();
#endif
    ImGui::End();
}

void UI::RenderMemoryUsage() const
{
    const uint64 ContainerAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Container>();
    const uint64 ContainerAllocCount = FPlatformMemory::GetAllocationCount<EAT_Container>();
    const uint64 ObjectAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Object>();
    const uint64 ObjectAllocCount = FPlatformMemory::GetAllocationCount<EAT_Object>();
    ImGui::Text(
        "Container Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte,
        ContainerAllocCount
    );
    ImGui::Text(
        "Object Memory Uses: %llubyte, Count: %llu Objects",
        ObjectAllocByte,
        ObjectAllocCount
    );
    ImGui::Text(
        "Total Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte + ObjectAllocByte,
        ContainerAllocCount + ObjectAllocCount
    );

    ImGui::Separator();
}

void UI::RenderPrimitiveSelection()
{
    const char* items[] = { "Sphere", "Cube", "Cylinder", "Cone", "SpotLight", "Dice", "Mug",
	"Girl", "SpaceShip", "Pirate", "AVLSuitJerry"};

    ImGui::Combo("Primitive", &currentItem, items, IM_ARRAYSIZE(items));

    if (ImGui::Button("Spawn"))
    {
        UWorld* World = UEngine::Get().GetWorld();
        for (int i = 0 ;  i < NumOfSpawn; i++)
        {
            if (strcmp(items[currentItem], "Sphere") == 0)
            {
                World->SpawnActor<ASphere>();
            }
            else if (strcmp(items[currentItem], "Cube") == 0)
            {
                World->SpawnActor<ACube>();
            }
            else if (strcmp(items[currentItem], "Cylinder") == 0)
            {
                World->SpawnActor<ACylinder>();
            }
			else if (strcmp(items[currentItem], "Cone") == 0)
			{
				World->SpawnActor<ACone>();
			}
			else if (strcmp(items[currentItem], "SpotLight") == 0)
			{
				World->SpawnActor<ASpotLight>();
			}
			else if (strcmp(items[currentItem], "Dice") == 0) {
				World->SpawnStaticMeshActor("dice.obj", true);
			}
			else if (strcmp(items[currentItem], "Mug") == 0) {
				World->SpawnStaticMeshActor("Mug.obj");
			}
			else if (strcmp(items[currentItem], "Girl") == 0) {
				World->SpawnStaticMeshActor("Girl.obj", true);
			}
			else if (strcmp(items[currentItem], "SpaceShip") == 0) {
				World->SpawnStaticMeshActor("SpaceShip.obj", true);
			}
			else if (strcmp(items[currentItem], "Pirate") == 0) {
				World->SpawnStaticMeshActor("pirate.obj", true);
			}
			else if (strcmp(items[currentItem], "AVLSuitJerry") == 0) {
				World->SpawnStaticMeshActor("AVLSuitJerry.obj", true);
			}
            //else if (strcmp(items[currentItem], "Triangle") == 0)
            //{
            //    Actor->AddComponent<UTriangleComp>();   
            //}
        }
    }
    ImGui::SameLine();
    ImGui::InputInt("Number of spawn", &NumOfSpawn, 0);

    ImGui::Separator();

    UWorld* World = UEngine::Get().GetWorld();
    uint32 bufferSize = 100;
    char* SceneNameInput = new char[bufferSize];
    strcpy_s(SceneNameInput, bufferSize, *World->SceneName);
    
	if (ImGui::InputText("Scene Name", SceneNameInput, bufferSize))
	{
		World->SceneName = SceneNameInput;
	}
    
    if (ImGui::Button("New Scene"))
    {
        World->ClearWorld();
    }
    if (ImGui::Button("Save Scene"))
    {
        World->SaveWorld();   
    }
    if (ImGui::Button("Load Scene"))
    {
        World->LoadWorld(SceneNameInput);
    }

	delete[] SceneNameInput;
    ImGui::Separator();
}

void UI::RenderCameraSettings() const
{
    ImGui::Text("Camera");

	ACamera* Camera = UEngine::Get().GetWorld()->GetCameraFocused();

    bool IsOrthogonal;
    if (Camera->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        IsOrthogonal = true;
    }
    else if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        IsOrthogonal = false;
    }

    if (ImGui::Checkbox("Orthogonal", &IsOrthogonal))
    {
        if (IsOrthogonal)
        {
            Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
        }
        else
        {
            Camera->ProjectionMode = ECameraProjectionMode::Perspective;
        }
    }

	if (IsOrthogonal) {
		float Scale = Camera->GetZoomSize();
		if ( ImGui::DragFloat("Scale", &Scale, 5.0f) ) {
			Scale = std::clamp(Scale, 100.f, 10000.f);
			Camera->SetZoomSize(Scale);
		}
	} else {
		float FOV = Camera->GetFieldOfView();
		if ( ImGui::DragFloat("FOV", &FOV, 0.1f) ) {
			FOV = std::clamp(FOV, 0.01f, 179.99f);
			Camera->SetFieldOfVew(FOV);
		}
	}
	
    float NearFar[2] = { Camera->GetNear(), Camera->GetFar() };
    if (ImGui::DragFloat2("Near, Far", NearFar, 0.1f))
    {
        NearFar[0] = FMath::Max(0.01f, NearFar[0]);
        NearFar[1] = FMath::Max(0.01f, NearFar[1]);

        if (NearFar[0] < NearFar[1])
        {
            Camera->SetNear(NearFar[0]);
            Camera->SetFar(NearFar[1]);
        }
        else
        {
            if (abs(NearFar[0] - Camera->GetNear()) < 0.00001f)
            {
                Camera->SetFar(NearFar[0] + 0.01f);
            }
            else if (abs(NearFar[1] - Camera->GetFar()) < 0.00001f)
            {
                Camera->SetNear(NearFar[1] - 0.01f);
            }
        }
    }
    
    FVector CameraPosition = Camera->GetActorTransform().GetPosition();
    if (ImGui::DragFloat3("Camera Location", reinterpret_cast<float*>(&CameraPosition), 0.1f))
    {
        FTransform Trans = Camera->GetActorTransform();
        Trans.SetPosition(CameraPosition);
        Camera->SetActorTransform(Trans);
    }

    FVector PrevEulerAngle = Camera->GetActorTransform().GetRotation().GetEuler();
    FVector UIEulerAngle = { PrevEulerAngle.X, PrevEulerAngle.Y, PrevEulerAngle.Z };
    if (ImGui::DragFloat3("Camera Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
    {
        FTransform Transform = Camera->GetActorTransform();

        //FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;
        //Transform.Rotate(DeltaEulerAngle);
        
        UIEulerAngle.Y = FMath::Clamp(UIEulerAngle.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
        Transform.SetRotation(UIEulerAngle);
        Camera->SetActorTransform(Transform);
    }
    ImGui::DragFloat("Camera Speed", &Camera->CameraSpeed, 0.1f);
	ImGui::DragFloat("Camera Sensitivity", &Camera->Sensitivity, 0.1f);

	UEditorManager* editor = UEngine::Get().GetEditor();

	if ( ImGui::Button("Split Horizontal") ) {
		if ( editor->SelectedWindow != nullptr )
			editor->SplitHorizontalSWindow(editor->SelectedWindow);
			
	}
	ImGui::SameLine(0.f, 3.0f);
	if ( ImGui::Button("Split Vertical") ) {
		if ( editor->SelectedWindow != nullptr )
			editor->SplitVerticalSWindow(editor->SelectedWindow);
	}
	ImGui::SameLine(0.f, 3.0f);
	if ( ImGui::Button("Remove") ) {
		if ( editor->SelectedWindow != nullptr )
			editor->RemoveSWindow(editor->SelectedWindow);
	}

	ImGui::Separator();


    FVector Forward = Camera->GetActorTransform().GetForward();
    FVector Up = Camera->GetActorTransform().GetUp();
    FVector Right = Camera->GetActorTransform().GetRight();

    ImGui::Text("Camera GetForward(): (%.2f %.2f %.2f)", Forward.X, Forward.Y, Forward.Z);
    ImGui::Text("Camera GetUp(): (%.2f %.2f %.2f)", Up.X, Up.Y, Up.Z);
    ImGui::Text("Camera GetRight(): (%.2f %.2f %.2f)", Right.X, Right.Y, Right.Z);
	ImGui::Text("MouseLeftDown: %s", UEngine::Get().GetInput()->GetKeyPress(EKeyCode::LButton) ? "True" : "False");
	ImGui::Text("MousePress : %s", UEngine::Get().GetInput()->GetKeyDown(EKeyCode::LButton) ? "True" : "False");
	ImGui::Text("MosueLeftUp: %s", UEngine::Get().GetInput()->GetKeyUp(EKeyCode::LButton) ? "True" : "False");
	ImGui::Separator();
}

void UI::RenderPropertyWindow()
{

	ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

	ImGui::SetNextWindowPos(ImVec2(DisplaySize.x * 0.8f, DisplaySize.y * 0.6f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(DisplaySize.x * 0.2f, DisplaySize.y * 0.4f), ImGuiCond_Once);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove ;

	ImGui::Begin("Properties", nullptr, window_flags);

	if (bWasWindowSizeUpdated)
	{
		ImGui::SetWindowPos(ImVec2(DisplaySize.x * 0.8f, DisplaySize.y * 0.6f));
		ImGui::SetWindowSize(ImVec2(DisplaySize.x * 0.2f, DisplaySize.y * 0.4f));
	}


    AActor* selectedActor = UEngine::Get().GetEditor()->GetSelectedActor();
    if (selectedActor != nullptr)
    {
        FTransform selectedTransform = selectedActor->GetActorTransform();
        float position[] = { selectedTransform.GetPosition().X, selectedTransform.GetPosition().Y, selectedTransform.GetPosition().Z };
        float scale[] = { selectedTransform.GetScale().X, selectedTransform.GetScale().Y, selectedTransform.GetScale().Z };

        if (ImGui::DragFloat3("Translation", position, 0.1f))
        {
            selectedTransform.SetPosition(position[0], position[1], position[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }

        FVector PrevEulerAngle = selectedTransform.GetRotation().GetEuler();
        FVector UIEulerAngle = PrevEulerAngle;
        if (ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
        {
            FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;

            selectedTransform.Rotate(DeltaEulerAngle);
			UE_LOG("Rotation: %.2f, %.2f, %.2f", DeltaEulerAngle.X, DeltaEulerAngle.Y, DeltaEulerAngle.Z);
            selectedActor->SetActorTransform(selectedTransform);
        }
        if (ImGui::DragFloat3("Scale", scale, 0.1f))
        {
            selectedTransform.SetScale(scale[0], scale[1], scale[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }

		AStaticMesh* selectMesh = Cast<AStaticMesh>(selectedActor);

		PropertyStaticMesh(selectMesh);

		// SpotLight 속성 표시
		ASpotLight* spotLight = dynamic_cast<ASpotLight*>(selectedActor);
		if (spotLight != nullptr)
		{
			ImGui::Separator();
			ImGui::Text("SpotLight Properties");

			// SpotLightComponent 가져오기
			USpotLightComponent* spotLightComp = dynamic_cast<USpotLightComponent*>(spotLight->GetRootComponent());
			if (spotLightComp != nullptr)
			{
				// 조명 색상
				/*FVector4 lightColor = spotLightComp->GetLightColor();
				float color[4] = { lightColor.X, lightColor.Y, lightColor.Z, lightColor.W };
				if (ImGui::ColorEdit4("Light Color", color))
				{
					spotLightComp->SetLightColor(FVector4(color[0], color[1], color[2], color[3]));
				}*/

				// 조명 강도
				/*float intensity = spotLightComp->GetIntensity();
				if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 100.0f))
				{
					spotLightComp->SetIntensity(intensity);
				}*/

				// 감쇠 반경
				float attenRadius = spotLightComp->GetAttenuationRadius();
				if (ImGui::DragFloat("Attenuation Radius", &attenRadius, 1.0f, 1.0f, 100.0f))
				{
					spotLightComp->SetAttenuationRadius(attenRadius);
				}

				// 내부 원뿔 각도 (라디안에서 도로 변환)
				/*float innerAngleDegrees = spotLightComp->GetInnerConeAngle() * (180.0f / PI);
				if (ImGui::SliderFloat("Inner Cone Angle", &innerAngleDegrees, 0.0f, 90.0f))
				{
					spotLightComp->SetInnerConeAngle(innerAngleDegrees * (PI / 180.0f));
				}*/

				// 외부 원뿔 각도 (라디안에서 도로 변환)
				float outerAngleDegrees = spotLightComp->GetOuterConeAngle() * (180.0f / PI);
				if (ImGui::SliderFloat("Outer Cone Angle", &outerAngleDegrees, 0.0f, 120.0f))
				{
					spotLightComp->SetOuterConeAngle(outerAngleDegrees * (PI / 180.0f));
				}

				// 그림자 캐스팅 여부
				/*bool castShadows = spotLightComp->GetCastShadows();
				if (ImGui::Checkbox("Cast Shadows", &castShadows))
				{
					spotLightComp->SetCastShadows(castShadows);
				}*/

				// 조명 활성화 여부
				/*bool lightEnabled = spotLightComp->IsLightEnabled();
				if (ImGui::Checkbox("Light Enabled", &lightEnabled))
				{
					spotLightComp->SetLightEnabled(lightEnabled);
				}*/
			}
		}
    }
    ImGui::End();
}

static bool StringGetter(void* data, int idx, const char** out_text)
{
	const std::vector<std::string>* v = static_cast<std::vector<std::string>*>(data);
	if (idx < 0 || idx >= static_cast<int>(v->size())) return false;
	*out_text = (*v)[idx].c_str();
	return true;
}

void UI::RenderOutLiner()
{

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.8f, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.2f, 
			ImGui::GetIO().DisplaySize.y * 0.4f), ImGuiCond_Once);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove;

	ImGui::Begin("OutLiner", nullptr, window_flags);

	if (bWasWindowSizeUpdated)
	{
		auto* Window = ImGui::GetCurrentWindow();
		ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

		ImGui::SetWindowPos(ImVec2(DisplaySize.x *0.8f, 0));
		ImGui::SetWindowSize(ImVec2(DisplaySize.x * 0.2f, DisplaySize.y * 0.4f));
	}

	TArray<AStaticMesh*> Actors;
	for (TObjectIterator<AStaticMesh> It; It; ++It)
	{
		if (It->GetWorld() == UEngine::Get().GetWorld())
		{
			Actors.Add(*It);
		}
	}

	if (Actors.Num() == 0)
	{
		ImGui::End();
		return;
	}

	if (PrevSize != Actors.Num())
	{
		if (CurActor != nullptr)
		{
			CurActor = nullptr;
		}

		// 사용 전에 항상 비우기
		UUIDNames.Empty();
		cUUIDNames.Empty();
		UUIDs.Empty();
		UUIDNames.Reserve(Actors.Num());
		cUUIDNames.Reserve(Actors.Num());
		UUIDs.Reserve(Actors.Num());


		for (int i = 0; i < Actors.Num(); i++) {

			FString UUIDName = Actors[i]->GetClass()->GetName();
			UUIDName += std::to_string(Actors[i]->GetUUID());
			UUIDNames.Add(UUIDName);

			UUIDs.Add(Actors[i]->GetUUID());
		}

		 //모든 문자열이 추가된 후에 포인터 설정
		for (const auto& str : UUIDNames) {
			cUUIDNames.Add(*str);
		}
	}

	PrevSize = Actors.Num();

	static int SelectUUIDIndex = 0;

	const char* preview = SelectUUIDIndex < UUIDNames.Num() ? (*UUIDNames[SelectUUIDIndex]) : "Select Static Mesh";

	// ListBox 사용
	/*if (ImGui::ListBox("StaticMeshList", &SelectUUIDIndex, StringGetter,
		static_cast<void*>(&UUIDNames), static_cast<int>(UUIDNames.Num())))*/


	if (ImGui::ListBox("ActorList", &SelectUUIDIndex, &cUUIDNames[0], static_cast<int>(cUUIDNames.Num())))
	{
		uint32 UUID = UUIDs[SelectUUIDIndex];

		for (int i = 0; i < Actors.Num(); i++)
		{
			AActor* Actor = Actors[i];
			if (Actor->GetUUID() == UUID)
			{
				//if (CurActor != nullptr)
					//CurActor->IsHighlightValue = false;
				CurActor = Actor;
				UEngine::Get().GetEditor()->SelectActor(CurActor);
				UEngine::Get().GetRenderer()->GetUUIDBillBoard()->SetTarget(CurActor);
			}
		}
	}



	//if (ImGui::BeginCombo("StaticMeshList", "Select Static Mesh"))
	//{
	//	for (int i = 0; i < UUIDNames.Num(); i++)
	//	{
	//		bool is_selected = (SelectUUIDIndex == i);
	//		if (ImGui::Selectable((*UUIDNames[i]), is_selected))
	//		{
	//			SelectUUIDIndex = i;
	//			uint32 UUID = UUIDs[SelectUUIDIndex];

	//			for (AActor* Actor : Actors)
	//			{
	//				if (Actor->GetUUID() == UUID)
	//				{
	//					CurActor = Actor;
	//					UEngine::Get().GetEditor()->SelectActor(CurActor);
	//					UEngine::Get().GetRenderer()->GetUUIDBillBoard()->SetTarget(CurActor);
	//					break;
	//				}
	//			}
	//		}

	//		if (is_selected)
	//		{
	//			ImGui::SetItemDefaultFocus();
	//		}
	//	}
	//	ImGui::EndCombo();
	//}

	//if (CurActor != nullptr)
	// {
	// 	// 선택된 오브젝트의 정보를 출력
	// 	FVector Location = CurActor->RelativeLocation();
	// 	FVector Rotation = CurActor->RelativeRotation();
	// 	FVector Scale = CurActor->RelativeScale();
	// 	float LocationArray[3] = { Location.X, Location.Y, Location.Z };
	// 	float RotationArray[3] = { Rotation.X, Rotation.Y, Rotation.Z };
	// 	float ScaleArray[3] = { Scale.X, Scale.Y, Scale.Z };
	// 	if (ImGui::DragFloat3("Location", LocationArray))
	// 	{
	// 		CurObject->SetRelativeLocation(FVector(LocationArray[0], LocationArray[1], LocationArray[2]));
	// 	}
	// 	if (ImGui::DragFloat3("Rotation", RotationArray, 0.05f))
	// 	{
	// 		CurObject->SetRelativeRotation(FVector(RotationArray[0], RotationArray[1], RotationArray[2]));
	// 	}
	// 	if (ImGui::DragFloat3("Scale", ScaleArray,0.02))
	// 	{
	// 		CurObject->SetRelativeScale(FVector(ScaleArray[0], ScaleArray[1], ScaleArray[2]));
	// 	}
	// 	CurObject->IsHighlightValue = true;
	// }

	//if (ImGui::CollapsingHeader("Transform"))
	//{
	//	ImGui::Text("Position");
	//	// Position 관련 위젯들

	//	ImGui::Text("Rotation");
	//	// Rotation 관련 위젯들

	//	ImGui::Text("Scale");
	//	// Scale 관련 위젯들
	//}

	AStaticMesh* selectedStaticMesh = Cast<AStaticMesh>(UEngine::Get().GetEditor()->GetSelectedActor());
	static bool isFirstSelection = true;

	////메쉬 트랜스폼
	//if (selectedStaticMesh != nullptr)
	//{
	//	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;

	//	if (isFirstSelection)
	//	{
	//		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	//		isFirstSelection = false;
	//	}

	//	if (ImGui::TreeNodeEx("Selected Static Mesh", flags))
	//	{
	//		FTransform selectedTransform = selectedStaticMesh->GetActorTransform();
	//		float position[] = { selectedTransform.GetPosition().X, selectedTransform.GetPosition().Y, selectedTransform.GetPosition().Z };
	//		float scale[] = { selectedTransform.GetScale().X, selectedTransform.GetScale().Y, selectedTransform.GetScale().Z };

	//		if (ImGui::DragFloat3("Translation", position, 0.1f))
	//		{
	//			selectedTransform.SetPosition(position[0], position[1], position[2]);
	//			selectedStaticMesh->SetActorTransform(selectedTransform);
	//		}

	//		FVector PrevEulerAngle = selectedTransform.GetRotation().GetEuler();
	//		FVector UIEulerAngle = PrevEulerAngle;
	//		if (ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
	//		{
	//			FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;

	//			selectedTransform.Rotate(DeltaEulerAngle);
	//			UE_LOG("Rotation: %.2f, %.2f, %.2f", DeltaEulerAngle.X, DeltaEulerAngle.Y, DeltaEulerAngle.Z);
	//			selectedStaticMesh->SetActorTransform(selectedTransform);
	//		}
	//		if (ImGui::DragFloat3("Scale", scale, 0.1f))
	//		{
	//			selectedTransform.SetScale(scale[0], scale[1], scale[2]);
	//			selectedStaticMesh->SetActorTransform(selectedTransform);
	//		}

	//		ImGui::TreePop();
	//	}
	//}
	//else
	//{
	//	isFirstSelection = true;
	//}

	ImGui::End();
}

void UI::RenderShowFlagsPanel() const
{


	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove;


	if (ImGui::Begin("Show Flags", nullptr, window_flags))
	{
		if (bWasWindowSizeUpdated)
		{

			auto* Window = ImGui::GetCurrentWindow();
			ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

			ImGui::SetWindowPos(ImVec2(DisplaySize.x * 0.25f, 0.0f));
			ImGui::SetWindowSize(ImVec2(DisplaySize.x * 0.08f, DisplaySize.y * 0.1f));
		}

		bool bPrimitives = FEngineShowFlags::Get().GetSingleFlag(EEngineShowFlags::SF_Primitives);
		if (ImGui::Checkbox("Primitives", &bPrimitives))
		{
			FEngineShowFlags::Get().SetSingleFlag(EEngineShowFlags::SF_Primitives, bPrimitives);
		}

		bool bBillboardText = FEngineShowFlags::Get().GetSingleFlag(EEngineShowFlags::SF_BillboardText);
		if (ImGui::Checkbox("Billboard Text", &bBillboardText))
		{
			FEngineShowFlags::Get().SetSingleFlag(EEngineShowFlags::SF_BillboardText, bBillboardText);
		}
	}
	ImGui::End();
}

void UI::RenderViewModePanel() const
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove;


	if (ImGui::Begin("View Mode", nullptr, window_flags))
	{
		if (bWasWindowSizeUpdated)
		{

			auto* Window = ImGui::GetCurrentWindow();
			ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

			ImGui::SetWindowPos(ImVec2(DisplaySize.x * (0.8f - 0.08f), 0.0f));
			ImGui::SetWindowSize(ImVec2(DisplaySize.x * 0.08f, DisplaySize.y * 0.1f));
		}

		FViewModeManager* viewMode = UEngine::Get().GetRenderer()->GetViewMode();
		static const char* viewModeNames[] = { "Default", "Solid", "Wireframe" };
		int currentViewMode = static_cast<int>(viewMode->GetCurrentViewMode());

		if (ImGui::Combo(" ", &currentViewMode, viewModeNames, IM_ARRAYSIZE(viewModeNames)))
		{
			viewMode->SetViewMode((static_cast<EViewModeIndex>(currentViewMode)));
		}
	}
	ImGui::End();
}

void UI::RenderGridSettings() const
{
	UEngine* Engine = &UEngine::Get();
	UWorld* World = Engine->GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if(ImGui::SliderFloat("Grid Size", &World->GetGridSizePtr(), 100.f, 1000.f, "%.2f"))
	{
		World->OnChangedGridSize();
	}
}

void UI::RenderViewerPanel()
{
	const char* items[] = {"Dice", "Mug",
	"Girl", "SpaceShip", "Pirate", "AVLSuitJerry" };

	ImGui::Combo("Obj", &currentItem, items, IM_ARRAYSIZE(items));

	if (ImGui::Button("Spawn"))
	{
		UWorld* World = UEngine::Get().GetWorld();
		if (NumOfSpawn == 1) {
			World->ClearWorld();
		}
		if (strcmp(items[currentItem], "Dice") == 0) {
			World->SpawnStaticMeshActor("dice.obj", true);
		}
		else if (strcmp(items[currentItem], "Mug") == 0) {
			World->SpawnStaticMeshActor("Mug.obj");
		}
		else if (strcmp(items[currentItem], "Girl") == 0) {
			World->SpawnStaticMeshActor("Girl.obj", true);
		}
		else if (strcmp(items[currentItem], "SpaceShip") == 0) {
			World->SpawnStaticMeshActor("SpaceShip.obj", true);
		}
		else if (strcmp(items[currentItem], "Pirate") == 0) {
			World->SpawnStaticMeshActor("Pirate.obj", true);
		}
		else if (strcmp(items[currentItem], "AVLSuitJerry") == 0) {
			World->SpawnStaticMeshActor("AVLSuitJerry.obj", true);
		}
		NumOfSpawn = 1;
	}
}

void UI::PropertyStaticMesh(AStaticMesh* InAStaticMesh)
{
	if (InAStaticMesh != nullptr)
	{
		const TMap < FName, std::shared_ptr<UMesh> >& Meshes = UMesh::GetAllResources();

		StaticMeshNames.Empty();
		StaticMeshNames.Reserve(Meshes.Num());
		
		cStaticMeshNames.Empty();
		cStaticMeshNames.Reserve(Meshes.Num());


		for (const auto& Pair : Meshes)
		{
			StaticMeshNames.Add(*Pair.Key.ToString());
		}

		//모든 문자열이 추가된 후에 포인터 설정
		for (const auto& str : StaticMeshNames) {
			cStaticMeshNames.Add(*str);
		}


		UTextureComponent* RootStaticMeshComponent = Cast<UTextureComponent>(InAStaticMesh->GetRootComponent());

		if(RootStaticMeshComponent == nullptr)
		{
			return;
		}


		// Get the current mesh name
		FName currentMeshName = RootStaticMeshComponent->GetMesh()->GetFName();
		const char* currentItemName = (*currentMeshName.ToString());


		// 드롭다운 UI 생성
		if (ImGui::BeginCombo("Static Mesh", currentItemName))
		{
			for (int i = 0; i < cStaticMeshNames.Num(); i++)
			{
				bool isSelected = (currentItemName == cStaticMeshNames[i]);
				if (ImGui::Selectable(cStaticMeshNames[i], isSelected))
				{
					// 새 메시 설정
					FName newMeshName = FName(cStaticMeshNames[i]);
					if (Meshes.Contains(newMeshName))
					{
						RootStaticMeshComponent->SetMesh(newMeshName.ToString());
						//UStaticMesh* newMesh = Meshes[newMeshName]->GetMesh();
						//if (newMesh)
						//{
						//	StaticMeshComponent->SetStaticMesh(newMesh);
						//}
					}
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus(); // 기본 포커스 설정
				}
			}
			ImGui::EndCombo();
		}
	}
}
