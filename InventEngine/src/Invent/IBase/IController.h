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

		virtual void  SetSceneCamera(ICamera* camera) = 0;
		virtual const ICamera* GetSceneCamera() const = 0;
	};

	class IController : public IControllerBase
	{
	public:
		IController();
		virtual ~IController();


	private:


	};

	class IPlayerController2D : public IControllerBase
	{
	public:
		IPlayerController2D();
		virtual ~IPlayerController2D();

		void UseDefaultController(bool is_default = true) { _is_default = is_default; }

		virtual void SetSceneCamera(ICamera* camera) override { _scene_camera = camera; }
		virtual const ICamera* GetSceneCamera() const override { return _scene_camera; }

		void AddPlayer(IBasePawnControl2D* pawn);
		void ErasePlayer(IBasePawnControl2D* pawn);

		void SetControlPlayerIndex(size_t index) { _control_player_index = index; }
		size_t GetControlPlayerIndex() const { return _control_player_index; }

		const std::vector<IBasePawnControl2D*>& Get2DPlayers() const { return _pawns; }
		const IBasePawnControl2D* Get2DPlayerController(size_t index) const;
		IBasePawnControl2D* Get2DPlayerController(size_t index);

		virtual bool EVENT_KEY_W(float delta) override;
		virtual bool EVENT_KEY_A(float delta) override;
		virtual bool EVENT_KEY_S(float delta) override;
		virtual bool EVENT_KEY_D(float delta) override;

	private:
		std::vector<IBasePawnControl2D*> _pawns;

		size_t _control_player_index;

		ICamera* _scene_camera;

		bool _is_default = true;
	};

}



#endif // !_ICONTROLLER_
