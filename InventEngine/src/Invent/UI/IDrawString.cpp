#include "IDrawString.h"

#include "Invent/ILog.h"

#include <ft2build.h>
#include FT_FREETYPE_H


FT_Library library;
FT_Face face;

FT_Library* libraries;
FT_Face* faces;

static unsigned int library_num = 0;

static std::unordered_map<char, INVENT::CharCharacter> Characters;
static std::unordered_map<wchar_t, INVENT::CharCharacter> WCharacters;
static std::unordered_map<char, INVENT::CharCharacter>* CharacterMaps;
static std::unordered_map<wchar_t, INVENT::CharCharacter>* WCharacterMaps;

namespace INVENT
{
	namespace UI
	{
		bool IDrawString::Init(const char* path, signed long face_index)
		{
			if (FT_Init_FreeType(&library))
			{
				INVENT_LOG_ERROR("FT_Init_FreeType ERROR");
				return false;
			}
			
			auto error = FT_New_Face(library, path, face_index, &face);
			if (error == FT_Err_Unknown_File_Format)
			{
				INVENT_LOG_ERROR("FT_Err_Unknown_File_Format");
				return false;
			}
			else if (error)
			{
				INVENT_LOG_ERROR(std::string("other ") + std::to_string(error));
				return false;
			}
			else
			{
				INVENT_LOG_INFO(std::string("num_glphs ") + std::to_string(face->num_glyphs));
				INVENT_LOG_INFO(std::string("num_face ") + std::to_string(face->num_faces));
			}

			SetPixelSize(0, 64);
			return true;
		}

		bool IDrawString::Init(const std::vector<std::string>& pathes, const std::vector<signed long>& face_indexes)
		{
			libraries = new FT_Library[pathes.size()];
			faces = new FT_Face[pathes.size()];
			CharacterMaps = new std::unordered_map<char, INVENT::CharCharacter>[pathes.size()];
			WCharacterMaps = new std::unordered_map<wchar_t, INVENT::CharCharacter>[pathes.size()];

			library_num = (unsigned int)pathes.size();

			int i = 0;
			if (face_indexes.empty() || (pathes.size() != face_indexes.size()))
			{
				INVENT_LOG_WARNING("face_indexes will use 0 in all libraries;");

				for (auto& path : pathes)
				{
					if (FT_Init_FreeType(&libraries[i]))
					{
						INVENT_LOG_ERROR("FT_Init_FreeType ERROR");
						return false;
					}
					if (FT_New_Face(libraries[i], path.c_str(), 0, &faces[i]))
					{
						INVENT_LOG_ERROR("FT_New_Face ERROR");
						return false;
					}
					FT_Set_Pixel_Sizes(faces[i], 0, 64);
					++i;
				}
			}
			else
			{
				for (auto& path : pathes)
				{
					if (FT_Init_FreeType(&libraries[i]))
					{
						INVENT_LOG_ERROR("FT_Init_FreeType ERROR");
						return false;
					}
					if (FT_New_Face(libraries[i], path.c_str(), (FT_Long)face_indexes[i], &faces[i]))
					{
						INVENT_LOG_ERROR("FT_New_Face ERROR");
						return false;
					}
					FT_Set_Pixel_Sizes(faces[i], 0, 64);
					++i;
				}
			}

			
			return true;
		}

		void IDrawString::Shutdown()
		{
			FT_Done_Face(face);
			FT_Done_FreeType(library);

			if (library_num)
			{
				for (unsigned int i = 0; i < library_num; ++i)
				{
					FT_Done_Face(faces[i]);
					FT_Done_FreeType(libraries[i]);
				}
				delete[] faces;
				delete[] libraries;
				delete[] CharacterMaps;
				delete[] WCharacterMaps;
			}
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

		CharCharacter IDrawString::LoadChar(const char& char_code, unsigned int index)
		{
			if (index >= library_num)
			{
				INVENT_LOG_ERROR("ERROR index");
				return CharCharacter();
			}

			auto result = CharacterMaps[index].find(char_code);
			if (result != CharacterMaps[index].end())
			{
				return result->second;
			}

			if (FT_Load_Char(faces[index], char_code, FT_LOAD_RENDER))
			{
				INVENT_LOG_ERROR("ERROR::FREETYTPE: Failed to load Glyph");
				return CharCharacter();
			}

			CharCharacter c;
			c.Width = faces[index]->glyph->bitmap.width;
			c.Rows = faces[index]->glyph->bitmap.rows;
			c.Buffer = faces[index]->glyph->bitmap.buffer;
			c.OffsetLeft = faces[index]->glyph->bitmap_left;
			c.OffsetTop = faces[index]->glyph->bitmap_top;
			c.AdvanceX = faces[index]->glyph->advance.x;
			c.AdvanceY = faces[index]->glyph->advance.y;

			CharacterMaps[index].insert({char_code, c});
			return c;
		}

		CharCharacter IDrawString::LoadWChar(const wchar_t& wchar_code, unsigned int index)
		{
			if (index >= library_num)
			{
				INVENT_LOG_ERROR("ERROR index");
				return CharCharacter();
			}

			auto result = WCharacterMaps[index].find(wchar_code);
			if (result != WCharacterMaps[index].end())
			{
				return result->second;
			}

			if (FT_Load_Char(faces[index], wchar_code, FT_LOAD_RENDER))
			{
				INVENT_LOG_ERROR("ERROR::FREETYTPE: Failed to load Glyph");
				return CharCharacter();
			}

			CharCharacter c;
			c.Width = faces[index]->glyph->bitmap.width;
			c.Rows = faces[index]->glyph->bitmap.rows;
			c.Buffer = faces[index]->glyph->bitmap.buffer;
			c.OffsetLeft = faces[index]->glyph->bitmap_left;
			c.OffsetTop = faces[index]->glyph->bitmap_top;
			c.AdvanceX = faces[index]->glyph->advance.x;
			c.AdvanceY = faces[index]->glyph->advance.y;

			WCharacterMaps[index].insert({ wchar_code, c });
			return c;
		}


	} // namespace UI


}