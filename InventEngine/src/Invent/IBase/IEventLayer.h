#ifndef _IEVENTLAYER_
#define _IEVENTLAYER_

#include <vector>
#include <queue>
#include <tuple>
#include <mutex>

//#include <GLFW/glfw3.h>

struct GLFWwindow;

namespace INVENT
{
	class IEventLayer
	{
		friend class IWindow;
	public:

		typedef bool IsEventDone;

		IEventLayer();
		virtual ~IEventLayer();

		typedef size_t EventKeyFrameFunctionID;
		EventKeyFrameFunctionID RegisterEventKeyFrameFunction(std::function<void(float)>&& callback, int key);
		void CancellationEventKeyFrameFunction(EventKeyFrameFunctionID id);

		typedef size_t CursorPositionFunctionID;
		CursorPositionFunctionID RegisterCursorPositionFunction(std::function<void(float, bool, double, double)>&& callback);
		void CancellationCursorPositionFunction(CursorPositionFunctionID id);

		typedef size_t EventKeyFunctionID;
		/// <summary>
		/// 注册一个按键事件回调，当指定的按键、动作和修饰键匹配时调用该回调，并返回该回调的标识符。
		/// 关于修饰键：只要有绑定的修饰键如只绑定了 Ctrl 键但同时按下了 Ctrl 与 Shift 键依旧可触发并会同时触发 符合要求的其他按键组合
		/// 更改见 源文件
		/// </summary>
		/// <param name="callback">要注册的回调函数，签名为 void()。通过右值引用传入并会被移动到内部存储，在匹配的按键事件发生时调用。</param>
		/// <param name="key">用于匹配的按键码或按键标识。</param>
		/// <param name="action">按键动作的标识（例如按下、释放等），用于决定何时触发回调。</param>
		/// <param name="mods">修饰键掩码（例如 Shift、Ctrl、Alt 等）为 0 时将无视是否按下修饰键</param>
		/// <returns>返回一个 EventKeyFunctionID，表示已注册回调的唯一标识符，可用于之后取消注册或管理该回调。</returns>
		EventKeyFunctionID RegisterEventKeyFunction(std::function<void()>&& callback, int key, int action, int mods = 0);
		void CancellationEventKeyFunction(EventKeyFunctionID id);

		typedef size_t EventMouseButtonFunctionID;
		/// <summary>
		/// 注册一个鼠标按键事件的回调函数，当指定的鼠标按键发生指定动作时调用。
		/// </summary>
		/// <param name="callback">右值引用的回调函数，签名为 void(double x, double y)，表示事件发生时的坐标（光标的 x 和 y）。</param>
		/// <param name="button">要监听的鼠标按键。</param>
		/// <param name="action">按键动作类型（例如按下或释放）</param>
		/// <param name="mods">可选的修饰键掩码（例如 Shift、Ctrl、Alt）详情见 RegisterEventKeyFunction</param>
		/// <returns>返回一个 EventMouseButtonFunctionID，用于标识该注册的回调，可用于后续取消注册或管理该回调。</returns>
		EventMouseButtonFunctionID RegisterEventMouseButtonFunction(std::function<void(double, double)>&& callback, int button, int action, int mods = 0);
		void CancellationEventMouseButtonFunction(EventMouseButtonFunctionID id);

		typedef size_t EventScrollFunctionID;
		/// <summary>
		/// 注册一个滚动事件回调函数并返回其标识符。
		/// </summary>
		/// <param name="callback">回调函数 void(double, double, double, double)。表示滚动相关的数值（位置与增量）</param>
		/// <returns>返回一个 EventScrollFunctionID，表示已注册的回调。该标识符可用于后续的管理或注销操作。</returns>
		EventScrollFunctionID RegisterEventScrollFunction(std::function<void(double, double, double, double)>&& callback);
		void CancellationEventScrollFunction(EventScrollFunctionID id);

