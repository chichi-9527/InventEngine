#include "IEpch.h"
#include "IController.h"

namespace INVENT
{

	IPlayerControllerBase::IPlayerControllerBase()
	{
		this->SetAllEventReturn(false);
		this->CursorPositionEventReturn = false;
		this->MouseButtonEventReturn = false;
	}

	IController::IController()
		: IPlayerControllerBase()
	{}
	IController::~IController()
	{}




	IPlayerController2D::IPlayerController2D()
		: IPlayerControllerBase()
		, _scene_camera(nullptr)
		, _control_player_index(0)
	{}

	IPlayerController2D::~IPlayerController2D()
	{

	}

	void IPlayerController2D::SetSceneCamera(ICamera* camera)
	{
		if (camera)
		{
			camera->UpdateWindowAspect();
			_scene_camera = camera;
		}
		
	}

	size_t IPlayerController2D::AddPlayer(IBasePawnControl2D * pawn)
	{
		_pawns.push_back(pawn);
		return _pawns.size() - 1;
	}

	void IPlayerController2D::ErasePlayer(IBasePawnControl2D* pawn)
	{
		_pawns.erase(std::remove(_pawns.begin(), _pawns.end(), pawn), _pawns.end());
	}

	size_t IPlayerController2D::GetPlayerIndex(IBasePawnControl2D* pawn)
	{
		auto iter = std::find(_pawns.begin(), _pawns.end(), pawn);
		return iter - _pawns.begin();
	}

	bool IPlayerController2D::SetControlPlayer(IBasePawnControl2D* pawn)
	{
		return SetControlPlayerIndex(GetPlayerIndex(pawn));
	}

	bool IPlayerController2D::SetControlPlayerIndex(size_t index)
	{
		if (index >= _pawns.size())
			return false;
		_control_player_index = index;
		return true;
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