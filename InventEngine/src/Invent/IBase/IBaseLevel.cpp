#include "IEpch.h"
#include "IBaseLevel.h"

namespace INVENT
{

	IBaseLevel::IBaseLevel()
		: ObjectEventLayer(nullptr)
		, UIEventLayer(nullptr)
		, _window_size({ 0.0f, 0.0f })
		, _clear_color_vec({ 0.2f,0.2f,0.2f,1.0f })
	{
		ObjectEventLayer = CreateLayer();
		this->SetAllEventReturn(false);
		
		UIEventLayer = CreateLayer();

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

	IEventLayer* IBaseLevel::CreateLayer()
	{
		IEventLayer* layer = new IEventLayer;
		layers.push_back(layer);
		return layer;
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
