#ifndef _IDRAWSTRING_
#define _IDRAWSTRING_

#include "ITexture/ITexture.h"

#include <cwchar>

namespace INVENT
{
	namespace UI
	{

		class IDrawString 
		{
		public:

			static bool Init(const char* path, signed long face_index = 0);
			static bool Init(const std::vector<std::string>& pathes, const std::vector<signed long>& face_indexes = {});
			static void Shutdown();
			static void SetPixelSize(unsigned int w, unsigned int h);

			static CharCharacter LoadChar(const char&);
			static CharCharacter LoadWChar(const wchar_t&);

			// 选择使用哪个 library
			static CharCharacter LoadChar(const char&, unsigned int);
			static CharCharacter LoadWChar(const wchar_t&, unsigned int);

			static void ShowText(const std::string& text);

		private:
			

		};
	}

}

#endif // !_IDRAWSTRING_

