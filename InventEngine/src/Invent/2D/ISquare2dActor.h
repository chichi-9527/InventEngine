#ifndef _ISQUARE2DACTOR_
#define _ISQUARE2DACTOR_

#include "IObject2D.h"

#include "Shader/IShader.h"

#include "IBase/IBaseActor.h"
#include "IBase/ICamera.h"

#include "ITexture/ITexture.h"

namespace INVENT
{
	class ISquare2dActor : public IObject2D ,public IBaseActor
	{
	public:
		ISquare2dActor();
		virtual ~ISquare2dActor();

		void SetColor(const glm::vec4& color) { _color = color; }
		const glm::vec4& GetColor() { return _color; }

		void SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition();

		void SetScale(const glm::vec2& scale);
		const glm::vec2& GetScale();

		void SetRotation(float rotation);
		float GetRotation();

		void SetShader(IShader* shader) { _shader = shader; }
		const IShader* GetShader() { return _shader; }

		void SetTexture();

	private:
		ITexture2D* _texture = nullptr;

		glm::vec4 _color = { 1.0f, 1.0f,1.0f,1.0f }; // white
		IShader* _shader;
	};
}


#endif // !_ISQUARE2DACTOR_

