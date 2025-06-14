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




	IPlayerController2D::IPlayerController2D()
		: IControllerBase()
		, _scene_camera(nullptr)
		, _control_player_index(0)
	{}

	IPlayerController2D::~IPlayerController2D()
	{

	}

	void IPlayerController2D::AddPlayer(IBasePawnControl2D * pawn)
	{
		_pawns.push_back(pawn);
	}

	void IPlayerController2D::ErasePlayer(IBasePawnControl2D* pawn)
	{
		_pawns.erase(std::remove(_pawns.begin(), _pawns.end(), pawn), _pawns.end());
	}

	const IBasePawnControl2D* IPlayerController2D::Get2DPlayerController(size_t index) const
	{
		if (_pawns.size() > index)
		{
			return _pawns[index];
		}
		return nullptr;
	}
	IBasePawnControl2D* IPlayerController2D::Get2DPlayerController(size_t index)
	{
		if (_pawns.size() > index)
		{
			return _pawns[index];
		}
		return nullptr;
	}

	bool IPlayerController2D::EVENT_KEY_W(float delta)
	{
		if (!_is_default)
			return false;
		_pawns[_control_player_index]->MoveUp(delta);
		return true;
	}

	bool IPlayerController2D::EVENT_KEY_A(float delta)
	{
		if (!_is_default)
			return false;
		_pawns[_control_player_index]->MoveLeft(delta);
		return true;
	}

	bool IPlayerController2D::EVENT_KEY_S(float delta)
	{
		if (!_is_default)
			return false;
		_pawns[_control_player_index]->MoveDown(delta);
		return true;
	}

	bool IPlayerController2D::EVENT_KEY_D(float delta)
	{
		if (!_is_default)
			return false;
		_pawns[_control_player_index]->MoveRight(delta);
		return true;
	}

}