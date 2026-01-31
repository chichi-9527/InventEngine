#include "IEpch.h"
#include "IWindow.h"

#include "Renderer/IRenderer.h"
#include "Renderer/IRenderer2D.h"

#include "IPhysicsCollision//ICollisionPresets.h"
#include "2D/2DComponent/Invent2DAnimationComponent.h"

#include "IEngine.h"
#include "IGameInstance.h"
#include "ThreadPool/IThreadPool.h"

#include "UI/IUIImgui.h"
#include "UI/IDrawString.h"

#include <chrono>


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


	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		IWindow* iwindow = static_cast<IWindow*>(glfwGetWindowUserPointer(window));
#ifdef USE_OPENGL
		glViewport(0, 0, width, height);
#endif // USE_OPENGL
		if (iwindow)
		{
			iwindow->Width = width;
			iwindow->Height = height;

			iwindow->_window_size_change();
		}
	}

	void register_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		IWindow* iwindow = static_cast<IWindow*>(glfwGetWindowUserPointer(window));
		if (iwindow)
		{
			if (action == GLFW_PRESS)
			{
				for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
				{
					if ((*iter)->PRESS_EVENT(key))
						break;
				}
			}
			else if (action == GLFW_RELEASE)
			{
				for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
				{
					if ((*iter)->RELEASE_EVENT(key))
						break;
				}
			}
		}
	}

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		IWindow* iwindow = static_cast<IWindow*>(glfwGetWindowUserPointer(window));
		if (iwindow)
		{
			for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
			{
				if ((*iter)->EVENT_CURSOR_POSITION(xpos, ypos))
					break;
			}
		}
	}

