#ifndef _ICONTROLLER_
#define _ICONTROLLER_

#include "IEventLayer.h"
#include "ICamera.h"

namespace INVENT
{
	class ISquare2dPawn;
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

		void UseControlPawnDefault(bool is_default = true) { _is_default = is_default; }

		virtual void SetSceneCamera(ICamera* camera) override;
		virtual const ICamera* GetSceneCamera() const override { return _scene_camera; }
		virtual void UpdateCameraWindowAspect() const override { if (_scene_camera) _scene_camera->UpdateWindowAspect(); }

		size_t AddPlayer(ISquare2dPawn* pawn);
		void ErasePlayer(ISquare2dPawn* pawn);

		size_t GetPlayerIndex(ISquare2dPawn* pawn);

		bool SetControlPlayer(ISquare2dPawn* pawn);
		bool SetControlPlayerIndex(size_t index);
		size_t GetControlPlayerIndex() const { return _control_player_index; }

		const std::vector<ISquare2dPawn*>& Get2DPlayers() const { return _pawns; }
		const ISquare2dPawn* Get2DPlayerController(size_t index) const;
		ISquare2dPawn* Get2DPlayerController(size_t index);

		template<typename T>
		const T* Get2DPlayerController(size_t index) const
		{
			if (std::is_base_of_v<ISquare2dPawn, T>)
			{
				return (T*)Get2DPlayerController(index);
			}
			return nullptr;
		}
		template<typename T>
		T* Get2DPlayerController(size_t index)
		{
			if (std::is_base_of_v<ISquare2dPawn, T>)
			{
				return (T*)Get2DPlayerController(index);
			}
			return nullptr;
		}

		virtual void EventUp(float delta);
		virtual void EventLeft(float delta);
		virtual void EventDown(float delta);
		virtual void EventRight(float delta);

	private:
		std::vector<ISquare2dPawn*> _pawns;

		size_t _control_player_index;

		ICamera* _scene_camera;

		bool _is_default = true;
	};

}



#endif // !_ICONTROLLER_
