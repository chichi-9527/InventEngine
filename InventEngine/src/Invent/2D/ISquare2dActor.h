#ifndef _ISQUARE2DACTOR_
#define _ISQUARE2DACTOR_

#include "IObject2D.h"

#include "Shader/IShader.h"

namespace INVENT
{
	class ISquare2dActor : public IObject2D 
	{
	public:
		ISquare2dActor();
		virtual ~ISquare2dActor();

		void SetColor(const glm::vec4& color) { _color = color; }
		const glm::vec4& GetColor() { return _color; }

	private:
		glm::vec4 _color = { 1.0f, 1.0f,1.0f,1.0f }; // white
		IShader* _shader;
	};
}


#endif // !_ISQUARE2DACTOR_

