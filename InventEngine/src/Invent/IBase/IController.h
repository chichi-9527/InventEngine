#ifndef _ICONTROLLER_
#define _ICONTROLLER_

#include "IEventLayer.h"
#include "IBasePawnControl.h"
#include "ICamera.h"

namespace INVENT
{

	class IControllerBase : public IEventLayer 
	{
	public:
		IControllerBase();
		virtual ~IControllerBase() = default;


	};

	class IController : public IControllerBase
	{
	public:
		IController();
		virtual ~IController();


	private:


	};

	class IController2D : public IControllerBase
	{
	public:
		IController2D();
		virtual ~IController2D();

		void SetSceneCamera(ICamera* camera) { _scene_camera = camera; }
		const ICamera* GetSceneCamera() const { return _scene_camera; }

		void AddPlayer(IBasePawnControl2D* pawn);
		void ErasePlayer(IBasePawnControl2D* pawn);

		void SetControlPlayerIndex(size_t index) { _control_player_index = index; }
		size_t GetControlPlayerIndex() const { return _control_player_index; }

		const std::vector<IBasePawnControl2D*>& Get2DPlayers() const { return _pawns; }
		const IBasePawnControl2D* Get2DPlayerController(size_t index) const;
		IBasePawnControl2D* Get2DPlayerController(size_t index);

	private:
		std::vector<IBasePawnControl2D*> _pawns;

		size_t _control_player_index;

		ICamera* _scene_camera;
	};

}



#endif // !_ICONTROLLER_
