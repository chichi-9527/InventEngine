#ifndef _IUITEXT_
#define _IUITEXT_

#include <glm/glm.hpp>

#include <string>

namespace INVENT
{
	namespace UI
	{
		struct IUIText
		{
			glm::vec2 Position{};
			glm::vec4 Color{};

			std::string Text;

		};
	}
}

#endif // !_IUITEXT_

