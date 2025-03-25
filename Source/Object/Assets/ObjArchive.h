#pragma once
#include "Core/Container/String.h"
#include <Primitive/PrimitiveVertices.h>

class FObjArchive
{
public: 
	static void ObjToBinary(const FString& filePath, const TArray<FVertexTextureArray> vertices, const TArray<uint32> indices, const TArray<FString>& textureNames);

	static bool ReadBinary( const FString& filePath, TArray<FVertexTextureArray>& outVertices, TArray<uint32>& outIndices, TArray<FString>& outTextureNames);
};