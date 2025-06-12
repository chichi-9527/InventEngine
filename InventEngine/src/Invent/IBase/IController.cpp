#include "IEpch.h"
#include "IController.h"

namespace INVENT
{

	IControllerBase::IControllerBase()
	{
		this->SetAllEventReturn(false);
	}

	IController::IController()
		: IControllerBase()
	{}
	IController::~IController()
	{}




	IController2D::IController2D()
		: IControllerBase()
		, _scene_camera(nullptr)
	{}

	IController2D::~IController2D()
	{}

	const IBasePawnControl2D* IController2D::Get2DPlayerController(size_t index) const
	{
		if (_pawns.size() > index)
		{
			return _pawns[index];
		}
		return nullptr;
	}
	IBasePawnControl2D* IController2D::Get2DPlayerController(size_t index)
	{
		if (_pawns.size() > index)
		{
			return _pawns[index];
		}
		return nullptr;
	}

}