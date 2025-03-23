#pragma once
#include "Vector.h"

struct FRect
{

	union
	{
		struct
		{
			FVector2D Min;
			FVector2D Max;
		};
		struct
		{
			float Left;
			float Top;
			float Right;
			float Bottom;
		};
	};


	FRect()
		: Min(FVector2D::ZeroVector)
		, Max(FVector2D::ZeroVector)
	{
	}

	FRect(const FVector2D& InMin, const FVector2D& InMax)
		: Min(InMin)
		, Max(InMax)
	{
	}

	FRect(float InLeft, float InTop, float InRight, float InBottom)
		: Left(InLeft)
		, Top(InTop)
		, Right(InRight)
		, Bottom(InBottom)
	{
	}

	FVector2D GetSize() const
	{
		return Max - Min;
	}

	FVector2D GetCenter() const
	{
		return (Min + Max) * 0.5f;
	}

	float Width() const
	{
		return Right - Left;
	}

	float Height() const
	{
		return Bottom - Top;
	}

	bool Contains(const FVector2D& Point) const
	{
		return Point.X >= Min.X && Point.X <= Max.X &&
			Point.Y >= Min.Y && Point.Y <= Max.Y;
	}
};