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
	void AddTexture(const FString& textureName);

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