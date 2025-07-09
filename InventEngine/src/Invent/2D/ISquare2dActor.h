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
		const glm::vec4& GetColor() const { return _color; }

		/*void SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition();*/

		void SetScale(const glm::vec2& scale);
		const glm::vec2& GetScale();

		// 绕 z 轴 {0.0f, 0.0f, 1.0f} 旋转
		void SetRotation(float rotation);
		// 绕 z 轴 {0.0f, 0.0f, 1.0f} 旋转
		float GetRotation();

		virtual void SetWorldRotation(const glm::vec3& rotation);
		virtual const glm::vec3& GetWorldRotation();

		void SetShader(IShader* shader) { _shader = shader; }
		const IShader* GetShader() const { return _shader; }

		void SetTexture(ITexture2D* texture) { _texture = texture; }
		const ITexture2D* GetTexture() const { return _texture; }
		ITexture2D* GetTexture() { return _texture; }
		void SetTextureID(ITexture2DManagement::TextureID id) { _texture = nullptr; _texture_id = id; }
		ITexture2DManagement::TextureID GetTextureID() const { return _texture_id; }

		// size == 2
		// [0] 左下角 [1] 右上角
		const glm::vec2* GetTextureCoord() const { return _texture_coord; }
		// use this func will set index = 0
		void SetTextureCoord(const glm::vec2& left_down, const glm::vec2& right_up);

		// set index will set texture coord invalid
		void SetTextureCoordIndex(unsigned int width, unsigned int height);
		// set index will set texture coord invalid
		void SetTextureCoordIndex(const ITexture2D::_UInt2& index);

		const ITexture2D::_UInt2& GetTextureCoordIndex() const { return _texture_coord_index; }
		unsigned int GetTextureCoordWidthIndex() const { return _texture_coord_index.width; }
		unsigned int GetTextureCoordHeightIndex() const { return _texture_coord_index.height; }


	private:
		glm::vec4 _color;

		glm::vec2 _texture_coord[2]{};
		ITexture2D::_UInt2 _texture_coord_index;

		ITexture2DManagement::TextureID _texture_id;

		ITexture2D* _texture;
		IShader* _shader;
	};
}


#endif // !_ISQUARE2DACTOR_

