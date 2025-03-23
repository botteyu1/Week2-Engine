#pragma once

#include <memory>

#include "Resource/Resource.h"
#include "DirectBuffer.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/HAL/PlatformType.h"


class UIndexBuffer :
	public UResource<UIndexBuffer> , public FDirectBuffer

{
public:
	
static std::shared_ptr<UIndexBuffer> Create(const FString&  _Name, const TArray<uint32>& _Data , bool _bIsDynamic = false)
	{
		std::shared_ptr<UIndexBuffer> Res = UIndexBuffer::CreateRes(_Name);
	
		Res->bIsDynamic = _bIsDynamic;
		if (Res->bIsDynamic == false)
		{
			Res->ResCreate(_Data.GetData(), _Data.Len());
		}
		else
		{
			Res->ResCreateDynamic(_Data.GetData(), _Data.Len());
		}
		return Res;
	}

	void Setting() const;
	
	inline uint32 GetIndexCount() 
	{
		return IndexCount;
	}

	inline void SetIndexCount(uint32 InIndexCount)
	{
		IndexSize = InIndexCount;
	}

private:
	
	void ResCreate(const void* _Data, size_t _IndexCount);
	void ResCreateDynamic(const void* _Data, size_t _IndexCount);

	DXGI_FORMAT Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	uint32 IndexSize = 4; //sizeof(UINT)
	uint32 IndexCount = 0;
	uint32 Offset = 0;

	const void* CPUDataPtr = nullptr;
	bool bIsDynamic = false;

	
	
};
