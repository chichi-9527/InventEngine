#ifndef _IBASEEVENTFUNCTION_
#define _IBASEEVENTFUNCTION_

namespace INVENT
{
	class IBaseEventFunction
	{
	public:
		friend class IEventLayer;

		IBaseEventFunction() = default;
		virtual ~IBaseEventFunction() = default;

	protected:

		// 已废弃 现在通过注册回调函数 若使用这些函数，在 IWindow 中更改调用 _process_input
		virtual void EVENT_KEY_SPACE(float delta);
		virtual void EVENT_KEY_APOSTROPHE(float delta);
		virtual void EVENT_KEY_COMMA(float delta);
		virtual void EVENT_KEY_MINUS(float delta);
		virtual void EVENT_KEY_PERIOD(float delta);
		virtual void EVENT_KEY_SLASH(float delta);
		virtual void EVENT_KEY_0(float delta);
		virtual void EVENT_KEY_1(float delta);
		virtual void EVENT_KEY_2(float delta);
		virtual void EVENT_KEY_3(float delta);
		virtual void EVENT_KEY_4(float delta);
		virtual void EVENT_KEY_5(float delta);
		virtual void EVENT_KEY_6(float delta);
		virtual void EVENT_KEY_7(float delta);
		virtual void EVENT_KEY_8(float delta);
		virtual void EVENT_KEY_9(float delta);
		virtual void EVENT_KEY_SEMICOLON(float delta);
		virtual void EVENT_KEY_EQUAL(float delta);
		virtual void EVENT_KEY_A(float delta);
		virtual void EVENT_KEY_B(float delta);
		virtual void EVENT_KEY_C(float delta);
		virtual void EVENT_KEY_D(float delta);
		virtual void EVENT_KEY_E(float delta);
		virtual void EVENT_KEY_F(float delta);
		virtual void EVENT_KEY_G(float delta);
		virtual void EVENT_KEY_H(float delta);
		virtual void EVENT_KEY_I(float delta);
		virtual void EVENT_KEY_J(float delta);
		virtual void EVENT_KEY_K(float delta);
		virtual void EVENT_KEY_L(float delta);
		virtual void EVENT_KEY_M(float delta);
		virtual void EVENT_KEY_N(float delta);
		virtual void EVENT_KEY_O(float delta);
		virtual void EVENT_KEY_P(float delta);
		virtual void EVENT_KEY_Q(float delta);
		virtual void EVENT_KEY_R(float delta);
		virtual void EVENT_KEY_S(float delta);
		virtual void EVENT_KEY_T(float delta);
		virtual void EVENT_KEY_U(float delta);
		virtual void EVENT_KEY_V(float delta);
		virtual void EVENT_KEY_W(float delta);
		virtual void EVENT_KEY_X(float delta);
		virtual void EVENT_KEY_Y(float delta);
		virtual void EVENT_KEY_Z(float delta);
		virtual void EVENT_KEY_LEFT_BRACKET(float delta);
		virtual void EVENT_KEY_BACKSLASH(float delta);
		virtual void EVENT_KEY_RIGHT_BRACKET(float delta);
		virtual void EVENT_KEY_GRAVE_ACCENT(float delta);
		virtual void EVENT_KEY_WORLD_1(float delta);
		virtual void EVENT_KEY_WORLD_2(float delta);
		virtual void EVENT_KEY_ESCAPE(float delta);
		virtual void EVENT_KEY_ENTER(float delta);
		virtual void EVENT_KEY_TAB(float delta);
		virtual void EVENT_KEY_BACKSPACE(float delta);
		virtual void EVENT_KEY_INSERT(float delta);
		virtual void EVENT_KEY_DELETE(float delta);
		virtual void EVENT_KEY_RIGHT(float delta);
		virtual void EVENT_KEY_LEFT(float delta);
		virtual void EVENT_KEY_DOWN(float delta);
		virtual void EVENT_KEY_UP(float delta);
		virtual void EVENT_KEY_PAGE_UP(float delta);
		virtual void EVENT_KEY_PAGE_DOWN(float delta);
		virtual void EVENT_KEY_HOME(float delta);
		virtual void EVENT_KEY_END(float delta);
		virtual void EVENT_KEY_CAPS_LOCK(float delta);
		virtual void EVENT_KEY_SCROLL_LOCK(float delta);
		virtual void EVENT_KEY_NUM_LOCK(float delta);
		virtual void EVENT_KEY_PRINT_SCREEN(float delta);
		virtual void EVENT_KEY_PAUSE(float delta);
		virtual void EVENT_KEY_F1(float delta);
		virtual void EVENT_KEY_F2(float delta);
		virtual void EVENT_KEY_F3(float delta);
		virtual void EVENT_KEY_F4(float delta);
		virtual void EVENT_KEY_F5(float delta);
		virtual void EVENT_KEY_F6(float delta);
		virtual void EVENT_KEY_F7(float delta);
		virtual void EVENT_KEY_F8(float delta);
		virtual void EVENT_KEY_F9(float delta);
		virtual void EVENT_KEY_F10(float delta);
		virtual void EVENT_KEY_F11(float delta);
		virtual void EVENT_KEY_F12(float delta);
		virtual void EVENT_KEY_F13(float delta);
		virtual void EVENT_KEY_F14(float delta);
		virtual void EVENT_KEY_F15(float delta);
		virtual void EVENT_KEY_F16(float delta);
		virtual void EVENT_KEY_F17(float delta);
		virtual void EVENT_KEY_F18(float delta);
		virtual void EVENT_KEY_F19(float delta);
		virtual void EVENT_KEY_F20(float delta);
		virtual void EVENT_KEY_F21(float delta);
		virtual void EVENT_KEY_F22(float delta);
		virtual void EVENT_KEY_F23(float delta);
		virtual void EVENT_KEY_F24(float delta);
		virtual void EVENT_KEY_F25(float delta);
		virtual void EVENT_KEY_KP_0(float delta);
		virtual void EVENT_KEY_KP_1(float delta);
		virtual void EVENT_KEY_KP_2(float delta);
		virtual void EVENT_KEY_KP_3(float delta);
		virtual void EVENT_KEY_KP_4(float delta);
		virtual void EVENT_KEY_KP_5(float delta);
		virtual void EVENT_KEY_KP_6(float delta);
		virtual void EVENT_KEY_KP_7(float delta);
		virtual void EVENT_KEY_KP_8(float delta);
		virtual void EVENT_KEY_KP_9(float delta);
		virtual void EVENT_KEY_KP_DECIMAL(float delta);
		virtual void EVENT_KEY_KP_DIVIDE(float delta);
		virtual void EVENT_KEY_KP_MULTIPLY(float delta);
		virtual void EVENT_KEY_KP_SUBTRACT(float delta);
		virtual void EVENT_KEY_KP_ADD(float delta);
		virtual void EVENT_KEY_KP_ENTER(float delta);
		virtual void EVENT_KEY_KP_EQUAL(float delta);
		virtual void EVENT_KEY_LEFT_SHIFT(float delta);
		virtual void EVENT_KEY_LEFT_CONTROL(float delta);
		virtual void EVENT_KEY_LEFT_ALT(float delta);
		virtual void EVENT_KEY_LEFT_SUPER(float delta);
		virtual void EVENT_KEY_RIGHT_SHIFT(float delta);
		virtual void EVENT_KEY_RIGHT_CONTROL(float delta);
		virtual void EVENT_KEY_RIGHT_ALT(float delta);
		virtual void EVENT_KEY_RIGHT_SUPER(float delta);
		virtual void EVENT_KEY_MENU(float delta);

