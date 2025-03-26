#pragma once
#include "Core/Container/String.h"
#include <Primitive/PrimitiveVertices.h>

struct FTextureCount;

class FObjArchive
{
public: 
	static void ObjToBinary(const FString& filePath, const TArray<FVertexTextureArray> vertices, const TArray<uint32> indices, TArray<uint32> intervals, const TArray<FTextureCount>& textureNames);

	static bool ReadBinary( const FString& filePath, TArray<FVertexTextureArray>& outVertices, TArray<uint32>& outIndices, TArray<uint32>& outIntervals, TArray<FTextureCount>& outTextureNames);
};