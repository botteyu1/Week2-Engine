#include "AssetManager.h"
#include <filesystem>
#include <iostream>

#include "SceneAsset.h"
#include "TextureAsset.h"
#include "FontAtlasAsset.h"
#include <fstream>
#include <string>
#include "Primitive/PrimitiveVertices.h"

using namespace std;

void UAssetManager::RegisterAssetMetaDatas()
{
	if (IsDebuggerPresent())
	{
		filesystem::path curPath = filesystem::current_path();
		filesystem::path assetDir = curPath / TEXT("Contents");
		try
		{
			for (const auto& entry : filesystem::recursive_directory_iterator(assetDir))
			{
				if (filesystem::is_regular_file(entry.status()))
				{
					filesystem::path filePath = entry.path();

					// 파일의 확장자, 전체 경로, 파일 크기를 가져옵니다.
					FString fileName = filePath.stem().string() + filePath.extension().string();
					FString extension = filePath.extension().string();
					FString fullPath = filePath.string();
					uint64 fileSize = filesystem::file_size(filePath);

					RegisterAssetMetaData(fileName, fullPath, fileSize, extension);
				}
			}
		}
		catch (std::filesystem::filesystem_error& e)
		{
			std::wcout << e.what() << std::endl;
		}
	}
	else
	{
		// 빌드된 실행 파일의 경로로 설정
	}
}

void UAssetManager::RegisterAssetMetaData(const FString& name, const FString& path, uint64 size, const FString& extension)
{
	AssetMetaDatas[name] = FAssetMetaData(name, path, size, extension);
}

void UAssetManager::LoadAssets()
{
	for (auto& asset : AssetMetaDatas)
	{
		switch (asset.Value.GetAssetType())
		{
		case EAssetType::Scene:
		{
			USceneAsset* sceneAsset = FObjectFactory::ConstructObject<USceneAsset>();
			if (sceneAsset != nullptr)
			{
				sceneAsset->SetMetaData(asset.Value);
				sceneAsset->Load();
				Assets.Add(sceneAsset->GetAssetName(), sceneAsset);
			}
			else
			{
				cout << "Scene Asset Load Failed: " << asset.Value.GetAssetName().GetData() << endl;
			}
			break;
		}
		case EAssetType::EditorConfig:
		{
			break;
		}
		case EAssetType::Texture:
		{
			UTextureAsset* textureAsset = FObjectFactory::ConstructObject<UTextureAsset>();
			if (textureAsset != nullptr) {
				textureAsset->SetMetaData(asset.Value);
				textureAsset->Load();
				Assets.Add(textureAsset->GetAssetName(), textureAsset);
			} else {
				cout << "Texture Asset Load Failed: " << asset.Value.GetAssetName().GetData() << endl;
			}
			//TextureImage->CreateShaderResourceView();
			break;
		}
		case EAssetType::FontAtlas:
		{
			UFontAtlasAsset* fontAtlasAsset = FObjectFactory::ConstructObject<UFontAtlasAsset>();
			if (fontAtlasAsset != nullptr) {
				fontAtlasAsset->SetMetaData(asset.Value);
				fontAtlasAsset->Load();
				Assets.Add(fontAtlasAsset->GetAssetName(), fontAtlasAsset);
			} else {
				cout << "Texture Asset Load Failed: " << asset.Value.GetAssetName().GetData() << endl;
			}
			break;
		}

		}
	}
}

struct VertexKey {
	int v, vt, vn;
	bool operator==(const VertexKey& o) const {
		return v == o.v && vt == o.vt && vn == o.vn;
	}
};

namespace std {
	template<> struct hash<VertexKey> {
		size_t operator()(const VertexKey& k) const {
			return ((hash<int>()(k.v) ^ (hash<int>()(k.vt) << 1)) >> 1)
				^ (hash<int>()(k.vn) << 1);
		}
	};
}

void UAssetManager::ObjParsing(const FString& filePath, TArray<FVertexSimple>& outVertex, TArray<uint32>& outIndex)
{
	std::ifstream objFile(filePath.ToWideString());

	TArray<FVector> vertex;
	TArray<TPair<float, float>> UV;
	TArray<FVector> normals;
	TArray<TPair<TPair<int, int>, int>> faceData; // ((vertexIdx, uvIdx), normalIdx)

	// 파일 로드
	if (objFile)
		UE_LOG("Failed Parsing %s", filePath.GetData());

	std::string line;
	while (std::getline(objFile, line)) {
		if (line.empty()) continue;

		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "#") continue;
		else if (type == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			vertex.Add(FVector(x, y, z));
		}
		else if (type == "vt") {
			float u, v;
			iss >> u >> v;
			UV.Add({ u, 1.0f - v });
		}
		else if (type == "vn") {
			float nx, ny, nz;
			iss >> nx >> ny >> nz;
			normals.Add(FVector(nx, ny, nz));
		}
		else if (type == "f") {
			std::replace(line.begin(), line.end(), '/', ' ');
			std::istringstream faceStream(line);
			std::string data;
			faceStream >> data; // "f" 읽기

			for (int i = 0; i < 3; i++) {
				int iv, it, in;
				faceStream >> iv >> it >> in;
				faceData.Add(TPair<TPair<int, int>, int>(TPair<int, int>(iv - 1, it - 1), in - 1)); // OBJ 파일 인덱스는 1부터 시작하므로 -1 처리
			}
		}
	}

	TMap<VertexKey, int> vertexMap; // Key: (vertexIdx * 1000000 + uvIdx * 1000 + normalIdx), Value: New Vertex Index
	TArray<FVertexSimple> newVertices;
	TArray<uint32> newIndices;

	for (const auto& face : faceData) {
		int vIdx = face.Key.Key;
		int uvIdx = face.Key.Value;
		int nIdx = face.Value;

		// 키 생성 (단순한 정수 연산으로 유니크한 값 생성)
		//int key = vIdx * 1000000 + uvIdx * 1000 + nIdx;

		VertexKey vertexKey = { vIdx, uvIdx, nIdx };

		if (!vertexMap.Contains(vertexKey)) {
			// 새로운 정점 추가
			int newIndex = newVertices.Num();
			FVector pos = vertex[vIdx];
			FVector norm = normals[nIdx];
			auto [u, v] = UV[uvIdx];

			FVertexSimple simpleVertex(pos.X - 0.5f, pos.Y - 0.5f, pos.Z -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, u, v, norm.X, norm.Y, norm.Z);
			newVertices.Add(simpleVertex);
			vertexMap.Add(vertexKey, newIndex);
		}

		// 인덱스 추가
		newIndices.Add(vertexMap[vertexKey]);
	}

	// 최종 결과 저장
	outVertex = newVertices;
	outIndex = newIndices;

	for (auto& vertex : outVertex) {
		UE_LOG("%f, %f, %f, %f, %f", vertex.X, vertex.Y, vertex.Z, vertex.U, vertex.V);
	}

	UE_LOG("Parsing complete: Total vertices = %d, Total indices = %d", outVertex.Num(), outIndex.Num());
}
