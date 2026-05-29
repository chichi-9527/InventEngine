#include "IEpch.h"
#include "IRenderThread.h"

#include "Invent/IEngine.h"
#include "IWindow.h"
#include "IScene.h"
#include "IController.h"

#include "Invent/2D/ITileMap.h"

#include "Invent/Renderer/IRenderer.h"
#include "Invent/Renderer/IRenderer2D.h"

namespace INVENT
{
#if defined(_WIN32) && defined(USE_OPENGL)
	// Source - https://stackoverflow.com/a/589232
	// Posted by eugensk, modified by community. See post 'Timeline' for change history
	// Retrieved 2025-12-20, License - CC BY-SA 4.0

	static bool WGLExtensionSupported(const char* extension_name)
	{
		// this is pointer to function which returns pointer to string with list of all wgl extensions
		PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

		// determine pointer to wglGetExtensionsStringEXT function
		_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

		if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
		{
			// string was not found
			return false;
		}

		// extension is supported
		return true;
	}

	static PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
	static PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

	static void InitWGL()
	{

		if (WGLExtensionSupported("WGL_EXT_swap_control"))
		{
			// Extension is supported, init pointers.
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

			// this is another function from WGL_EXT_swap_control extension
			wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
		}
	}

#endif // defined(_WIN32) && defined(USE_OPENGL)

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	IRenderThread::IRenderThread(IWindow& iwindow)
		: _iwindow(iwindow)
		, _thread(nullptr)
		, _running(false)
	{
	}

	IRenderThread::~IRenderThread()
	{
		ShutDown();
	}

	std::shared_ptr<IRenderThread> IRenderThread::InstancePtr(IWindow& iwindow)
	{
		static auto rthread = std::shared_ptr<IRenderThread>(new IRenderThread(iwindow));
		return rthread;
	}

#ifdef USE_OPENGL
	static std::queue<std::function<void()>> OpenglInitFuncs;
	static std::mutex OpenglInitFuncsMutex;
	void IRenderThread::Start()
	{
		_running = true;
		_thread = new std::thread([this]() {
			glfwMakeContextCurrent(_iwindow.GetGLFWWindow());

			if (_init_opengl())
			{
				return;
			}

			IRenderer::Init();

			while (_running)
			{
				// call opengl funcs
				{
					std::lock_guard<std::mutex> lock(OpenglInitFuncsMutex);
					while (!OpenglInitFuncs.empty())
					{
						if (auto& func = OpenglInitFuncs.front())
						{
							func();
						}
						OpenglInitFuncs.pop();
					}
				}

				// init textures
				while (!TEXTURE_MANAGEMENT::GetUninitTextureFuncs().empty())
				{
					if (auto& func = TEXTURE_MANAGEMENT::GetUninitTextureFuncs().front())
					{
						func();
					}
					TEXTURE_MANAGEMENT::GetUninitTextureFuncs().pop();
				}

				// 
				glClearColor(_clear_color_vec.r, _clear_color_vec.g, _clear_color_vec.b, _clear_color_vec.a);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// render 
				this->_opengl_render();
				
				//
				glfwSwapBuffers(_iwindow.GetGLFWWindow());
			}

			IRenderer::Shutdown();

			glfwMakeContextCurrent(nullptr);
			});
	}
#elif USE_VULKAN
	void IRenderThread::Start()
	{

	}
#else
#error "must to use opengl or vulkan"
#endif // USE_OPENGL

	void IRenderThread::ShutDown()
	{
		_running = false;
		if (_thread && _thread->joinable())
		{
			_thread->join();
			delete _thread;
			_thread = nullptr;
		}
	}

	void IRenderThread::Viewport(int width, int height, int x, int y)
	{
#ifdef USE_OPENGL
		OpenglInitFuncs.push([x, y, width, height]() {
			glViewport(x, y, width, height);
			});
#endif // USE_OPENGL
	}
	void IRenderThread::SetBackgroundColor(float red, float green, float blue, float alpha)
	{
		_clear_color_vec.r = red;
		_clear_color_vec.g = green;
		_clear_color_vec.b = blue;
		_clear_color_vec.a = alpha;
	}

	void IRenderThread::SetBackgroundColor(glm::vec4 color)
	{
		_clear_color_vec = color;
	}

#ifdef USE_OPENGL
	void IRenderThread::SubmitOpenglInitFuncs(std::function<void()>&& func)
	{
		return OpenglInitFuncs.push(std::forward<std::function<void()>>(func));
	}


	int IRenderThread::_init_opengl()
	{
		if (_iwindow.GetGLFWWindow())
		{
			glfwMakeContextCurrent(IEngine::InstancePtr()->GetIWindow()->GetGLFWWindow());
#if defined(_WIN32)
			InitWGL();
			if (wglGetSwapIntervalEXT() != SwapIntervalEXT)
				wglSwapIntervalEXT(SwapIntervalEXT);
			INVENT_LOG_DEBUG(std::format("当前 swap interval: {} ", wglGetSwapIntervalEXT()));
#endif // defined(_WIN32)

			if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				glViewport(0, 0, _iwindow.GetWidth(), _iwindow.GetHeight());

				GLint max_combined;       // 总可用纹理单元数
				GLint max_fragment;       // 片段着色器可用数
				GLint max_vertex;         // 顶点着色器可用数
				GLint max_geometry;       // 几何着色器可用数
				GLint max_compute;        // 计算着色器可用数

				glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined);
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_fragment);
				glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_vertex);
				glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &max_geometry);
				glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_compute);

				INVENT_LOG_INFO(std::format("总可用纹理单元数: {} ", max_combined));
				INVENT_LOG_INFO(std::format("片段着色器可用数: {} ", max_fragment));
				INVENT_LOG_INFO(std::format("顶点着色器可用数: {} ", max_vertex));
				INVENT_LOG_INFO(std::format("几何着色器可用数: {} ", max_geometry));
				INVENT_LOG_INFO(std::format("计算着色器可用数: {} ", max_compute));
			}
			else
			{
				INVENT_LOG_ERROR("Failed to initialize GLAD");
				return -1;
			}
			return 0;
		}
		return -1;
	}
#endif // USE_OPENGL

	void IRenderThread::_opengl_render()
	{
		IRenderer2D::BeginRender(IEngine::InstancePtr()->GetMainScene()->GetController().lock() ? 
			IEngine::InstancePtr()->GetMainScene()->GetController().lock()->GetSceneCamera() : nullptr);

		auto& show_vectors = IEngine::InstancePtr()->GetMainScene()->GetShowLevelActorVectors();

		for (auto square_actor : show_vectors.Square2dActors)
		{
			IRenderer2D::DrawSquare(square_actor);
		}
		for (auto tile_map : show_vectors.TileMaps)
		{
			for (auto& square : tile_map->GetSquares())
			{
				IRenderer2D::DrawSquare(&square);
			}
		}
		IRenderer2D::DrawWString(std::wstring(L"fps: no"), { 0.5f, 0.8f, 0.2f, 1.0f }, { 0.8f, 0.9f }, 32.0f, 1);
		IRenderer2D::EndRender();
	}

}