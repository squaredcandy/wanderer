#pragma once

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Wanderer::Engine::Font
{
	extern FT_Library fontLib;
	extern FT_Face fontFace;
	extern FT_GlyphSlot fontGlyphSlot;

	inline void CheckFTError(FT_Error error);
	void InitFontLibrary();
	void LoadFontFace(std::string fontPath);
	void LoadGlyph(FT_ULong charcode, FT_Int32 loadFlags);
	void DrawGlyphs(std::string text);
}