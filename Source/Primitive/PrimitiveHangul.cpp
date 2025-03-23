#include <string>
#include "PrimitiveHangul.h"

HangulPrimitive::HangulJamo HangulPrimitive::DecomposeHangul(wchar_t c) {
	HangulJamo result = { 0, 0, 0 };

	// 한글 완성형 범위 (0xAC00 ~ 0xD7A3) 확인
	if ( c < 0xAC00 || c > 0xD7A3 ) {
		return result;
	}

	const int baseCode = 0xAC00;
	const int choCount = 19;
	const int jungCount = 21;
	const int jongCount = 28; // 종성 없음(0) 포함

	int code = c - baseCode;

	int jongIndex = code % jongCount;
	code /= jongCount;

	int jungIndex = code % jungCount;

	int choIndex = code / jungCount;

	// 초성 매핑
	static const wchar_t choTable[19] = {
		0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142, 0x3143,
		0x3145, 0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E
	};

	// 중성 매핑
	static const wchar_t jungTable[21] = {
		0x314F, 0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156, 0x3157,
		0x3158, 0x3159, 0x315A, 0x315B, 0x315C, 0x315D, 0x315E, 0x315F, 0x3160, 0x3161, 0x3162, 0x3163
	};

	// 종성 매핑
	static const wchar_t jongTable[28] = {
		0, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136, 0x3137, 0x3139, 0x313A,
		0x313B, 0x313C, 0x313D, 0x313E, 0x313F, 0x3140, 0x3141, 0x3142, 0x3144, 0x3145,
		0x3146, 0x3147, 0x3148, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E
	};

	result.cho = choTable[choIndex];
	result.jung = jungTable[jungIndex];
	result.jong = jongTable[jongIndex];

	return result;
}

// 중성이 초성의 오른쪽에 오는지 확인
bool HangulPrimitive::IsRightJungseong(wchar_t jung) {
	static const std::wstring rightJungseongs = L"ㅏㅐㅑㅒㅓㅔㅕㅖㅣ";
	return rightJungseongs.find(jung) != std::wstring::npos;
}

// 중성이 초성의 아래에 오는지 확인
bool HangulPrimitive::IsBelowJungseong(wchar_t jung) {
	static const std::wstring belowJungseongs = L"ㅗㅛㅜㅠㅡ";
	return belowJungseongs.find(jung) != std::wstring::npos;
}

// 복합 중성의 첫 부분 반환 (예: ㅘ → ㅗ)
wchar_t HangulPrimitive::GetFirstPartOfComplex(wchar_t jung) {
	switch ( jung ) {
	case L'ㅘ': case L'ㅙ': case L'ㅚ': return L'ㅗ';
	case L'ㅝ': case L'ㅞ': case L'ㅟ': return L'ㅜ';
	case L'ㅢ': return L'ㅡ';
	default: return 0;
	}
}

// 복합 중성의 두번째 부분 반환 (예: ㅘ → ㅏ)
wchar_t HangulPrimitive::GetSecondPartOfComplex(wchar_t jung) {
	switch ( jung ) {
	case L'ㅘ': return L'ㅏ';
	case L'ㅙ': return L'ㅐ';
	case L'ㅚ': return L'ㅣ';
	case L'ㅝ': return L'ㅓ';
	case L'ㅞ': return L'ㅔ';
	case L'ㅟ': return L'ㅣ';
	case L'ㅢ': return L'ㅣ';
	default: return 0;
	}
}

