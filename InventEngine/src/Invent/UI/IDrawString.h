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
			static void Shutdown();
			static void SetPixelSize(unsigned int w, unsigned int h);

			static CharCharacter LoadChar(const char&);
			static CharCharacter LoadWChar(const wchar_t&);

			static void ShowText(const std::string& text);

		private:
			

		};
	}

}

#endif // !_IDRAWSTRING_

