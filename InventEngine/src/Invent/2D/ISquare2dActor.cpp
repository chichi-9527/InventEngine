#include "IEpch.h"
#include "ISquare2dActor.h"

#include "Invent/IComponent/InventComponent.h"
#include "Invent/2D/2DComponent/Invent2DComponent.h"

#include "Invent/IEngine.h"
#include "Invent/IBase/IRenderThread.h"

namespace INVENT
{
	ISquare2dActor::ISquare2dActor()
		: IActor2D()
		, _texture_id(0)
		, _texture(nullptr)
		, _shader(nullptr)
		, _flip_left_right(false)
		, _flip_up_down(false)
	{
		AddComponent<Scale2DComponent>(glm::vec2{ 1.0f,1.0f });

#ifdef USE_OPENGL
		IEngine::InstancePtr()->GetRenderThreadPtr()->SubmitOpenglInitFuncs([this]() {
			_shader = IShaderManagement::GetDefaultSquare2DShader();
			});
#endif
		
		_texture_coord[0] = { 0.0f, 0.0f };
		_texture_coord[1] = { 1.0f, 1.0f };
		_texture_coord_index.is_valid = false;
	}

	ISquare2dActor::~ISquare2dActor()
	{
		RemoveComponent<Scale2DComponent>();
	}

	void ISquare2dActor::SetScale(const glm::vec2& scale)
	{
		this->GetComponent<Scale2DComponent>()->Scale = scale;
	}

	const glm::vec2& ISquare2dActor::GetScale()
	{
		return this->GetComponent<Scale2DComponent>()->Scale;
	}

	void ISquare2dActor::SetRotation(float rotation)
	{
		this->GetComponent<WorldRotationComponent>()->Rotation.z = rotation;
	}

	float ISquare2dActor::GetRotation()
	{
		return this->GetComponent<WorldRotationComponent>()->Rotation.z;
	}

	void ISquare2dActor::SetTextureCoord(const glm::vec2& left_down, const glm::vec2& right_up)
	{
		_texture_coord[0] = left_down;
		_texture_coord[1] = right_up;

		_texture_coord_index.is_valid = false;
		_texture_coord_index.width = 0;
		_texture_coord_index.height = 0;
	}

	void ISquare2dActor::SetTextureCoordIndex(unsigned int width, unsigned int height)
	{
		_texture_coord_index.is_valid = true;
		_texture_coord_index.width = width;
		_texture_coord_index.height = height;
	}

	void ISquare2dActor::SetTextureCoordIndex(const ITexture2D::_UInt2 & index)
	{
		_texture_coord_index.is_valid = true;
		_texture_coord_index = index;
	}

}