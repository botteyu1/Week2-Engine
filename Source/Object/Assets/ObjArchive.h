#pragma once
#include "Core/Container/String.h"
#include <Primitive/PrimitiveVertices.h>

class FObjArchive
{
public: 
	static void ObjToBinary(const FString& filePath, const TArray<FVertexSimple> vertices, const TArray<uint32> indices);

	static bool ReadBinary( const FString& filePath, TArray<FVertexSimple>& outVertices, TArray<uint32>& outIndices);
};