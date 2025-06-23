#ifndef _ICONTROLLER_
#define _ICONTROLLER_

#include "IEventLayer.h"
#include "IBasePawnControl.h"
#include "ICamera.h"

namespace INVENT
{

	class IPlayerControllerBase : public IEventLayer 
	{
	public:
		IPlayerControllerBase();
		virtual ~IPlayerControllerBase() = default;

		virtual void  SetSceneCamera(ICamera* camera) = 0;
		virtual const ICamera* GetSceneCamera() const = 0;
		virtual void UpdateCameraWindowAspect() const = 0;
	};

	class IController : public IPlayerControllerBase
	{
	public:
		IController();
		virtual ~IController();


	private:


	};

	class IPlayerController2D : public IPlayerControllerBase
	{
	public:
		IPlayerController2D();
		virtual ~IPlayerController2D();

		void UseDefaultController(bool is_default = true) { _is_default = is_default; }

		virtual void SetSceneCamera(ICamera* camera) override;
		virtual const ICamera* GetSceneCamera() const override { return _scene_camera; }
		virtual void UpdateCameraWindowAspect() const override { if (_scene_camera) _scene_camera->UpdateWindowAspect(); }

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