		///////////////////////////////////////////////////////////

		virtual void EVENT_CURSOR_POSITION_FRAME(float delta, bool cursor_inside_window, double xpos, double ypos);

		virtual void PRESS_EVENT(int key);
		
		virtual void RELEASE_EVENT(int key);

		virtual void EVENT_CURSOR_POSITION(double xpos, double ypos);

		virtual void PRESS_EVENT_MOUSE_BUTTON_LEFT(double xpos, double ypos);
		virtual void RELEASE_EVENT_MOUSE_BUTTON_LEFT(double xpos, double ypos);

		virtual void  PRESS_EVENT_MOUSE_BUTTON_RIGHT(double xpos, double ypos);
		virtual void  RELEASE_EVENT_MOUSE_BUTTON_RIGHT(double xpos, double ypos);
		virtual void  PRESS_EVENT_MOUSE_BUTTON_MIDDLE(double xpos, double ypos);
		virtual void  RELEASE_EVENT_MOUSE_BUTTON_MIDDLE(double xpos, double ypos);
		virtual void  PRESS_EVENT_MOUSE_BUTTON_4(double xpos, double ypos);
		virtual void  RELEASE_EVENT_MOUSE_BUTTON_4(double xpos, double ypos);
		virtual void  PRESS_EVENT_MOUSE_BUTTON_5(double xpos, double ypos);
		virtual void  RELEASE_EVENT_MOUSE_BUTTON_5(double xpos, double ypos);
		virtual void  PRESS_EVENT_MOUSE_BUTTON_6(double xpos, double ypos);
		virtual void  RELEASE_EVENT_MOUSE_BUTTON_6(double xpos, double ypos);
		virtual void  PRESS_EVENT_MOUSE_BUTTON_7(double xpos, double ypos);
		virtual void  RELEASE_EVENT_MOUSE_BUTTON_7(double xpos, double ypos);
		virtual void  PRESS_EVENT_MOUSE_BUTTON_8(double xpos, double ypos);
		virtual void  RELEASE_EVENT_MOUSE_BUTTON_8(double xpos, double ypos);

		virtual void EVNET_SCROLL(double cursor_xpos, double cursor_ypos, double xoffset, double yoffset);
	};
}

#include "IEventDefine.h"

#endif // !_IBASEEVENTFUNCTION_
