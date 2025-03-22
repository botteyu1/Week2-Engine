#pragma once
#include "Asset.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Container/Map.h"
#include "Core/AbstractClass/Singleton.h"

class UFontAtlasAsset : public UAsset
{
public:
	UFontAtlasAsset();

	const GlyphInfo& GetGlyph(wchar_t c) const
	{
		auto it = glyphs.Find(c);
		if (it == nullptr)
		{
			return defaultGlyph;
		}
		return *it;
	}

	void AddGlyph(wchar_t c, const GlyphInfo& glyph)
	{
		glyphs[c] = glyph;
	}

	float GlyphAspectRatio = 1.0f;
	float Kerning = 0.6f;

	// UAsset을(를) 통해 상속됨
	bool RegisterAsset() override;
	bool Load() override;
	bool Save(FString path) override;
	bool Unload() override;

private:

	bool ReadFontAtlas(std::vector<wchar_t>& glyphChars);
	bool ApplyGlyphs(std::vector<wchar_t>& glyphChars);

	// Default Setting Value
	int textureWidth = 512;
	int textureHeight = 512;
	int cellWidth = 14;
	int cellHeight = 32;
	int cellsPerRow = 36;
	int cellsPerColumn = 16;

	TMap<wchar_t, GlyphInfo> glyphs;
	GlyphInfo defaultGlyph = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
};