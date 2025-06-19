#ifndef _ISQUARE2DACTOR_
#define _ISQUARE2DACTOR_

#include "IActor2D.h"

#include "Shader/IShader.h"

#include "IBase/ICamera.h"

#include "ITexture/ITexture.h"

namespace INVENT
{
	class ISquare2dActor : public IActor2D
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

		void SetTexture(ITexture2D* texture) { _texture = texture; }
		const ITexture2D* GetTexture() const { return _texture; }
		void SetTextureID(ITexture2DManagement::TextureID id) { _texture = nullptr; _texture_id = id; }
		ITexture2DManagement::TextureID GetTextureID() const { return _texture_id; }

	private:
		glm::vec4 _color;

		ITexture2DManagement::TextureID _texture_id;

		ITexture2D* _texture;
		IShader* _shader;
	};
}


#endif // !_ISQUARE2DACTOR_

