#include "IEpch.h"
#include "ISquare2dActor.h"

namespace INVENT
{
	ISquare2dActor::ISquare2dActor()
		: IObject2D()
	{
		AddComponent<Scale2DComponent>(glm::vec2{ 1.0f,1.0f });
		AddComponent<Rotation2DComponent>(0.0f);

		_shader = IShaderManagement::GetDefaultSquare2DShader();
	}

	ISquare2dActor::~ISquare2dActor()
	{}
}