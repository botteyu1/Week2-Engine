#include "ObjArchive.h"
#include <fstream>
#include <Debug/DebugConsole.h>

void FObjArchive::ObjToBinary(const FString& filePath, const TArray<FVertexSimple> vertices, const TArray<uint32> indices)
{
	std::ofstream objBinary;
	objBinary.open(filePath.ToWideString(), std::ios::binary);
	if (!objBinary) {
		MsgBoxAssert("can't open file");
		return;
	}
	// 정점 개수 저장 (int32)
	int32 vertexCount = vertices.Num();
	objBinary.write(reinterpret_cast<const char*>(&vertexCount), sizeof(int32));

	// 인덱스 개수 저장 (int32)
	int32 indexCount = indices.Num();
	objBinary.write(reinterpret_cast<const char*>(&indexCount), sizeof(int32));

	// 모든 정점 데이터 저장
	objBinary.write(reinterpret_cast<const char*>(vertices.GetData()), sizeof(FVertexSimple) * vertexCount);

	// 모든 인덱스 데이터 저장
	objBinary.write(reinterpret_cast<const char*>(indices.GetData()), sizeof(uint32) * indexCount);

	objBinary.close();
}

bool FObjArchive::ReadBinary(const FString& filePath, TArray<FVertexSimple>& outVertices, TArray<uint32>& outIndices)
{
	std::ifstream objBinary;
	objBinary.open(filePath.ToWideString(), std::ios::binary);
	if (!objBinary) {
		return false;
	}
	// 정점(Vertex) 개수 읽기
	int32 vertexCount;
	objBinary.read(reinterpret_cast<char*>(&vertexCount), sizeof(int32));

	// 인덱스(Index) 개수 읽기
	int32 indexCount;
	objBinary.read(reinterpret_cast<char*>(&indexCount), sizeof(int32));

	// 정점(Vertex) 데이터 읽기
	outVertices.SetNum(vertexCount);
	objBinary.read(reinterpret_cast<char*>(outVertices.GetData()), sizeof(FVertexSimple) * vertexCount);

	// 인덱스(Index) 데이터 읽기
	outIndices.SetNum(indexCount);
	objBinary.read(reinterpret_cast<char*>(outIndices.GetData()), sizeof(uint32) * indexCount);

	objBinary.close();
	return true;
}
