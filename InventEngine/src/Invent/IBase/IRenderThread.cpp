#include "IEpch.h"
#include "IRenderThread.h"

#include "Invent/IEngine.h"
#include "IWindow.h"
#include "VulkanBase.h"
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
	bool IRenderThread::Start()
	{
		_running = true;
		std::promise<bool> initDonePromise;
		std::future<bool> initDoneFuture = initDonePromise.get_future();
		_thread = new std::thread([this, &initDonePromise]() {
			glfwMakeContextCurrent(_iwindow.GetGLFWWindow());

			if (_init_opengl() != 0)
			{
				initDonePromise.set_value(false);
				return;
			}
			initDonePromise.set_value(true);

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

		initDoneFuture.wait();
		return initDoneFuture.get();
	}
#elif USE_VULKAN
	bool IRenderThread::Start()
	{
		_running = true;

		if (_init_vulkan() != 0)
		{
			INVENT_LOG_ERROR(" [ IRenderThread ] Init Vulkan error \n");
			return false;
		}

		_thread = new std::thread([this]() -> bool {


			return true;
			});

		return true;
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

#ifdef USE_VULKAN

		VulkanBase::Base().CleanUp();
#endif // USE_VULKAN


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

#elif defined(USE_VULKAN)

	int IRenderThread::_init_vulkan()
	{
		// 失败则返回nullptr，并意味着此设备不支持Vulkan。
		uint32_t extensionCount = 0;
		const char** extensionNames;
		extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
		if (!extensionNames)
		{
			INVENT_LOG_TRACE(std::format("[ IRenderThread ] Vulkan is not available on this machine!\n"));
			return -1;
		}
		for (size_t i = 0; i < extensionCount; i++)
		{

			VulkanBase::Base().AddInstanceExtension(extensionNames[i]);
		}

		VulkanBase::Base().AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		VulkanBase::Base().CreateVulkanInstance();

		// 创建window surface
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		if (VkResult result = glfwCreateWindowSurface(VulkanBase::Base().GetVkInstance(), _iwindow.GetGLFWWindow(), nullptr, &surface))
		{
			std::cout << std::format("[ InitializeWindow ] ERROR\nFailed to create a window surface!\nError code: {}\n", int32_t(result));
			glfwTerminate();
			return -1;
		}
		VulkanBase::Base().SetSurface(surface);

		// start init vulkan
		if (!VulkanBase::Base().PickPhysicalDevice() ||
			!VulkanBase::Base().CreateLogicalDevice() ||
			!VulkanBase::Base().CreateSwapChain() ||
			!VulkanBase::Base().CreateSwapChainImageView() ||
			!VulkanBase::Base().CreateVmaAllocator() ||
			!VulkanBase::Base().FindDepthFormat() ||
			!VulkanBase::Base().InitializeAllOffscreenPasses() ||
			!VulkanBase::Base().CreateBindlessDescriptorPool())
		{
			return -1;
		}

		VulkanBase::Base().WaitForWindowEvents = []() {
			glfwWaitEvents();
			};

		if (!VulkanBase::Base().Version_1_3_OrHigher())
		{
			if (!VulkanBase::Base().CreateDefaultRenderPasses() ||
				!VulkanBase::Base().CreateDefaultFramebuffers())
			{
				return -1;
			}
		}


		return 0;
	}

	void IRenderThread::_vulkan_render()
	{}

#endif // USE_OPENGL

}