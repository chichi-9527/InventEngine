#ifndef _IDRAWSTRING_
#define _IDRAWSTRING_

#include "ITexture/ITexture.h"

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



		};
	}

}

#endif // !_IDRAWSTRING_

