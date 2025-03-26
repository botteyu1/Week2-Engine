#include "ObjArchive.h"
#include <fstream>
#include <Debug/DebugConsole.h>
#include "Object/Assets/MeshAsset.h"

void FObjArchive::ObjToBinary(const FString& filePath, const TArray<FVertexTextureArray> vertices, 
	const TArray<uint32> indices, TArray<uint32> intervals, const TArray<FTextureCount>& textureCounts)
{
	std::ofstream objBinary;
	objBinary.open(filePath.ToWideString(), std::ios::binary);
	if (!objBinary) {
		MsgBoxAssert("can't open file");
		return;
	}
	// 정점 개수 저장 (uint32)
	uint32 vertexCount = vertices.Num();
	objBinary.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint32));

	// 인덱스 개수 저장 (uint32)
	uint32 indexCount = indices.Num();
	objBinary.write(reinterpret_cast<const char*>(&indexCount), sizeof(uint32));

	// 간격 데이터 개수 저장 (uint32)
	uint32 intervalCount = intervals.Num();
	objBinary.write(reinterpret_cast<const char*>(&intervalCount), sizeof(uint32));

	// 모든 정점 데이터 저장
	objBinary.write(reinterpret_cast<const char*>(vertices.GetData()), sizeof(FVertexTextureArray) * vertexCount);

	// 모든 인덱스 데이터 저장
	objBinary.write(reinterpret_cast<const char*>(indices.GetData()), sizeof(uint32) * indexCount);

	// 모든 간격 데이터 저장
	objBinary.write(reinterpret_cast<const char*>(intervals.GetData()), sizeof(uint32) * intervalCount);

	// 텍스처 이름 목록 저장
	uint32 textureCount = textureCounts.Num();
	objBinary.write(reinterpret_cast<const char*>(&textureCount), sizeof(uint32));
	for (int32 i = 0; i < textureCounts.Num(); i++)
	{
		// FString -> std::string (UTF-8)
		std::string nameStr = textureCounts[i].TextureName;
		uint32 nameLength = static_cast<uint32>(nameStr.length());
		// 이름 길이 저장
		objBinary.write(reinterpret_cast<const char*>(&nameLength), sizeof(uint32));
		// 이름 데이터 저장 (문자열 끝의 null은 저장하지 않음)
		objBinary.write(nameStr.c_str(), nameLength);

		objBinary.write(reinterpret_cast<const char*>(&textureCounts[i].RefCount), sizeof(int32));
	}

	objBinary.close();
}

bool FObjArchive::ReadBinary(const FString& filePath, TArray<FVertexTextureArray>& outVertices, 
	TArray<uint32>& outIndices, TArray<uint32>& outIntervals ,TArray<FTextureCount>& outTextureCounts)
{
	std::ifstream objBinary;
	objBinary.open(filePath.ToWideString(), std::ios::binary);
	if (!objBinary) {
		return false;
	}
	// 정점(Vertex) 개수 읽기
	uint32 vertexCount;
	objBinary.read(reinterpret_cast<char*>(&vertexCount), sizeof(uint32));

	// 인덱스(Index) 개수 읽기
	uint32 indexCount;
	objBinary.read(reinterpret_cast<char*>(&indexCount), sizeof(uint32));

	// interval 개수 읽기
	uint32 intervalCount;
	objBinary.read(reinterpret_cast<char*>(&intervalCount), sizeof(uint32));

	// 정점(Vertex) 데이터 읽기
	outVertices.SetNum(vertexCount);
	objBinary.read(reinterpret_cast<char*>(outVertices.GetData()), sizeof(FVertexTextureArray) * vertexCount);

	// 인덱스(Index) 데이터 읽기
	outIndices.SetNum(indexCount);
	objBinary.read(reinterpret_cast<char*>(outIndices.GetData()), sizeof(uint32) * indexCount);

	// interval 데이터 읽기
	outIntervals.SetNum(intervalCount);
	objBinary.read(reinterpret_cast<char*>(outIntervals.GetData()), sizeof(uint32) * intervalCount);

	// 텍스처 이름 목록 읽기
	uint32 textureCount;
	objBinary.read(reinterpret_cast<char*>(&textureCount), sizeof(uint32));
	outTextureCounts.Empty();
	for (uint32 i = 0; i < textureCount; i++)
	{
		uint32 nameLength;
		objBinary.read(reinterpret_cast<char*>(&nameLength), sizeof(uint32));

		std::string nameStr(nameLength, '\0');
		objBinary.read(&nameStr[0], nameLength);

		int32 refCount;
		objBinary.read(reinterpret_cast<char*>(&refCount), sizeof(int32));

		outTextureCounts.Add(FTextureCount(nameStr, refCount));
	}

	objBinary.close();
	return true;
}
