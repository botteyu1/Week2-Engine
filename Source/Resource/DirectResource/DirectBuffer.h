#pragma once
#include <d3d11.h>

class FDirectBuffer
{
public:
	// constrcuter destructer
	FDirectBuffer();
	virtual ~FDirectBuffer();

	// delete Function
	FDirectBuffer(const FDirectBuffer& _Other) = delete;
	FDirectBuffer(FDirectBuffer&& _Other) noexcept = delete;
	FDirectBuffer& operator=(const FDirectBuffer& _Other) = delete;
	FDirectBuffer& operator=(FDirectBuffer&& _Other) noexcept = delete;
protected:
	D3D11_BUFFER_DESC BufferInfo = {0};
	// 모든 버퍼들은 그 용도가 무엇이건 나오는 인터페이스는 버퍼로 동일되다.
	ID3D11Buffer* Buffer = nullptr;
	
	void BufferRelease();
	
};
