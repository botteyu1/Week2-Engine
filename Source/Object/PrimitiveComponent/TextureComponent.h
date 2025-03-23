#pragma once
#include "UPrimitiveComponent.h"

class UTextureComponent: public UPrimitiveComponent {
	DECLARE_CLASS(UTextureComponent, UPrimitiveComponent)
public:
	UTextureComponent() ;
	virtual ~UTextureComponent();

public:
	inline TArray<UTextureAsset*> GetTextures() const { return textures; }
	inline void AddTexture(UTextureAsset* texture) { textures.Add(texture); }
	inline void AddTexture(FString textureName);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render();
	virtual void CalculateModelMatrix(FMatrix& OutMatrix);
	virtual EPrimitiveType GetType() override {
		return EPrimitiveType::EPT_StaticMesh;
	}

protected:
	TArray<UTextureAsset*> textures;
};

class UDiceComp : public UTextureComponent {
	DECLARE_CLASS(UDiceComp, UTextureComponent)

public:
	UDiceComp();
};

class UMugComp : public UTextureComponent {
	DECLARE_CLASS(UMugComp, UTextureComponent)

public:
	UMugComp();
};