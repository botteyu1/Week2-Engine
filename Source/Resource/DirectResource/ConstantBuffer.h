#pragma once
#include <memory>

#include "Resource/Resource.h"
#include "DirectBuffer.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/HAL/PlatformType.h"


class UConstantBuffer :
	public UResource<UConstantBuffer> , public FDirectBuffer
{
public:

	// constrcuter destructer
	UConstantBuffer();
	~UConstantBuffer();
	
	static	std::shared_ptr<UConstantBuffer> Create(const FString&  _Name, uint32 _ByteSize)
	{
		std::shared_ptr<UConstantBuffer> Res = UConstantBuffer::CreateRes(_Name);
		Res->ResCreate(_ByteSize);
		return Res;
	}

	template<typename DataType>
void ChangeData(const DataType& _Data)
	{
		ChangeData(&_Data, sizeof(DataType));
	}
public:
	// 1바이트 자료형
	void ChangeData(const void* _Data, UINT _Size);

	void VSSetting(UINT _Slot);
	void PSSetting(UINT _Slot);
	void CSSetting(UINT _Slot);
	void GSSetting(UINT _Slot);

private:
	void ResCreate(int _ByteSize);
	
};
