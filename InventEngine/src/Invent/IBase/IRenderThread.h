#ifndef _IRENDERTHREAD_
#define _IRENDERTHREAD_

#include <thread>
#include <memory>
#include <atomic>

namespace INVENT
{
	class IWindow;

	class IRenderThread
	{
	public:
		~IRenderThread();
		static std::shared_ptr<IRenderThread> InstancePtr(IWindow& iwindow);

		void Start();
		void ShutDown();

		void Viewport(int width, int height, int x = 0, int y = 0);

	private:
		IRenderThread(IWindow& iwindow);

#ifdef USE_OPENGL
		int _init_opengl();
#endif

	private:
		std::thread* _thread;
		IWindow& _iwindow;

		// 限制帧率为与显示器帧率的反比，0 为不限制
		// 仅 Windows 下有效
		int SwapIntervalEXT = 1;

		std::atomic_bool _running;
	};

}

#endif // !_IRENDERTHREAD_

