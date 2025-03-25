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
#include "Object/Assets/ObjMTLAsset.h"


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
		UObjMTLAsset* ObjMTLAsset = UAssetManager::Get().FindAsset<UObjMTLAsset>(OBJLoader.LoadedMTL);
		

		if (loadout)
		{
			uint32 indexStart = 0;
			for (int i = 0; i < OBJLoader.LoadedMeshes.size(); i++)
			{
				objl::Mesh curMesh = OBJLoader.LoadedMeshes[i];
				FSubMesh curSubMesh;
				curSubMesh.SubMeshName = curMesh.MeshName;

				
				int textureIndex = 0;
				if (ObjMTLAsset == nullptr) 
				{
					textureIndex = -1;
				}
				else 
				{
					FObjMaterial* curMaterial = ObjMTLAsset->GetMaterialByName(FName(curMesh.MeshMaterial.name));
					if (curMaterial == nullptr) 
					{
						textureIndex = -2;
					}
					else {
						curSubMesh.MaterialName = curMesh.MeshMaterial.name;
						textureIndex = curMaterial->textureIndex;
					}
				}

				SubMeshes.Add(curSubMesh);


				for (int j = 0; j < curMesh.Vertices.size(); j++)
				{
					FVector4 color((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX,
						(float)rand() / (float)RAND_MAX, 1.0f);
					FVertexTextureArray  inVertex = {
						curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z,
						color.X, color.Y, color.Z, color.W ,
						curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y,
						curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z,
						textureIndex
					};
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
