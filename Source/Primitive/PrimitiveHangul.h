#pragma once
#include "Core/HAL/PlatformType.h"
#include "Object/Assets/FontAtlasAsset.h"

struct HangulPrimitive {

	struct HangulJamo {
		wchar_t cho;
		wchar_t jung;
		wchar_t jong;
	};


	static HangulPrimitive::HangulJamo DecomposeHangul(wchar_t c);

	// 중성이 초성의 오른쪽에 오는지 확인
	static bool IsRightJungseong(wchar_t jung);

	// 중성이 초성의 아래에 오는지 확인
	static bool IsBelowJungseong(wchar_t jung);

	// 복합 중성의 첫 부분 반환 (예: ㅘ → ㅗ)
	static wchar_t GetFirstPartOfComplex(wchar_t jung);

	// 복합 중성의 두번째 부분 반환 (예: ㅘ → ㅏ)
	static wchar_t GetSecondPartOfComplex(wchar_t jung);


	static void CreateKoreanQuad(
		const wchar_t character,
		float& cursorX,
		int StringLength,
		UFontAtlasAsset* FontAtlas,
		TArray<FVertexSimple>& VertexBuffer,
		TArray<uint32>& IndexBuffer
	);

	static void CreateKoreanConsonantVowel(
		wchar_t jamo,
		float posX,
		float offsetX,
		float offsetY,
		UFontAtlasAsset* FontAtlas,
		TArray<FVertexSimple>& VertexBuffer,
		TArray<uint32>& IndexBuffer
	);
};