void HangulPrimitive::CreateKoreanQuad(
	const wchar_t character, 
	float& cursorX, 
	int StringLength, 
	UFontAtlasAsset* FontAtlas,
	TArray<FVertexSimple>& VertexBuffer,
	TArray<uint32>& IndexBuffer
) {
	// 한글 자모 분리
	HangulJamo jamo = DecomposeHangul(character);

	// 초성 쿼드 구성
	if ( jamo.cho ) {
		CreateKoreanConsonantVowel(jamo.cho, cursorX, 0.0f, 0.0f, FontAtlas, VertexBuffer, IndexBuffer);
	}

	// 중성 쿼드 구성
	if ( jamo.jung ) {
		// 중성이 오른쪽에 오는 경우
		if ( IsRightJungseong(jamo.jung) ) {
			CreateKoreanConsonantVowel(jamo.jung, cursorX, 0.5f, 0.0f, FontAtlas, VertexBuffer, IndexBuffer);
		}
		// 중성이 아래에 오는 경우
		else if ( IsBelowJungseong(jamo.jung) ) {
			CreateKoreanConsonantVowel(jamo.jung, cursorX, 0.0f, 0.7f, FontAtlas, VertexBuffer, IndexBuffer);
		}
		// 복합 중성인 경우 (ㅘ, ㅙ, ㅚ, ㅝ, ㅞ, ㅟ, ㅢ 등)
		else {
			// 복합 중성의 첫 부분 (예: ㅘ의 ㅗ)
			wchar_t firstPart = GetFirstPartOfComplex(jamo.jung);
			// 복합 중성의 두번째 부분 (예: ㅘ의 ㅏ)
			wchar_t secondPart = GetSecondPartOfComplex(jamo.jung);

			if ( firstPart ) {
				CreateKoreanConsonantVowel(firstPart, cursorX, 0.0f, 0.8f, FontAtlas, VertexBuffer, IndexBuffer); // 아래 부분
			}
			if ( secondPart ) {
				CreateKoreanConsonantVowel(secondPart, cursorX, 0.5f, 0.3f, FontAtlas, VertexBuffer, IndexBuffer); // 오른쪽 부분
			}
		}
	}

	// 종성 쿼드 구성
	if ( jamo.jong ) {
		if ( IsRightJungseong(jamo.jung) ) {
			CreateKoreanConsonantVowel(jamo.jong, cursorX, 0.3f, 1.0f, FontAtlas, VertexBuffer, IndexBuffer);
		} else if ( IsBelowJungseong(jamo.jung) ) {
			CreateKoreanConsonantVowel(jamo.jong, cursorX, 0.0f, 1.4f, FontAtlas, VertexBuffer, IndexBuffer);
		} else {
			CreateKoreanConsonantVowel(jamo.jong, cursorX, 0.3f, 1.4f, FontAtlas, VertexBuffer, IndexBuffer);
		}
	}

	// 커서 이동 (한 글자 너비만큼)
	cursorX += 2 * FontAtlas->GlyphAspectRatio * FontAtlas->Kerning;
}

inline void HangulPrimitive::CreateKoreanConsonantVowel(
	wchar_t jamo, 
	float posX, 
	float offsetX, 
	float offsetY, 
	UFontAtlasAsset* FontAtlas, 
	TArray<FVertexSimple>& VertexBuffer, 
	TArray<uint32>& IndexBuffer
) {
	const GlyphInfo& glyph = FontAtlas->GetGlyph(jamo);

	FVertexSimple vertices[4] = {
		{ 0.0f, posX + offsetX, 1.0f - offsetY, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u, glyph.v },
		{ 0.0f, posX + offsetX + FontAtlas->GlyphAspectRatio, 1.0f - offsetY, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u + glyph.width, glyph.v },
		{ 0.0f, posX + offsetX + FontAtlas->GlyphAspectRatio, -1.0f - offsetY, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u + glyph.width, glyph.v + glyph.height },
		{ 0.0f, posX + offsetX, -1.0f - offsetY, 0.0f, 0.0f, 0.0f, 0.0f, glyph.u, glyph.v + glyph.height }
	};

	// 정점 및 인덱스 버퍼에 추가
	for ( int j = 0; j < 4; ++j ) {
		VertexBuffer.Add(vertices[j]);
	}

	uint32 baseIndex = static_cast<uint32>(VertexBuffer.Num()) - 4;
	IndexBuffer.Add(baseIndex + 0);
	IndexBuffer.Add(baseIndex + 1);
	IndexBuffer.Add(baseIndex + 2);
	IndexBuffer.Add(baseIndex + 0);
	IndexBuffer.Add(baseIndex + 2);
	IndexBuffer.Add(baseIndex + 3);
}
