#include "IEpch.h"
#include "IWindow.h"

#include "Invent/IPhysicsCollision//ICollisionPresets.h"
#include "Invent/2D/2DComponent/Invent2DAnimationComponent.h"

#include "Invent/IEngine.h"
#include "Invent/IEngineTools.h"
#include "IGameInstance.h"
#include "Invent/ThreadPool/IThreadPool.h"
#include "IRenderThread.h"
#include "IBaseLevel.h"
#include "Invent/IBase/IScene.h"

#include "Invent/UI/IUIImgui.h"
#include "Invent/UI/IDrawString.h"

#include <chrono>


namespace INVENT
{
	static GLFWwindow* Window = nullptr;
	static GLFWmonitor* Monitor = nullptr;

	IWindow::IWindow(unsigned int width, unsigned int height, std::string title, bool is_resizable)
		: Width(width)
		, Height(height)
		, Title(title)
		, Level(nullptr)
		, delta_time(0.0f)
	{
		ILog::Instance().IINFO("Log init done");
		_create_window(is_resizable);

		_render_thread = IRenderThread::InstancePtr(*this);
	}

	void IWindow::Begin()
	{
		
		UI::IDrawString::Init("./Assets/TTF/VictorMono-Bold-2.otf");
		UI::IDrawString::Init({ "./Assets/TTF/huawencaiyun.ttf", "./Assets/TTF/huawenfangsong.ttf" });

		IEngineTools::Instance().Init();
		ICollisionPresets::Init();
		AnimationManagement::Start();
		IEngine::InstancePtr()->GetMainScene()->Begin();
		IEngine::InstancePtr()->GetGameInstance()->Begin();
		
	}

	void IWindow::End()
	{
		_render_thread->ShutDown();
		IEngine::InstancePtr()->GetGameInstance()->End();
		IEngine::InstancePtr()->GetMainScene()->End();
		AnimationManagement::Shutdown();
		IEngineTools::Instance().Clear();
		
	}

	IWindow::~IWindow()
	{
		UI::IDrawString::Shutdown();

		glfwTerminate();
	}

	// ----------------------Start--------------------------------------
	bool IWindow::Start()
	{
		Begin();

		///////////////////////////////////////////////////////////
		////////// Begin Loop
		//////////////////////////////////////////////////////////

		if (!_render_thread->Start())
		{
			End();
			return false;
		}

		//last_frame = static_cast<float>(glfwGetTime());
		last_frame = std::chrono::high_resolution_clock::now();
		while (!glfwWindowShouldClose(Window))
		{
			auto current_frame = std::chrono::high_resolution_clock::now();
			delta_time = std::chrono::duration<float>(current_frame - last_frame).count();
			last_frame = current_frame;

			IEngine::InstancePtr()->GetGameInstance()->Update(delta_time);

			//IUIImgui::StartFrame();

			//INVENT_LOG_DEBUG(std::to_string(delta_time));

			IEngine::InstancePtr()->GetMainScene()->Update(delta_time);

			_process_input_callback(delta_time);

			//IUIImgui::Render();

			// 检查并调用事件，交换缓冲
			
			glfwPollEvents();
		}

		End();

		return true;
	}

	void IWindow::SetWindowSize(unsigned int width, unsigned int height) const
	{
		glfwSetWindowSize(Window, (int)width, (int)height);
	}

	GLFWwindow* IWindow::GetGLFWWindow() const
	{
		return Window;
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

	void IWindow::_create_window(bool is_resizable)
	{
		if (-1 == _glfw_init(is_resizable))
			return;
		//glfwSwapInterval(0);
		glfwSetErrorCallback([](int error, const char* description) {
			INVENT_LOG_ERROR(std::format("[GLFW] description : {} \nerrorcode : {}", description, error));
			});

		if (Window = glfwCreateWindow(Width, Height, Title.c_str(), NULL, NULL))
		{
			INVENT_LOG_INFO("[IWindow] Create GLFW window done.");
		}
		else
		{
			INVENT_LOG_ERROR("[IWindow] Failed to create GLFW window!");
			return;
		}

		glfwSetWindowUserPointer(Window, this);

		glfwSetFramebufferSizeCallback(Window, [](GLFWwindow* window, int width, int height){
			if (IWindow* iwindow = (IWindow*)glfwGetWindowUserPointer(window))
			{
				iwindow->_framebuffer_size_callback(width, height);
			}
			});
		glfwSetKeyCallback(Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (IWindow* iwindow = (IWindow*)glfwGetWindowUserPointer(window))
			{
				iwindow->_register_key_callback(key, scancode, action, mods);
			}
			});
		glfwSetCursorPosCallback(Window, [](GLFWwindow* window, double xpos, double ypos) {
			if (IWindow* iwindow = (IWindow*)glfwGetWindowUserPointer(window))
			{
				iwindow->_cursor_position_callback(xpos, ypos);
			}
			});
		glfwSetMouseButtonCallback(Window, [](GLFWwindow* window, int button, int action, int mods) {
			if (IWindow* iwindow = (IWindow*)glfwGetWindowUserPointer(window))
			{
				iwindow->_mouse_button_callback(button, action, mods);
			}
			});
		glfwSetScrollCallback(Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			if (IWindow* iwindow = (IWindow*)glfwGetWindowUserPointer(window))
			{
				iwindow->_scroll_callback(xoffset, yoffset);
			}
			});
		glfwSetCursorEnterCallback(Window, [](GLFWwindow* window, int entered) {
			if (IWindow* iwindow = (IWindow*)glfwGetWindowUserPointer(window))
			{
				iwindow->_cursor_enter_callback(entered);
			}
			});

