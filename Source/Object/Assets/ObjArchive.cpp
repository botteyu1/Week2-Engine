#include "ObjArchive.h"
#include <fstream>
#include <Debug/DebugConsole.h>

void FObjArchive::ObjToBinary(const FString& filePath, const TArray<FVertexTextureArray> vertices, const TArray<uint32> indices, const TArray<FString>& textureNames)
{
	std::ofstream objBinary;
	objBinary.open(filePath.ToWideString(), std::ios::binary);
	if (!objBinary) {
		MsgBoxAssert("can't open file");
		return;
	}
	// 정점 개수 저장 (int32)
	uint32 vertexCount = vertices.Num();
	objBinary.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint32));

	// 인덱스 개수 저장 (uint32)
	uint32 indexCount = indices.Num();
	objBinary.write(reinterpret_cast<const char*>(&indexCount), sizeof(uint32));

	// 모든 정점 데이터 저장
	objBinary.write(reinterpret_cast<const char*>(vertices.GetData()), sizeof(FVertexTextureArray) * vertexCount);

	// 모든 인덱스 데이터 저장
	objBinary.write(reinterpret_cast<const char*>(indices.GetData()), sizeof(uint32) * indexCount);

	// 텍스처 이름 목록 저장
	uint32 textureNameCount = textureNames.Num();
	objBinary.write(reinterpret_cast<const char*>(&textureNameCount), sizeof(uint32));
	for (int32 i = 0; i < textureNames.Num(); i++)
	{
		// FString -> std::string (UTF-8)
		std::string nameStr = textureNames[i].GetData();
		uint32 nameLength = static_cast<uint32>(nameStr.length());
		// 이름 길이 저장
		objBinary.write(reinterpret_cast<const char*>(&nameLength), sizeof(uint32));
		// 이름 데이터 저장 (문자열 끝의 null은 저장하지 않음)
		objBinary.write(nameStr.c_str(), nameLength);
	}

	objBinary.close();
}

bool FObjArchive::ReadBinary(const FString& filePath, TArray<FVertexTextureArray>& outVertices, TArray<uint32>& outIndices, TArray<FString>& outTextureNames)
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

	// 정점(Vertex) 데이터 읽기
	outVertices.SetNum(vertexCount);
	objBinary.read(reinterpret_cast<char*>(outVertices.GetData()), sizeof(FVertexTextureArray) * vertexCount);

	// 인덱스(Index) 데이터 읽기
	outIndices.SetNum(indexCount);
	objBinary.read(reinterpret_cast<char*>(outIndices.GetData()), sizeof(uint32) * indexCount);


	// 텍스처 이름 목록 읽기
	uint32 textureNameCount;
	objBinary.read(reinterpret_cast<char*>(&textureNameCount), sizeof(uint32));
	outTextureNames.Empty();
	for (uint32 i = 0; i < textureNameCount; i++)
	{
		uint32 nameLength;
		objBinary.read(reinterpret_cast<char*>(&nameLength), sizeof(uint32));

		std::string nameStr(nameLength, '\0');
		objBinary.read(&nameStr[0], nameLength);
		// std::string을 FString으로 변환 (UTF-8 -> TCHAR*)
		outTextureNames.Add(FString(nameStr.c_str()));
	}

	objBinary.close();
	return true;
}
