#include "IEpch.h"
#include "IBaseLevel.h"
#include "IBaseActor.h"

namespace INVENT
{

	IBaseLevel::IBaseLevel()
		: ObjectEventLayer(nullptr)
		, _window_size({ 0.0f, 0.0f })
		, _clear_color_vec({ 0.2f,0.2f,0.2f,1.0f })
	{
		ObjectEventLayer = CreateLayer<IEventLayer>();
		this->SetAllEventReturn(false);

		AddLayer(ObjectEventLayer);
		AddLayer(this);
	}

	IBaseLevel::~IBaseLevel()
	{
		for (auto layer : layers)
		{
			if (layer)
			{
				delete layer;
				layer = nullptr;
			}
		}

		for (auto actor : _actors)
		{
			if (actor)
			{
				delete actor;
				actor = nullptr;
			}
		}
	}

	void IBaseLevel::Update(float delta)
	{
		
	}

	void IBaseLevel::SetClearColor(float red, float green, float blue, float alpha)
	{
		_clear_color_vec.r = red;
		_clear_color_vec.g = green;
		_clear_color_vec.b = blue;
		_clear_color_vec.a = alpha;
	}

	void IBaseLevel::SetClearColor(glm::vec4 color)
	{
		_clear_color_vec = color;
	}

	void IBaseLevel::DeleteActor(const IBaseActor* actor)
	{
		// 删除 _actors 中的元素
		auto iter = std::find(_actors.begin(), _actors.end(), actor);
		if (iter != _actors.end())
		{
			delete* iter;
			*iter = _actors.back();
			_actors.pop_back();
		}

		// 删除 _square_2d_actors 中的元素
		EraseSquare2dActor((ISquare2dActor*)actor);

		// 删除其他记录实例类中的元素

	}

	void IBaseLevel::AddActor(IBaseActor* actor)
	{
		_actors.push_back(actor);
	}

	void IBaseLevel::AddSquare2dActor(ISquare2dActor * actor)
	{
		_square_2d_actors.push_back(actor);
	}

	void IBaseLevel::EraseSquare2dActor(ISquare2dActor* actor)
	{
		auto iter = std::find(_square_2d_actors.begin(), _square_2d_actors.end(), actor);
		if (iter != _square_2d_actors.end())
		{
			*iter = _square_2d_actors.back();
			_actors.pop_back();
		}
	}

	void IBaseLevel::_clear()
	{
#ifdef USE_OPENGL
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif // USE_OPENGL
	}

	void IBaseLevel::_clear_color()
	{
#ifdef USE_OPENGL
		glClearColor(_clear_color_vec.r, _clear_color_vec.g, _clear_color_vec.b, _clear_color_vec.a);
#endif // USE_OPENGL
	}

	void IBaseLevel::AddLayer(IEventLayer* layer)
	{
		_event_layers.push_back(layer);
	}

	void IBaseLevel::PopLayer()
	{
		_event_layers.pop_back();
	}

	void IBaseLevel::PopLayer(IEventLayer * layer)
	{
		for (auto iter = _event_layers.begin(); iter != _event_layers.end(); iter++)
		{
			if ((*iter) == layer)
			{
				_event_layers.erase(iter);
			}
		}

	}


	void IBaseLevel::EraseLayer(IEventLayer* layer)
	{
		for (auto iter = layers.begin(); iter != layers.end(); iter++)
		{
			if ((*iter) == layer)
			{
				layers.erase(iter);
			}
		}
		if (layer)
		{
			delete layer;
			layer = nullptr;
		}
	}

}