//#define GLFW_MOUSE_BUTTON_1         0
//#define GLFW_MOUSE_BUTTON_2         1
//#define GLFW_MOUSE_BUTTON_3         2
//#define GLFW_MOUSE_BUTTON_4         3
//#define GLFW_MOUSE_BUTTON_5         4
//#define GLFW_MOUSE_BUTTON_6         5
//#define GLFW_MOUSE_BUTTON_7         6
//#define GLFW_MOUSE_BUTTON_8         7
//#define GLFW_MOUSE_BUTTON_LAST      GLFW_MOUSE_BUTTON_8
//#define GLFW_MOUSE_BUTTON_LEFT      GLFW_MOUSE_BUTTON_1
//#define GLFW_MOUSE_BUTTON_RIGHT     GLFW_MOUSE_BUTTON_2
//#define GLFW_MOUSE_BUTTON_MIDDLE    GLFW_MOUSE_BUTTON_3

	static double xpos = 0.0, ypos = 0.0;
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		IWindow* iwindow = static_cast<IWindow*>(glfwGetWindowUserPointer(window));
		if (iwindow)
		{
			
			glfwGetCursorPos(window, &xpos, &ypos);

			if (action == GLFW_PRESS)
			{
				if (button == GLFW_MOUSE_BUTTON_1)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->PRESS_EVENT_MOUSE_BUTTON_LEFT(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_2)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->PRESS_EVENT_MOUSE_BUTTON_RIGHT(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_3)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->PRESS_EVENT_MOUSE_BUTTON_MIDDLE(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_4)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->PRESS_EVENT_MOUSE_BUTTON_4(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_5)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->PRESS_EVENT_MOUSE_BUTTON_5(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_6)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->PRESS_EVENT_MOUSE_BUTTON_6(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_7)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->PRESS_EVENT_MOUSE_BUTTON_7(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_8)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->PRESS_EVENT_MOUSE_BUTTON_8(xpos, ypos))
							break;
					}
				}
			}
			else if (action == GLFW_RELEASE)
			{
				if (button == GLFW_MOUSE_BUTTON_1)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->RELEASE_EVENT_MOUSE_BUTTON_LEFT(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_2)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->RELEASE_EVENT_MOUSE_BUTTON_RIGHT(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_3)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->RELEASE_EVENT_MOUSE_BUTTON_MIDDLE(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_4)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->RELEASE_EVENT_MOUSE_BUTTON_4(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_5)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->RELEASE_EVENT_MOUSE_BUTTON_5(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_6)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->RELEASE_EVENT_MOUSE_BUTTON_6(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_7)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->RELEASE_EVENT_MOUSE_BUTTON_7(xpos, ypos))
							break;
					}
				}
				else if (button == GLFW_MOUSE_BUTTON_8)
				{
					for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
					{
						if ((*iter)->RELEASE_EVENT_MOUSE_BUTTON_8(xpos, ypos))
							break;
					}
				}



			} // if (action ==

		} // if (iwindow)
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		IWindow* iwindow = static_cast<IWindow*>(glfwGetWindowUserPointer(window));
		if (iwindow)
		{
			double xpos = 0.0, ypos = 0.0;
			glfwGetCursorPos(window, &xpos, &ypos);

			for (auto iter = iwindow->Level->_event_layers.rbegin(); iter != iwindow->Level->_event_layers.rend(); ++iter)
			{
				if ((*iter)->EVNET_SCROLL(xpos, ypos, xoffset, yoffset))
					break;
			}
		}
	}

	void cursor_enter_callback(GLFWwindow* window, int entered)
	{
		IWindow* iwindow = static_cast<IWindow*>(glfwGetWindowUserPointer(window));
		if (iwindow)
		{
			iwindow->CursorInsideWindow = (entered == GLFW_TRUE);
		}
	}

	IWindow::IWindow(unsigned int width, unsigned int height, std::string title)
		: Width(width)
		, Height(height)
		, Title(title)
		, Window(nullptr)
		, Monitor(nullptr)
		, _default_level(nullptr)
		, Level(nullptr)
		, delta_time(0.0f)
	{
		ILog::Instance().IINFO("Log init done");
	}

	void IWindow::Begin()
	{
		_create();
		if (nullptr == Level)
		{
			_default_level = new IBaseLevel;
			Level = _default_level;
		}
	
		UI::IDrawString::Init("./Assets/TTF/VictorMono-Bold-2.otf");
		UI::IDrawString::Init({ "./Assets/TTF/huawencaiyun.ttf", "./Assets/TTF/huawenfangsong.ttf" });
	}

	IWindow::~IWindow()
	{
		UI::IDrawString::Shutdown();

		if (_default_level)
			delete _default_level;
		_default_level = nullptr;

		glfwTerminate();
	}

	// ----------------------Start--------------------------------------
	void IWindow::Start()
	{
		Begin();

		ICollisionPresets::Init();
		IRenderer::Init();
		IUIImgui::Init(Window);
		AnimationManagement::Start();
	
		IEngine::InstancePtr()->GetGameInstance()->Begin();
		

		///////////////////////////////////////////////////////////
		////////// Begin Loop
		//////////////////////////////////////////////////////////

		//last_frame = static_cast<float>(glfwGetTime());
		last_frame = std::chrono::high_resolution_clock::now();
		while (!glfwWindowShouldClose(Window))
		{
			auto current_frame = std::chrono::high_resolution_clock::now();
			delta_time = std::chrono::duration<float>(current_frame - last_frame).count();
			last_frame = current_frame;

			// init textures
			while(!TEXTURE_MANAGEMENT::GetUninitTextures().empty())
			{
				TEXTURE_MANAGEMENT::GetUninitTextures().front()->InitTextureID();
				TEXTURE_MANAGEMENT::GetUninitTextures().pop();
			}

			// init other functions
			while (!_main_thread_init_queue.empty())
			{
				auto& func = _main_thread_init_queue.front();
				if (func)
				{
					func();
				}
				_main_thread_init_queue.pop();
			}

			IEngine::InstancePtr()->GetGameInstance()->Update(delta_time);

			IUIImgui::StartFrame();

			//INVENT_LOG_DEBUG(std::to_string(delta_time));

			Level->_clear_color();
			Level->_clear();
			Level->Update(delta_time);

			///////////////////////////////////////////////////////////
			///////////// Render Begin

			IRenderer::BeginRender(Level->GetController() ? Level->GetController()->GetSceneCamera() : nullptr);
			Render3d();
			IRenderer::EndRender();

			IRenderer2D::BeginRender(Level->GetController() ? Level->GetController()->GetSceneCamera() : nullptr);
			IWindow::Render(Level);
			//IRenderer2D::DrawWString(std::wstring(L"fps:") + std::to_wstring(int(1.0f / delta_time)), { 0.5f, 0.8f, 0.2f, 1.0f }, { 0.8f, 0.9f }, 32.0f, 1);
			IRenderer2D::EndRender();

			///////////// Render End
			//////////////////////////////////////////////////////////

			//_process_input(delta_time);
			_process_input_callback(delta_time);

			IUIImgui::Render();

			// 检查并调用事件，交换缓冲
			glfwSwapBuffers(Window);
			glfwPollEvents();
		}

		IEngine::InstancePtr()->GetGameInstance()->End();
		AnimationManagement::Shutdown();
		IUIImgui::End();
		
		IRenderer::Shutdown();
	}

	void IWindow::SetWindowSize(unsigned int width, unsigned int height) const
	{
		glfwSetWindowSize(Window, (int)width, (int)height);
	}

	void IWindow::Close() const
	{
		glfwSetWindowShouldClose(Window, true);
	}

	bool IWindow::IsFullScreen()
	{
		return glfwGetWindowMonitor(Window) != nullptr;
	}

	void IWindow::SetFullScreen(bool fullscreen)
	{
		if (IsFullScreen() == fullscreen) return;
		if (fullscreen)
		{
			glfwGetWindowPos(Window, &_pos_size[0], &_pos_size[1]);
			glfwGetWindowSize(Window, &_pos_size[2], &_pos_size[3]);
			auto mode = glfwGetVideoMode(Monitor);
			glfwSetWindowMonitor(Window, Monitor, 0, 0, mode->width, mode->height, 0);
		}
		else
		{
			glfwSetWindowMonitor(Window, nullptr, _pos_size[0], _pos_size[1], _pos_size[2], _pos_size[3], 0);
		}

	}

	void IWindow::SetLevel(IBaseLevel* level)
	{
		if (level)
			Level = level;
	}



	// debug
	struct quad_vertex {
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////
	////////////  渲染
	/////////////////////////////////////////////////////////////////////////////////////////////////
	void IWindow::Render(IBaseLevel* level)
	{
		
		for (auto square_actor : level->_square_2d_actors)
		{
			IRenderer2D::DrawSquare(square_actor);
		}
		for (auto tile_map : level->_tile_map_actors)
		{
			for (auto& square : tile_map->GetSquares())
			{
				IRenderer2D::DrawSquare(&square);
			}
		}

		IRenderer2D::DrawString("abcdefghijk lmnopqrstuvwxyz", { 0.5f, 0.8f, 0.2f, 1.0f }, { 25.0f, 25.0f }, 32.0f, 0);
		IRenderer2D::DrawWString(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", { 0.5f, 0.8f, 0.2f, 1.0f }, { 25.0f, 225.0f }, 32.0f, 0);
		IRenderer2D::DrawWString(L"你好，世 界", { 0.5f, 0.8f, 0.2f, 1.0f }, { 25.0f, 100.0f }, 32.0f, 0, 0);
		

	}

	void IWindow::_create()
	{
		if (-1 == _glfw_init())
			return;
		//glfwSwapInterval(0);
		Window = glfwCreateWindow(Width, Height, Title.c_str(), NULL, NULL);
		if (Window)
		{
			glfwMakeContextCurrent(Window);
#if defined(_WIN32) && defined(USE_OPENGL)
			InitWGL();
			if (wglGetSwapIntervalEXT() != SwapIntervalEXT)
				wglSwapIntervalEXT(SwapIntervalEXT);
			INVENT_LOG_DEBUG(std::format("当前 swap interval: {} ", wglGetSwapIntervalEXT()));
#endif // defined(_WIN32) && defined(USE_OPENGL)
			glfwSetWindowUserPointer(Window, this);
#ifdef USE_OPENGL
			if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				glViewport(0, 0, Width, Height);

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
				INVENT_LOG_ERROR("Failed to initialize GLAD");
#endif // USE_OPENGL
			glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);
			glfwSetKeyCallback(Window, register_key_callback);
			glfwSetCursorPosCallback(Window, cursor_position_callback);
			glfwSetMouseButtonCallback(Window, mouse_button_callback);
			glfwSetScrollCallback(Window, scroll_callback);
			glfwSetCursorEnterCallback(Window, cursor_enter_callback);

			Monitor = glfwGetPrimaryMonitor();
		}
		else
		{
			INVENT_LOG_ERROR("Failed to create GLFW window");
		}
	}

	int IWindow::_glfw_init()
	{
		if (GLFW_FALSE == glfwInit())
		{
			INVENT_LOG_ERROR("glfwInit() error");
			return -1;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef USE_OPENGL
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // USE_OPENGL
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		return 0;
	}

	void IWindow::_window_size_change()
	{
		Level->_window_size = { (float)Width , (float)Height };
		if (Level->GetController())
		{
			Level->GetController()->UpdateCameraWindowAspect();
		}
	}

#define CALLEVENT(GLFW_KEY, EVENT_FUNC_NAME)	if (glfwGetKey(Window, GLFW_KEY) == (GLFW_PRESS | GLFW_RELEASE)){	\
		for (auto iter = Level->_event_layers.rbegin(); iter != Level->_event_layers.rend(); ++iter){	\
			auto takeoff = (*iter)->EVENT_FUNC_NAME(delta);	\
			if (takeoff) break;	}}

	// 已废弃 现在通过注册回调函数
	void IWindow::_process_input(float delta)
	{
		CALLEVENT(GLFW_KEY_SPACE, EVENT_KEY_SPACE);
		CALLEVENT(GLFW_KEY_APOSTROPHE, EVENT_KEY_APOSTROPHE);
		CALLEVENT(GLFW_KEY_COMMA, EVENT_KEY_COMMA);
		CALLEVENT(GLFW_KEY_MINUS, EVENT_KEY_MINUS);
		CALLEVENT(GLFW_KEY_PERIOD, EVENT_KEY_PERIOD);
		CALLEVENT(GLFW_KEY_SLASH, EVENT_KEY_SLASH);
		CALLEVENT(GLFW_KEY_0, EVENT_KEY_0);
		CALLEVENT(GLFW_KEY_1, EVENT_KEY_1);
		CALLEVENT(GLFW_KEY_2, EVENT_KEY_2);
		CALLEVENT(GLFW_KEY_3, EVENT_KEY_3);
		CALLEVENT(GLFW_KEY_4, EVENT_KEY_4);
		CALLEVENT(GLFW_KEY_5, EVENT_KEY_5);
		CALLEVENT(GLFW_KEY_6, EVENT_KEY_6);
		CALLEVENT(GLFW_KEY_7, EVENT_KEY_7);
		CALLEVENT(GLFW_KEY_8, EVENT_KEY_8);
		CALLEVENT(GLFW_KEY_9, EVENT_KEY_9);
		CALLEVENT(GLFW_KEY_SEMICOLON, EVENT_KEY_SEMICOLON);
		CALLEVENT(GLFW_KEY_EQUAL, EVENT_KEY_EQUAL);
		CALLEVENT(GLFW_KEY_A, EVENT_KEY_A);
		CALLEVENT(GLFW_KEY_B, EVENT_KEY_B);
		CALLEVENT(GLFW_KEY_C, EVENT_KEY_C);
		CALLEVENT(GLFW_KEY_D, EVENT_KEY_D);
		CALLEVENT(GLFW_KEY_E, EVENT_KEY_E);
		CALLEVENT(GLFW_KEY_F, EVENT_KEY_F);
		CALLEVENT(GLFW_KEY_G, EVENT_KEY_G);
		CALLEVENT(GLFW_KEY_H, EVENT_KEY_H);
		CALLEVENT(GLFW_KEY_I, EVENT_KEY_I);
		CALLEVENT(GLFW_KEY_J, EVENT_KEY_J);
		CALLEVENT(GLFW_KEY_K, EVENT_KEY_K);
		CALLEVENT(GLFW_KEY_L, EVENT_KEY_L);
		CALLEVENT(GLFW_KEY_M, EVENT_KEY_M);
		CALLEVENT(GLFW_KEY_N, EVENT_KEY_N);
		CALLEVENT(GLFW_KEY_O, EVENT_KEY_O);
		CALLEVENT(GLFW_KEY_P, EVENT_KEY_P);
		CALLEVENT(GLFW_KEY_Q, EVENT_KEY_Q);
		CALLEVENT(GLFW_KEY_R, EVENT_KEY_R);
		CALLEVENT(GLFW_KEY_S, EVENT_KEY_S);
		CALLEVENT(GLFW_KEY_T, EVENT_KEY_T);
		CALLEVENT(GLFW_KEY_U, EVENT_KEY_U);
		CALLEVENT(GLFW_KEY_V, EVENT_KEY_V);
		CALLEVENT(GLFW_KEY_W, EVENT_KEY_W);
		CALLEVENT(GLFW_KEY_X, EVENT_KEY_X);
		CALLEVENT(GLFW_KEY_Y, EVENT_KEY_Y);
		CALLEVENT(GLFW_KEY_Z, EVENT_KEY_Z);
		CALLEVENT(GLFW_KEY_LEFT_BRACKET, EVENT_KEY_LEFT_BRACKET);
		CALLEVENT(GLFW_KEY_BACKSLASH, EVENT_KEY_BACKSLASH);
		CALLEVENT(GLFW_KEY_RIGHT_BRACKET, EVENT_KEY_RIGHT_BRACKET);
		CALLEVENT(GLFW_KEY_GRAVE_ACCENT, EVENT_KEY_GRAVE_ACCENT);
		CALLEVENT(GLFW_KEY_WORLD_1, EVENT_KEY_WORLD_1);
		CALLEVENT(GLFW_KEY_WORLD_2, EVENT_KEY_WORLD_2);
		CALLEVENT(GLFW_KEY_ESCAPE, EVENT_KEY_ESCAPE);
		CALLEVENT(GLFW_KEY_ENTER, EVENT_KEY_ENTER);
		CALLEVENT(GLFW_KEY_TAB, EVENT_KEY_TAB);
		CALLEVENT(GLFW_KEY_BACKSPACE, EVENT_KEY_BACKSPACE);
		CALLEVENT(GLFW_KEY_INSERT, EVENT_KEY_INSERT);
		CALLEVENT(GLFW_KEY_DELETE, EVENT_KEY_DELETE);
		CALLEVENT(GLFW_KEY_RIGHT, EVENT_KEY_RIGHT);
		CALLEVENT(GLFW_KEY_LEFT, EVENT_KEY_LEFT);
		CALLEVENT(GLFW_KEY_DOWN, EVENT_KEY_DOWN);
		CALLEVENT(GLFW_KEY_UP, EVENT_KEY_UP);
		CALLEVENT(GLFW_KEY_PAGE_UP, EVENT_KEY_PAGE_UP);
		CALLEVENT(GLFW_KEY_PAGE_DOWN, EVENT_KEY_PAGE_DOWN);
		CALLEVENT(GLFW_KEY_HOME, EVENT_KEY_HOME);
		CALLEVENT(GLFW_KEY_END, EVENT_KEY_END);
		CALLEVENT(GLFW_KEY_CAPS_LOCK, EVENT_KEY_CAPS_LOCK);
		CALLEVENT(GLFW_KEY_SCROLL_LOCK, EVENT_KEY_SCROLL_LOCK);
		CALLEVENT(GLFW_KEY_NUM_LOCK, EVENT_KEY_NUM_LOCK);
		CALLEVENT(GLFW_KEY_PRINT_SCREEN, EVENT_KEY_PRINT_SCREEN);
		CALLEVENT(GLFW_KEY_PAUSE, EVENT_KEY_PAUSE);
		CALLEVENT(GLFW_KEY_F1, EVENT_KEY_F1);
		CALLEVENT(GLFW_KEY_F2, EVENT_KEY_F2);
		CALLEVENT(GLFW_KEY_F3, EVENT_KEY_F3);
		CALLEVENT(GLFW_KEY_F4, EVENT_KEY_F4);
		CALLEVENT(GLFW_KEY_F5, EVENT_KEY_F5);
		CALLEVENT(GLFW_KEY_F6, EVENT_KEY_F6);
		CALLEVENT(GLFW_KEY_F7, EVENT_KEY_F7);
		CALLEVENT(GLFW_KEY_F8, EVENT_KEY_F8);
		CALLEVENT(GLFW_KEY_F9, EVENT_KEY_F9);
		CALLEVENT(GLFW_KEY_F10, EVENT_KEY_F10);
		CALLEVENT(GLFW_KEY_F11, EVENT_KEY_F11);
		CALLEVENT(GLFW_KEY_F12, EVENT_KEY_F12);
		CALLEVENT(GLFW_KEY_F13, EVENT_KEY_F13);
		CALLEVENT(GLFW_KEY_F14, EVENT_KEY_F14);
		CALLEVENT(GLFW_KEY_F15, EVENT_KEY_F15);
		CALLEVENT(GLFW_KEY_F16, EVENT_KEY_F16);
		CALLEVENT(GLFW_KEY_F17, EVENT_KEY_F17);
		CALLEVENT(GLFW_KEY_F18, EVENT_KEY_F18);
		CALLEVENT(GLFW_KEY_F19, EVENT_KEY_F19);
		CALLEVENT(GLFW_KEY_F20, EVENT_KEY_F20);
		CALLEVENT(GLFW_KEY_F21, EVENT_KEY_F21);
		CALLEVENT(GLFW_KEY_F22, EVENT_KEY_F22);
		CALLEVENT(GLFW_KEY_F23, EVENT_KEY_F23);
		CALLEVENT(GLFW_KEY_F24, EVENT_KEY_F24);
		CALLEVENT(GLFW_KEY_F25, EVENT_KEY_F25);
		CALLEVENT(GLFW_KEY_KP_0, EVENT_KEY_KP_0);
		CALLEVENT(GLFW_KEY_KP_1, EVENT_KEY_KP_1);
		CALLEVENT(GLFW_KEY_KP_2, EVENT_KEY_KP_2);
		CALLEVENT(GLFW_KEY_KP_3, EVENT_KEY_KP_3);
		CALLEVENT(GLFW_KEY_KP_4, EVENT_KEY_KP_4);
		CALLEVENT(GLFW_KEY_KP_5, EVENT_KEY_KP_5);
		CALLEVENT(GLFW_KEY_KP_6, EVENT_KEY_KP_6);
		CALLEVENT(GLFW_KEY_KP_7, EVENT_KEY_KP_7);
		CALLEVENT(GLFW_KEY_KP_8, EVENT_KEY_KP_8);
		CALLEVENT(GLFW_KEY_KP_9, EVENT_KEY_KP_9);
		CALLEVENT(GLFW_KEY_KP_DECIMAL, EVENT_KEY_KP_DECIMAL);
		CALLEVENT(GLFW_KEY_KP_DIVIDE, EVENT_KEY_KP_DIVIDE);
		CALLEVENT(GLFW_KEY_KP_MULTIPLY, EVENT_KEY_KP_MULTIPLY);
		CALLEVENT(GLFW_KEY_KP_SUBTRACT, EVENT_KEY_KP_SUBTRACT);
		CALLEVENT(GLFW_KEY_KP_ADD, EVENT_KEY_KP_ADD);
		CALLEVENT(GLFW_KEY_KP_ENTER, EVENT_KEY_KP_ENTER);
		CALLEVENT(GLFW_KEY_KP_EQUAL, EVENT_KEY_KP_EQUAL);
		CALLEVENT(GLFW_KEY_LEFT_SHIFT, EVENT_KEY_LEFT_SHIFT);
		CALLEVENT(GLFW_KEY_LEFT_CONTROL, EVENT_KEY_LEFT_CONTROL);
		CALLEVENT(GLFW_KEY_LEFT_ALT, EVENT_KEY_LEFT_ALT);
		CALLEVENT(GLFW_KEY_LEFT_SUPER, EVENT_KEY_LEFT_SUPER);
		CALLEVENT(GLFW_KEY_RIGHT_SHIFT, EVENT_KEY_RIGHT_SHIFT);
		CALLEVENT(GLFW_KEY_RIGHT_CONTROL, EVENT_KEY_RIGHT_CONTROL);
		CALLEVENT(GLFW_KEY_RIGHT_ALT, EVENT_KEY_RIGHT_ALT);
		CALLEVENT(GLFW_KEY_RIGHT_SUPER, EVENT_KEY_RIGHT_SUPER);
		CALLEVENT(GLFW_KEY_MENU, EVENT_KEY_MENU);

		
		glfwGetCursorPos(Window, &cursor_xpos, &cursor_ypos);

		for (auto iter = Level->_event_layers.rbegin(); iter != Level->_event_layers.rend(); ++iter)
		{
			if ((*iter)->EVENT_CURSOR_POSITION_FRAME(delta, CursorInsideWindow, cursor_xpos, cursor_ypos))
				break;
		}
		
	}

	void IWindow::_process_input_callback(float delta)
	{
		glfwGetCursorPos(Window, &cursor_xpos, &cursor_ypos);
		for (auto& func : IEngine::InstancePtr()->_cursor_position_input_callbacks)
		{
			if (func)
			{
				func(delta, CursorInsideWindow, cursor_xpos, cursor_ypos);
			}
		}

		for (auto& [func, key] : IEngine::InstancePtr()->_normal_input_callbacks)
		{
			if (func && glfwGetKey(Window, key) == (GLFW_PRESS | GLFW_RELEASE))
			{
				func(delta);
			}
		}

	}
	
}