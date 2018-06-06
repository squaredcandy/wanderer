#include "Font.h"

namespace Wanderer::Engine::Font
{
	FT_Library fontLib;
	FT_Face fontFace;
	FT_GlyphSlot fontGlyphSlot;

	void CheckFTError(FT_Error error)
	{
		if (error)
		{
			printf("FreeType Error: %d\n", error);
		}
	}

	void InitFontLibrary()
	{
		auto ftError = FT_Init_FreeType(&fontLib);
		CheckFTError(ftError);
	}

	void LoadFontFace(std::string fontPath)
	{
		auto ftError = FT_New_Face(fontLib, fontPath.c_str(), 0, &fontFace);
		CheckFTError(ftError);
		ftError = FT_Set_Char_Size(fontFace, 0, 16 * 64, 300, 300);
		CheckFTError(ftError);
		//ftError = FT_Set_Pixel_Sizes(fontFace, 0, 16);
		//CheckFTError(ftError);
		
		fontGlyphSlot = fontFace->glyph;
	}

	void LoadGlyph(FT_ULong charcode, FT_Int32 loadFlags)
	{
		auto ftError = FT_Load_Glyph(fontFace,
									 FT_Get_Char_Index(fontFace, charcode),
									 loadFlags);
		CheckFTError(ftError);
	}

	void DrawGlyphs(std::string text)
	{

	}
	
}