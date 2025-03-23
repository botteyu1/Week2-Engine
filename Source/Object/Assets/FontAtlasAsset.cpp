#include "FontAtlasAsset.h"
#include <string>
#include <fstream>
#include <sstream>
#include "Core/Container/String.h"
#include "Object/Assets/AssetManager.h"
#include "Object/Assets/TextureAsset.h"
#include "Debug/DebugConsole.h"

UFontAtlasAsset::UFontAtlasAsset() {}

bool UFontAtlasAsset::RegisterAsset() {
	return true;
}

bool UFontAtlasAsset::Load() {

	std::vector<wchar_t> glyphChars;

	ReadFontAtlas(glyphChars);
	ApplyGlyphs(glyphChars);
	return true;
}

bool UFontAtlasAsset::Save(FString path) {
	return true;
}

bool UFontAtlasAsset::Unload() {
	return false;
}

bool UFontAtlasAsset::ReadFontAtlas(std::vector<wchar_t>& glyphChars) {

	std::ifstream file(GetAssetPath().GetData());
	if ( !file.is_open() ) {
		// 파일을 열 수 없는 경우 오류 처리
		//UE_LOG("Failed to open glyph file: %s", file.c_str());
		UE_LOG("Failed to open font data file");
		return false;
	}

	std::string line;

	while ( std::getline(file, line) ) {
		if ( line.empty() || line[0] == '#' )
			continue;

		std::istringstream iss(line);
		std::string key;
		bool configSectionDone = false;

		if ( !configSectionDone && iss >> key ) {
			if ( key == "TEXTURE_WIDTH" && iss >> textureWidth ) continue;
			if ( key == "TEXTURE_HEIGHT" && iss >> textureHeight ) continue;
			if ( key == "CELL_WIDTH" && iss >> cellWidth ) continue;
			if ( key == "CELL_HEIGHT" && iss >> cellHeight ) continue;
			if ( key == "CELLS_PER_ROW" && iss >> cellsPerRow ) continue;
			if ( key == "CELLS_PER_COLUMN" && iss >> cellsPerColumn ) continue;

			// 숫자로 시작하면 글리프 섹션으로 간주
			if ( isdigit(key[0]) ) {
				configSectionDone = true;
				iss.seekg(0);
			}
		}

		int index, unicodeValue;

		// 인덱스와 유니코드 값 읽기
		if ( iss >> index >> unicodeValue ) {
			if ( index >= 0 && index < cellsPerRow * cellsPerColumn ) {
				if ( index >= glyphChars.size() )
					glyphChars.resize(index + 1, L' ');

				glyphChars[index] = static_cast<wchar_t>(unicodeValue);
			}
		}
	}

	file.close();

	return true;
}

bool UFontAtlasAsset::ApplyGlyphs(std::vector<wchar_t>& glyphChars) {

	GlyphAspectRatio = static_cast<float>(cellWidth) / cellHeight;
	const float uvCellWidth = static_cast<float>(cellWidth) / textureWidth;
	const float uvCellHeight = static_cast<float>(cellHeight) / textureHeight;

	for ( int row = 0; row < cellsPerColumn; ++row ) {
		for ( int col = 0; col < cellsPerRow; ++col ) {
			int index = row * cellsPerRow + col;
			if ( index >= glyphChars.size() ) break;

			float u = (static_cast<float>(col) * cellWidth) / textureWidth;
			float v = (static_cast<float>(row) * cellHeight) / textureHeight;

			GlyphInfo glyph = {
				u,
				v,
				uvCellWidth,
				uvCellHeight,
				0.0f,
				0.0f
			};

			wchar_t wc = glyphChars[index];
			AddGlyph(wc, glyph);
		}
	}

	return true;
}

bool UFontAtlasAsset::LoadTextureAsset() {
	std::string texName = GetAssetName().GetData();
	uint32 nameEndIdx = texName.find('.');
	texName = texName.substr(0, nameEndIdx) + ".png";
	texture = UAssetManager::Get().FindAsset<UTextureAsset>(texName);
	return true;
}