		if (Monitor = glfwGetPrimaryMonitor())
		{
			INVENT_LOG_INFO("[IWindow] Get primary monitor done.");
		}
		else
		{
			INVENT_LOG_ERROR("[IWindow] Failed to get primary monitor!" );
			return;
		}

	}

	int IWindow::_glfw_init(bool is_resizable)
	{
		if (GLFW_FALSE == glfwInit())
		{
			INVENT_LOG_ERROR("[IWindow] glfwInit() error");
			return -1;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef USE_OPENGL
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#elif defined(USE_VULKAN)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif // USE_OPENGL
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// 是否可拉伸
		glfwWindowHint(GLFW_RESIZABLE, is_resizable);

		return 0;
	}

	void IWindow::_window_size_change()
	{
		/*Level->_window_size = { (float)Width , (float)Height };
		if (Level->GetController())
		{
			Level->GetController()->UpdateCameraWindowAspect();
		}*/
	}


	void IWindow::_process_input_callback(float delta)
	{
		glfwGetCursorPos(Window, &cursor_xpos, &cursor_ypos);
		for (auto iter = IEngine::InstancePtr()->GetMainScene()->_event_layers.rbegin(); iter != IEngine::InstancePtr()->GetMainScene()->_event_layers.rend(); ++iter)
		{
			if (*iter)
			{
				if ((*iter)->EVENT_CURSOR_POSITION_FRAME(delta, CursorInsideWindow, cursor_xpos, cursor_ypos))
				{
					break;
				}
			}

		}// for

		for (auto iter = IEngine::InstancePtr()->GetMainScene()->_event_layers.rbegin(); iter != IEngine::InstancePtr()->GetMainScene()->_event_layers.rend(); ++iter)
		{
			if (*iter)
			{
				if ((*iter)->EVENT_KEY_FRAME(delta, Window))
				{
					break;
				}
			}

		} // for

	}// _process_input_callback

	void IWindow::_framebuffer_size_callback(int width, int height)
	{
		this->_render_thread->Viewport(width, height);

		this->Width = width;
		this->Height = height;

		this->_window_size_change();

	}

	void IWindow::_register_key_callback(int key, int scancode, int action, int mods)
	{

		for (auto iter = IEngine::InstancePtr()->GetMainScene()->_event_layers.rbegin(); iter != IEngine::InstancePtr()->GetMainScene()->_event_layers.rend(); ++iter)
		{
			if (*iter)
			{
				if ((*iter)->EVENT_KEY(key, action, mods))
				{
					break;
				}
			}
		}// for

	}

	void IWindow::_cursor_position_callback(double xpos, double ypos)
	{

		for (auto iter = IEngine::InstancePtr()->GetMainScene()->_event_layers.rbegin(); iter != IEngine::InstancePtr()->GetMainScene()->_event_layers.rend(); ++iter)
		{
			if (*iter)
			{
				if ((*iter)->EVENT_CURSOR_POSITION(xpos, ypos))
				{
					break;
				}
			}
		}// for
		
	}

	static double xpos = 0.0, ypos = 0.0;
	void IWindow::_mouse_button_callback(int button, int action, int mods)
	{
		glfwGetCursorPos(Window, &xpos, &ypos);

		for (auto iter = IEngine::InstancePtr()->GetMainScene()->_event_layers.rbegin(); iter != IEngine::InstancePtr()->GetMainScene()->_event_layers.rend(); ++iter)
		{
			if (*iter)
			{
				if ((*iter)->EVENT_MOUSE_BUTTON(xpos, ypos, button, action, mods))
				{
					break;
				}
			}
		}// for

	}

	void IWindow::_scroll_callback(double xoffset, double yoffset)
	{
		glfwGetCursorPos(Window, &xpos, &ypos);

		for (auto iter = IEngine::InstancePtr()->GetMainScene()->_event_layers.rbegin(); iter != IEngine::InstancePtr()->GetMainScene()->_event_layers.rend(); ++iter)
		{
			if (*iter)
			{
				if ((*iter)->EVNET_SCROLL(xpos, ypos, xoffset, yoffset))
				{
					break;
				}
			}
		}// for
	
	}

	void IWindow::_cursor_enter_callback(int entered)
	{

		CursorInsideWindow = (entered == GLFW_TRUE);

	}
	
}