#include "IDrawString.h"

#include "ILog.h"

#include <ft2build.h>
#include FT_FREETYPE_H


FT_Library library;
FT_Face face;

static std::unordered_map<char, INVENT::CharCharacter> Characters;
static std::unordered_map<wchar_t, INVENT::CharCharacter> WCharacters;

namespace INVENT
{
	namespace UI
	{
		bool IDrawString::Init(const char* path, signed long face_index)
		{
			if (FT_Init_FreeType(&library))
			{

				return false;
			}
			
			auto error = FT_New_Face(library, path, face_index, &face);
			if (error == FT_Err_Unknown_File_Format)
			{
				INVENT_LOG_ERROR("FT_Err_Unknown_File_Format");
			}
			else if (error)
			{
				INVENT_LOG_ERROR(std::string("other ") + std::to_string(error));
			}
			else
			{
				INVENT_LOG_INFO(std::string("num_glphs ") + std::to_string(face->num_glyphs));
				INVENT_LOG_INFO(std::string("num_face ") + std::to_string(face->num_faces));
			}

			SetPixelSize(0, 64);
		}

		void IDrawString::Shutdown()
		{
			FT_Done_Face(face);
			FT_Done_FreeType(library);
		}

		void IDrawString::SetPixelSize(unsigned int w, unsigned int h)
		{
			FT_Set_Pixel_Sizes(face, w, h);
		}

		CharCharacter IDrawString::LoadChar(const char& char_code)
		{
			auto result = Characters.find(char_code);
			if (result != Characters.end())
			{
				return result->second;
			}

			if (FT_Load_Char(face, char_code, FT_LOAD_RENDER))
			{
				INVENT_LOG_ERROR("ERROR::FREETYTPE: Failed to load Glyph");
				return CharCharacter();
			}

			CharCharacter c;
			c.Width = face->glyph->bitmap.width;
			c.Rows = face->glyph->bitmap.rows;
			c.Buffer = face->glyph->bitmap.buffer;
			c.OffsetLeft = face->glyph->bitmap_left;
			c.OffsetTop = face->glyph->bitmap_top;
			c.AdvanceX = face->glyph->advance.x;
			c.AdvanceY = face->glyph->advance.y;

			Characters.insert({ char_code, c });
			return c;
		}

		CharCharacter IDrawString::LoadWChar(const wchar_t& wchar_code)
		{
			auto result = WCharacters.find(wchar_code);
			if (result != WCharacters.end())
			{
				return result->second;
			}

			if (FT_Load_Char(face, wchar_code, FT_LOAD_RENDER))
			{
				INVENT_LOG_ERROR("ERROR::FREETYTPE: Failed to load Glyph");
				return CharCharacter();
			}

			CharCharacter c;
			c.Width = face->glyph->bitmap.width;
			c.Rows = face->glyph->bitmap.rows;
			c.Buffer = face->glyph->bitmap.buffer;
			c.OffsetLeft = face->glyph->bitmap_left;
			c.OffsetTop = face->glyph->bitmap_top;
			c.AdvanceX = face->glyph->advance.x;
			c.AdvanceY = face->glyph->advance.y;

			WCharacters.insert({ wchar_code, c });
			return c;
		}


	} // namespace UI


}