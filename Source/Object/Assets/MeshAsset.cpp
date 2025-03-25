#include "MeshAsset.h"
#include "ThirdParty/OBJ_Loader/OBJLoarder.h"
#include "Object/Assets/ObjArchive.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/InputLayout.h"
#include "Resource/Mesh.h"
#include <iostream>
#include <string>
#include "Object/Assets/AssetManager.h"


bool UMeshAsset::RegisterAsset()
{
    return true;
}

bool UMeshAsset::Load()
{
	if (IsLoaded()) {
		return true;
	}
	TArray<FVertexTextureArray>& vertices = GeometryData.Vertices;
	TArray<uint32>& indices = GeometryData.Indices;

	/*UAssetManager::Get().ObjParsing("cube-tex.obj", vertices, indices);*/
	FString binaryFile = "Contents/"+MetaData.GetAssetName();
	FString name = MetaData.GetAssetName();
	if (MetaData.GetAssetExtension() == ".obj") {
		binaryFile += +".objbinary";
	}
	else if (MetaData.GetAssetExtension() == ".objbinary") {
		std::string Name = name.GetData();
		name = Name.substr(0, Name.size() - 10);
	}
	if (!FObjArchive::ReadBinary(binaryFile, vertices, indices)) {
		objl::Loader OBJLoader;
		bool loadout = OBJLoader.LoadFile(MetaData.GetAssetPath().GetData());
		
		if (loadout)
		{
			// 아래 부분에서 TextureArray로 만들 Texture들을 보내줌,  겹치지 않도록
			for (int i = 0; i < OBJLoader.LoadedMaterials.size(); i++) {
				if (OBJLoader.LoadedMaterials[i].map_Kd == "") 
				{
					continue;
				}

				bool bContained = false;
				for(int j = 0 ; j < UsedTextureNames.Num(); j++)
				{
					if (UsedTextureNames[j] == OBJLoader.LoadedMaterials[i].map_Kd) 
					{
						bContained = true;
					}
				}
				if (!bContained) {
					UsedTextureNames.Add(OBJLoader.LoadedMaterials[i].map_Kd);
				}
			}

			uint32 indexStart = 0;
			FVector min(FLT_MAX, FLT_MAX, FLT_MAX);
			FVector max = -min;
			/*for (int i = 0; i < OBJLoader.LoadedVertices.size(); i++) {
				FVector4 color((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX,
					(float)rand() / (float)RAND_MAX, 1.0f);
				FVertexTextureArray  inVertex = {
					-OBJLoader.LoadedVertices[i].Position.Z, -OBJLoader.LoadedVertices[i].Position.X, OBJLoader.LoadedVertices[i].Position.Y,
					color.X, color.Y, color.Z, color.W ,
					OBJLoader.LoadedVertices[i].TextureCoordinate.X, OBJLoader.LoadedVertices[i].TextureCoordinate.Y,
					OBJLoader.LoadedVertices[i].Normal.X, OBJLoader.LoadedVertices[i].Normal.Y, OBJLoader.LoadedVertices[i].Normal.Z,
					0
				};
				vertices.Add(inVertex);
				max.X = FMath::Max(max.X, vertices[i].X);
				max.Y = FMath::Max(max.Y, vertices[i].Y);
				max.Z = FMath::Max(max.Z, vertices[i].Z);
				min.X = FMath::Min(min.X, vertices[i].X);
				min.Y = FMath::Min(min.Y, vertices[i].Y);
				min.Z = FMath::Min(min.Z, vertices[i].Z);
			}*/
			for (int i = 0; i < OBJLoader.LoadedMeshes.size(); i++)
			{
				objl::Mesh curMesh = OBJLoader.LoadedMeshes[i];
				FSubMesh curSubMesh;
				curSubMesh.SubMeshName = curMesh.MeshName;

				
				int textureIndex = GetTextureIndex(curMesh.MeshMaterial.map_Kd);

				SubMeshes.Add(curSubMesh);


				for (int j = 0; j < curMesh.Vertices.size(); j++)
				{
					FVertexTextureArray  inVertex = {
						-curMesh.Vertices[j].Position.Z, -curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y,
						0.4f, 0.4f, 0.4f, 1.0f,
						curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y,
						curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z,
						textureIndex
					};
					max.X = FMath::Max(max.X, inVertex.X);
					max.Y = FMath::Max(max.Y, inVertex.Y);
					max.Z = FMath::Max(max.Z, inVertex.Z);
					min.X = FMath::Min(min.X, inVertex.X);
					min.Y = FMath::Min(min.Y, inVertex.Y);
					min.Z = FMath::Min(min.Z, inVertex.Z);
					vertices.Add(inVertex);
				}

				// 인덱스를 추가할 때, 반드시 메시의 정점 오프셋(indexStart)을 고려해야 함
				for (int j = 0; j < curMesh.Indices.size(); j += 3) {  // 삼각형 기준
					if (j + 2 < curMesh.Indices.size()) // 안전한 접근을 위해 확인
					{
						indices.Add(curMesh.Indices[j] + indexStart);
						indices.Add(curMesh.Indices[j + 1] + indexStart);
						indices.Add(curMesh.Indices[j + 2] + indexStart);
					}
				}
				indexStart += curMesh.Vertices.size();
			}
			float xDist = max.X - min.X;
			float yDist = max.Y - min.Y;
			float zDist = max.Z - min.Z;
			float size = FMath::Max(xDist, yDist);
			size = FMath::Max(size, zDist);
			float scale = 1;
			if (size > 2.0f) {
				scale = FMath::Max(2.0f / size, 0.005f);
			}
			FVector center = (max + min) / 2.0f * scale;
			for (int i = 0; i < vertices.Num(); i++) {
				if (size > 2.0f) {
					vertices[i].X *= scale;
					vertices[i].Y *= scale;
					vertices[i].Z *= scale;
				}
				vertices[i].X -= center.X;
				vertices[i].Y -= center.Y;
				vertices[i].Z -= center.Z;
			}
			FObjArchive::ObjToBinary(binaryFile, vertices, indices);
		}
	}
	
	UVertexBuffer::Create(FString(TEXT(name)), vertices,
		UInputLayout::Find("TextureArray_IL")
	);
	UIndexBuffer::Create(FString(TEXT(name)), indices);
	
	UMesh::Create(TEXT(name));
	MetaData.SetIsLoaded(true);
	return true;
}

bool UMeshAsset::Save(FString path)
{
	return true;
}

bool UMeshAsset::Unload()
{
	return true;
}

int UMeshAsset::GetTextureIndex(FString textureName)
{
	for (int i = 0; i < UsedTextureNames.Num(); i++)
	{
		if (UsedTextureNames[i] == textureName)
		{
			return i;
		}
	}
	return -1;
}
