#include "DirectBuffer.h"

#include <string>
#include "Core/Rendering/FDevice.h"

#include "Debug/DebugConsole.h"

FDirectBuffer::FDirectBuffer()
{
}

FDirectBuffer::~FDirectBuffer()
{
	BufferRelease();
}

void FDirectBuffer::BufferRelease()
{
	if (nullptr != Buffer)
	{
		Buffer->Release();
		Buffer = nullptr;
	}
}
