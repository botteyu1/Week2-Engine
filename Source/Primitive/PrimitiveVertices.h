#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Container/Array.h"
#include "Core/Math/Vector.h"

struct FVertexSimple
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
	float U, V;		  // Texture UV
	float NX, NY, NZ;  // Normal
};

struct FVertexTextureArray 
{
	float X, Y, Z;    // Position
	float R, G, B, A; // Color
	float U, V;		  // Texture UV
	float NX, NY, NZ;  // Normal
	int TI;			  // Texture Index
};

struct FLineVertexSimple {
	float X, Y, Z;
	float R, G, B, A;

	FLineVertexSimple(const FVector& pos = FVector(0.0f, 0.0f, 0.0f), const FVector4& col = FVector4::WHITE)
		: X(pos.X), Y(pos.Y), Z(pos.Z), R(col.X), G(col.Y), B(col.Z), A(col.W) 
	{}
};

struct FGeometryData
{
	TArray<FVertexTextureArray> Vertices;
	TArray<uint32> Indices;
	TArray<uint32> MaterialIntervals;
};

enum class EPrimitiveType : uint8
{
	EPT_None,
	EPT_Line,
	EPT_Triangle,
	EPT_Quad,
	EPT_Cube,
	EPT_Sphere,
	EPT_Cylinder,
	EPT_Cone,
	EPT_Plane,
	EPT_Gizmo,
	EPT_StaticMesh,
	EPT_Max,
};

extern FVertexSimple GizmoArrowVertices[49];
extern uint32 GizmoArrowIndices[282];

extern FVertexSimple GizmoRotationVertices[128];
extern uint32 GizmoRotationIndices[768];

extern FVertexSimple GizmoScaleVertices[40];
extern uint32 GizmoScaleIndices[174];

extern FVertexSimple MonkeyVertices[507];
extern uint32 MonkeyIndices[2901];

extern FVertexSimple CubeVertices[36];
extern FVertexSimple SphereVertices[2400];
extern FVertexSimple TriangleVertices[3];

struct GlyphInfo
{
	float u;      
	float v;      
	float width;  
	float height; 
	float offsetX;
	float offsetY;
};