		typedef size_t EventCursorPositionFunctionID;
		EventCursorPositionFunctionID RegisterEventCursorPositionFunction(std::function<void(double, double)>&& callback);
		void CancellationEventCursorPositionFunction(EventCursorPositionFunctionID id);

	protected:

		/////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual IsEventDone EVENT_CURSOR_POSITION_FRAME(float delta, bool cursor_inside_window, double xpos, double ypos);
		virtual IsEventDone EVENT_KEY_FRAME(float delta, GLFWwindow* glfw_window);
		virtual IsEventDone EVENT_KEY(int key, int action, int mods);
		virtual IsEventDone EVENT_MOUSE_BUTTON(double xpos, double ypos, int button, int action, int mods);
		virtual IsEventDone EVNET_SCROLL(double cursor_xpos, double cursor_ypos, double xoffset, double yoffset);
		virtual IsEventDone EVENT_CURSOR_POSITION(double xpos, double ypos);

		///////////////////////////////////////////////////////////////////////////////////////////////////////

	protected:

		/// <summary>
		/// default : true
		/// 设置此变量标记事件是否已处理完毕，若为 true 则表示已处理完毕，将不继续向下层传递
		/// 例如：UI层处理鼠标位置事件且不想继续将鼠标位置事件（每帧）向场景层传递
		/// 若想继续传递则设置为 false
		/// </summary>
		bool EventCursorPositionFrameDone = true;
		/// <summary>
		/// default : true
		/// 设置此变量标记事件是否已处理完毕，若为 true 则表示已处理完毕，将不继续向下层传递
		/// 详细介绍见 EventCursorPositionFrameDone
		/// </summary>
		bool EventKeyFrameDone = true;
		/// <summary>
		/// default : false
		/// 设置此变量标记事件是否已处理完毕，若为 true 则表示已处理完毕，将不继续向下层传递
		/// 详细介绍见 EventCursorPositionFrameDone
		/// </summary>
		bool EventKeyDone = false;
		/// <summary>
		/// default : false
		/// 设置此变量标记事件是否已处理完毕，若为 true 则表示已处理完毕，将不继续向下层传递
		/// 详细介绍见 EventCursorPositionFrameDone
		/// </summary>
		bool EventMouseButtonDone = false;
		/// <summary>
		/// default : false
		/// 设置此变量标记事件是否已处理完毕，若为 true 则表示已处理完毕，将不继续向下层传递
		/// 详细介绍见 EventCursorPositionFrameDone
		/// </summary>
		bool EventScrollDone = false;
		/// <summary>
		/// default : false
		/// 设置此变量标记事件是否已处理完毕，若为 true 则表示已处理完毕，将不继续向下层传递
		/// 详细介绍见 EventCursorPositionFrameDone
		/// </summary>
		bool EventCursorPositionDone = false;


	private:
		std::mutex _mutex;
		// EventKeyFrame
		std::queue<size_t> NormalInputFunctionIDs;
		std::vector<std::pair<std::function<void(float)>, int>> _normal_input_callbacks;

		std::queue<size_t> CursorPositionFunctionIDs;
		std::vector<std::function<void(float, bool, double, double)>> _cursor_position_input_callbacks;

		std::queue<size_t> EventKeyFunctionIDs;
		std::vector<std::tuple<std::function<void()>, int, int, int>> _event_key_callbacks;

		std::queue<size_t> EventMouseButtonFunctionIDs;
		std::vector<std::tuple<std::function<void(double, double)>, int, int, int>> _event_mouse_button_callbacks;

		std::queue<size_t> EventScrollFunctionIDs;
		std::vector<std::function<void(double, double, double, double)>> _event_scroll_callbacks;

		std::queue<size_t> EventCursorPositionFunctionIDs;
		std::vector<std::function<void(double, double)>> _event_cursor_position_callbacks;
	};


}

#include "IEventDefine.h"

#endif //!_IEVENTLAYER_
