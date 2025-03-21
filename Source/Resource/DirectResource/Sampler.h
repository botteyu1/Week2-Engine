#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>

#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Core/hal/PlatformType.h"

class USampler :
	public UResource<USampler> 
{
public:
	USampler();
	~USampler();



	static std::shared_ptr<USampler> Create(const FString& _Name, const D3D11_SAMPLER_DESC& _Desc)
	{
		std::shared_ptr<USampler> Res = USampler::CreateRes(_Name);
		Res->ResCreate(_Desc);
		return Res;
	}
	

	void VSSetting(UINT _Slot);
	void PSSetting(UINT _Slot);

protected:
	void ResCreate(const D3D11_SAMPLER_DESC& _Desc);

private:
	//static std::map<D3D11_FILTER, std::map<D3D11_TEXTURE_ADDRESS_MODE, std::shared_ptr<GameEngineSampler>>> OptionSamplers;

	D3D11_SAMPLER_DESC Desc = {};
	ID3D11SamplerState* State = nullptr;

};
