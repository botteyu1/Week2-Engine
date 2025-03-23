#include "MeshAsset.h"
#include "ThirdParty/OBJ_Loader/OBJLoarder.h"
#include "Object/Assets/ObjArchive.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/InputLayout.h"
#include "Resource/Mesh.h"


bool UMeshAsset::RegisterAsset()
{
    return true;
}

bool UMeshAsset::Load()
{
	if (IsLoaded()) {
		return true;
	}
	TArray<FVertexSimple>& vertices = GeometryData.Vertices;
	TArray<uint32>& indices = GeometryData.Indices;

	/*UAssetManager::Get().ObjParsing("cube-tex.obj", vertices, indices);*/
	FString binaryFile = MetaData.GetAssetName() + ".objbinary";
	if (!FObjArchive::ReadBinary(binaryFile, vertices, indices)) {
		objl::Loader OBJLoader;
		bool loadout = OBJLoader.LoadFile(MetaData.GetAssetPath().GetData());

		if (loadout)
		{
			uint32 indexStart = 0;
			for (int i = 0; i < OBJLoader.LoadedMeshes.size(); i++)
			{
				objl::Mesh curMesh = OBJLoader.LoadedMeshes[i];

				for (int j = 0; j < curMesh.Vertices.size(); j++)
				{
					FVector4 color((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX,
						(float)rand() / (float)RAND_MAX, 1.0f);
					FVertexSimple inVertex = {
						curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z,
						color.X, color.Y, color.Z, color.W ,
						curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y,
						curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z
					};
					vertices.Add(inVertex);
				}

				for (int j = 0; j < curMesh.Indices.size(); j += 1) {
					/*indices.Add(curMesh.Indices[j]);
					indices.Add(curMesh.Indices[j + 1]);
					indices.Add(curMesh.Indices[j + 2]);*/
					indices.Add(curMesh.Indices[j]);
				}

				indexStart = curMesh.Vertices.size();

			}
			FObjArchive::ObjToBinary(binaryFile, vertices, indices);
		}
	}
	UVertexBuffer::Create(FString(TEXT(MetaData.GetAssetName())), vertices,
		UInputLayout::Find("Simple_IL")
	);
	UIndexBuffer::Create(FString(TEXT(MetaData.GetAssetName())), indices);

	UMesh::Create(TEXT(MetaData.GetAssetName()));
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
