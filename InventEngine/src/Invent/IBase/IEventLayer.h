#ifndef _IEVENTLAYER_
#define _IEVENTLAYER_

#include "IBaseEventFunction.h"

#include <vector>

#include <GLFW/glfw3.h>

namespace INVENT
{
	class IEventLayer
	{
	public:
		friend class IWindow;
		friend void register_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

		IEventLayer();
		virtual ~IEventLayer();

		// default true
		void SetAllEventReturn(bool v);
		// default false
		void SetAllPressEventReturn(bool v);
		// default false
		void SetAllReleaseEventReturn(bool v);

		void AddEventObj(IBaseEventFunction* obj);
		void EraseEventObj(IBaseEventFunction* obj);

	protected:
		// if return true, the event will not continue to next layer
		virtual bool EVENT_KEY_SPACE(float delta);
		virtual bool EVENT_KEY_APOSTROPHE(float delta);
		virtual bool EVENT_KEY_COMMA(float delta);
		virtual bool EVENT_KEY_MINUS(float delta);
		virtual bool EVENT_KEY_PERIOD(float delta);
		virtual bool EVENT_KEY_SLASH(float delta);
		virtual bool EVENT_KEY_0(float delta);
		virtual bool EVENT_KEY_1(float delta);
		virtual bool EVENT_KEY_2(float delta);
		virtual bool EVENT_KEY_3(float delta);
		virtual bool EVENT_KEY_4(float delta);
		virtual bool EVENT_KEY_5(float delta);
		virtual bool EVENT_KEY_6(float delta);
		virtual bool EVENT_KEY_7(float delta);
		virtual bool EVENT_KEY_8(float delta);
		virtual bool EVENT_KEY_9(float delta);
		virtual bool EVENT_KEY_SEMICOLON(float delta);
		virtual bool EVENT_KEY_EQUAL(float delta);
		virtual bool EVENT_KEY_A(float delta);
		virtual bool EVENT_KEY_B(float delta);
		virtual bool EVENT_KEY_C(float delta);
		virtual bool EVENT_KEY_D(float delta);
		virtual bool EVENT_KEY_E(float delta);
		virtual bool EVENT_KEY_F(float delta);
		virtual bool EVENT_KEY_G(float delta);
		virtual bool EVENT_KEY_H(float delta);
		virtual bool EVENT_KEY_I(float delta);
		virtual bool EVENT_KEY_J(float delta);
		virtual bool EVENT_KEY_K(float delta);
		virtual bool EVENT_KEY_L(float delta);
		virtual bool EVENT_KEY_M(float delta);
		virtual bool EVENT_KEY_N(float delta);
		virtual bool EVENT_KEY_O(float delta);
		virtual bool EVENT_KEY_P(float delta);
		virtual bool EVENT_KEY_Q(float delta);
		virtual bool EVENT_KEY_R(float delta);
		virtual bool EVENT_KEY_S(float delta);
		virtual bool EVENT_KEY_T(float delta);
		virtual bool EVENT_KEY_U(float delta);
		virtual bool EVENT_KEY_V(float delta);
		virtual bool EVENT_KEY_W(float delta);
		virtual bool EVENT_KEY_X(float delta);
		virtual bool EVENT_KEY_Y(float delta);
		virtual bool EVENT_KEY_Z(float delta);
		virtual bool EVENT_KEY_LEFT_BRACKET(float delta);
		virtual bool EVENT_KEY_BACKSLASH(float delta);
		virtual bool EVENT_KEY_RIGHT_BRACKET(float delta);
		virtual bool EVENT_KEY_GRAVE_ACCENT(float delta);
		virtual bool EVENT_KEY_WORLD_1(float delta);
		virtual bool EVENT_KEY_WORLD_2(float delta);
		virtual bool EVENT_KEY_ESCAPE(float delta);
		virtual bool EVENT_KEY_ENTER(float delta);
		virtual bool EVENT_KEY_TAB(float delta);
		virtual bool EVENT_KEY_BACKSPACE(float delta);
		virtual bool EVENT_KEY_INSERT(float delta);
		virtual bool EVENT_KEY_DELETE(float delta);
		virtual bool EVENT_KEY_RIGHT(float delta);
		virtual bool EVENT_KEY_LEFT(float delta);
		virtual bool EVENT_KEY_DOWN(float delta);
		virtual bool EVENT_KEY_UP(float delta);
		virtual bool EVENT_KEY_PAGE_UP(float delta);
		virtual bool EVENT_KEY_PAGE_DOWN(float delta);
		virtual bool EVENT_KEY_HOME(float delta);
		virtual bool EVENT_KEY_END(float delta);
		virtual bool EVENT_KEY_CAPS_LOCK(float delta);
		virtual bool EVENT_KEY_SCROLL_LOCK(float delta);
		virtual bool EVENT_KEY_NUM_LOCK(float delta);
		virtual bool EVENT_KEY_PRINT_SCREEN(float delta);
		virtual bool EVENT_KEY_PAUSE(float delta);
		virtual bool EVENT_KEY_F1(float delta);
		virtual bool EVENT_KEY_F2(float delta);
		virtual bool EVENT_KEY_F3(float delta);
		virtual bool EVENT_KEY_F4(float delta);
		virtual bool EVENT_KEY_F5(float delta);
		virtual bool EVENT_KEY_F6(float delta);
		virtual bool EVENT_KEY_F7(float delta);
		virtual bool EVENT_KEY_F8(float delta);
		virtual bool EVENT_KEY_F9(float delta);
		virtual bool EVENT_KEY_F10(float delta);
		virtual bool EVENT_KEY_F11(float delta);
		virtual bool EVENT_KEY_F12(float delta);
		virtual bool EVENT_KEY_F13(float delta);
		virtual bool EVENT_KEY_F14(float delta);
		virtual bool EVENT_KEY_F15(float delta);
		virtual bool EVENT_KEY_F16(float delta);
		virtual bool EVENT_KEY_F17(float delta);
		virtual bool EVENT_KEY_F18(float delta);
		virtual bool EVENT_KEY_F19(float delta);
		virtual bool EVENT_KEY_F20(float delta);
		virtual bool EVENT_KEY_F21(float delta);
		virtual bool EVENT_KEY_F22(float delta);
		virtual bool EVENT_KEY_F23(float delta);
		virtual bool EVENT_KEY_F24(float delta);
		virtual bool EVENT_KEY_F25(float delta);
		virtual bool EVENT_KEY_KP_0(float delta);
		virtual bool EVENT_KEY_KP_1(float delta);
		virtual bool EVENT_KEY_KP_2(float delta);
		virtual bool EVENT_KEY_KP_3(float delta);
		virtual bool EVENT_KEY_KP_4(float delta);
		virtual bool EVENT_KEY_KP_5(float delta);
		virtual bool EVENT_KEY_KP_6(float delta);
		virtual bool EVENT_KEY_KP_7(float delta);
		virtual bool EVENT_KEY_KP_8(float delta);
		virtual bool EVENT_KEY_KP_9(float delta);
		virtual bool EVENT_KEY_KP_DECIMAL(float delta);
		virtual bool EVENT_KEY_KP_DIVIDE(float delta);
		virtual bool EVENT_KEY_KP_MULTIPLY(float delta);
		virtual bool EVENT_KEY_KP_SUBTRACT(float delta);
		virtual bool EVENT_KEY_KP_ADD(float delta);
		virtual bool EVENT_KEY_KP_ENTER(float delta);
		virtual bool EVENT_KEY_KP_EQUAL(float delta);
		virtual bool EVENT_KEY_LEFT_SHIFT(float delta);
		virtual bool EVENT_KEY_LEFT_CONTROL(float delta);
		virtual bool EVENT_KEY_LEFT_ALT(float delta);
		virtual bool EVENT_KEY_LEFT_SUPER(float delta);
		virtual bool EVENT_KEY_RIGHT_SHIFT(float delta);
		virtual bool EVENT_KEY_RIGHT_CONTROL(float delta);
		virtual bool EVENT_KEY_RIGHT_ALT(float delta);
		virtual bool EVENT_KEY_RIGHT_SUPER(float delta);
		virtual bool EVENT_KEY_MENU(float delta);

		virtual bool EVENT_CURSOR_POSITION_FRAME(float delta, bool cursor_inside_window, double xpos, double ypos);

		virtual bool PRESS_EVENT_KEY_SPACE();
		virtual bool PRESS_EVENT_KEY_APOSTROPHE();
		virtual bool PRESS_EVENT_KEY_COMMA();
		virtual bool PRESS_EVENT_KEY_MINUS();
		virtual bool PRESS_EVENT_KEY_PERIOD();
		virtual bool PRESS_EVENT_KEY_SLASH();
		virtual bool PRESS_EVENT_KEY_0();
		virtual bool PRESS_EVENT_KEY_1();
		virtual bool PRESS_EVENT_KEY_2();
		virtual bool PRESS_EVENT_KEY_3();
		virtual bool PRESS_EVENT_KEY_4();
		virtual bool PRESS_EVENT_KEY_5();
		virtual bool PRESS_EVENT_KEY_6();
		virtual bool PRESS_EVENT_KEY_7();
		virtual bool PRESS_EVENT_KEY_8();
		virtual bool PRESS_EVENT_KEY_9();
		virtual bool PRESS_EVENT_KEY_SEMICOLON();
		virtual bool PRESS_EVENT_KEY_EQUAL();
		virtual bool PRESS_EVENT_KEY_A();
		virtual bool PRESS_EVENT_KEY_B();
		virtual bool PRESS_EVENT_KEY_C();
		virtual bool PRESS_EVENT_KEY_D();
		virtual bool PRESS_EVENT_KEY_E();
		virtual bool PRESS_EVENT_KEY_F();
		virtual bool PRESS_EVENT_KEY_G();
		virtual bool PRESS_EVENT_KEY_H();
		virtual bool PRESS_EVENT_KEY_I();
		virtual bool PRESS_EVENT_KEY_J();
		virtual bool PRESS_EVENT_KEY_K();
		virtual bool PRESS_EVENT_KEY_L();
		virtual bool PRESS_EVENT_KEY_M();
		virtual bool PRESS_EVENT_KEY_N();
		virtual bool PRESS_EVENT_KEY_O();
		virtual bool PRESS_EVENT_KEY_P();
		virtual bool PRESS_EVENT_KEY_Q();
		virtual bool PRESS_EVENT_KEY_R();
		virtual bool PRESS_EVENT_KEY_S();
		virtual bool PRESS_EVENT_KEY_T();
		virtual bool PRESS_EVENT_KEY_U();
		virtual bool PRESS_EVENT_KEY_V();
		virtual bool PRESS_EVENT_KEY_W();
		virtual bool PRESS_EVENT_KEY_X();
		virtual bool PRESS_EVENT_KEY_Y();
		virtual bool PRESS_EVENT_KEY_Z();
		virtual bool PRESS_EVENT_KEY_LEFT_BRACKET();
		virtual bool PRESS_EVENT_KEY_BACKSLASH();
		virtual bool PRESS_EVENT_KEY_RIGHT_BRACKET();
		virtual bool PRESS_EVENT_KEY_GRAVE_ACCENT();
		virtual bool PRESS_EVENT_KEY_WORLD_1();
		virtual bool PRESS_EVENT_KEY_WORLD_2();
		virtual bool PRESS_EVENT_KEY_ESCAPE();
		virtual bool PRESS_EVENT_KEY_ENTER();
		virtual bool PRESS_EVENT_KEY_TAB();
		virtual bool PRESS_EVENT_KEY_BACKSPACE();
		virtual bool PRESS_EVENT_KEY_INSERT();
		virtual bool PRESS_EVENT_KEY_DELETE();
		virtual bool PRESS_EVENT_KEY_RIGHT();
		virtual bool PRESS_EVENT_KEY_LEFT();
		virtual bool PRESS_EVENT_KEY_DOWN();
		virtual bool PRESS_EVENT_KEY_UP();
		virtual bool PRESS_EVENT_KEY_PAGE_UP();
		virtual bool PRESS_EVENT_KEY_PAGE_DOWN();
		virtual bool PRESS_EVENT_KEY_HOME();
		virtual bool PRESS_EVENT_KEY_END();
		virtual bool PRESS_EVENT_KEY_CAPS_LOCK();
		virtual bool PRESS_EVENT_KEY_SCROLL_LOCK();
		virtual bool PRESS_EVENT_KEY_NUM_LOCK();
		virtual bool PRESS_EVENT_KEY_PRINT_SCREEN();
		virtual bool PRESS_EVENT_KEY_PAUSE();
		virtual bool PRESS_EVENT_KEY_F1();
		virtual bool PRESS_EVENT_KEY_F2();
		virtual bool PRESS_EVENT_KEY_F3();
		virtual bool PRESS_EVENT_KEY_F4();
		virtual bool PRESS_EVENT_KEY_F5();
		virtual bool PRESS_EVENT_KEY_F6();
		virtual bool PRESS_EVENT_KEY_F7();
		virtual bool PRESS_EVENT_KEY_F8();
		virtual bool PRESS_EVENT_KEY_F9();
		virtual bool PRESS_EVENT_KEY_F10();
		virtual bool PRESS_EVENT_KEY_F11();
		virtual bool PRESS_EVENT_KEY_F12();
		virtual bool PRESS_EVENT_KEY_F13();
		virtual bool PRESS_EVENT_KEY_F14();
		virtual bool PRESS_EVENT_KEY_F15();
		virtual bool PRESS_EVENT_KEY_F16();
		virtual bool PRESS_EVENT_KEY_F17();
		virtual bool PRESS_EVENT_KEY_F18();
		virtual bool PRESS_EVENT_KEY_F19();
		virtual bool PRESS_EVENT_KEY_F20();
		virtual bool PRESS_EVENT_KEY_F21();
		virtual bool PRESS_EVENT_KEY_F22();
		virtual bool PRESS_EVENT_KEY_F23();
		virtual bool PRESS_EVENT_KEY_F24();
		virtual bool PRESS_EVENT_KEY_F25();
		virtual bool PRESS_EVENT_KEY_KP_0();
		virtual bool PRESS_EVENT_KEY_KP_1();
		virtual bool PRESS_EVENT_KEY_KP_2();
		virtual bool PRESS_EVENT_KEY_KP_3();
		virtual bool PRESS_EVENT_KEY_KP_4();
		virtual bool PRESS_EVENT_KEY_KP_5();
		virtual bool PRESS_EVENT_KEY_KP_6();
		virtual bool PRESS_EVENT_KEY_KP_7();
		virtual bool PRESS_EVENT_KEY_KP_8();
		virtual bool PRESS_EVENT_KEY_KP_9();
		virtual bool PRESS_EVENT_KEY_KP_DECIMAL();
		virtual bool PRESS_EVENT_KEY_KP_DIVIDE();
		virtual bool PRESS_EVENT_KEY_KP_MULTIPLY();
		virtual bool PRESS_EVENT_KEY_KP_SUBTRACT();
		virtual bool PRESS_EVENT_KEY_KP_ADD();
		virtual bool PRESS_EVENT_KEY_KP_ENTER();
		virtual bool PRESS_EVENT_KEY_KP_EQUAL();
		virtual bool PRESS_EVENT_KEY_LEFT_SHIFT();
		virtual bool PRESS_EVENT_KEY_LEFT_CONTROL();
		virtual bool PRESS_EVENT_KEY_LEFT_ALT();
		virtual bool PRESS_EVENT_KEY_LEFT_SUPER();
		virtual bool PRESS_EVENT_KEY_RIGHT_SHIFT();
		virtual bool PRESS_EVENT_KEY_RIGHT_CONTROL();
		virtual bool PRESS_EVENT_KEY_RIGHT_ALT();
		virtual bool PRESS_EVENT_KEY_RIGHT_SUPER();
		virtual bool PRESS_EVENT_KEY_MENU();

		virtual bool RELEASE_EVENT_KEY_SPACE();
		virtual bool RELEASE_EVENT_KEY_APOSTROPHE();
		virtual bool RELEASE_EVENT_KEY_COMMA();
		virtual bool RELEASE_EVENT_KEY_MINUS();
		virtual bool RELEASE_EVENT_KEY_PERIOD();
		virtual bool RELEASE_EVENT_KEY_SLASH();
		virtual bool RELEASE_EVENT_KEY_0();
		virtual bool RELEASE_EVENT_KEY_1();
		virtual bool RELEASE_EVENT_KEY_2();
		virtual bool RELEASE_EVENT_KEY_3();
		virtual bool RELEASE_EVENT_KEY_4();
		virtual bool RELEASE_EVENT_KEY_5();
		virtual bool RELEASE_EVENT_KEY_6();
		virtual bool RELEASE_EVENT_KEY_7();
		virtual bool RELEASE_EVENT_KEY_8();
		virtual bool RELEASE_EVENT_KEY_9();
		virtual bool RELEASE_EVENT_KEY_SEMICOLON();
		virtual bool RELEASE_EVENT_KEY_EQUAL();
		virtual bool RELEASE_EVENT_KEY_A();
		virtual bool RELEASE_EVENT_KEY_B();
		virtual bool RELEASE_EVENT_KEY_C();
		virtual bool RELEASE_EVENT_KEY_D();
		virtual bool RELEASE_EVENT_KEY_E();
		virtual bool RELEASE_EVENT_KEY_F();
		virtual bool RELEASE_EVENT_KEY_G();
		virtual bool RELEASE_EVENT_KEY_H();
		virtual bool RELEASE_EVENT_KEY_I();
		virtual bool RELEASE_EVENT_KEY_J();
		virtual bool RELEASE_EVENT_KEY_K();
		virtual bool RELEASE_EVENT_KEY_L();
		virtual bool RELEASE_EVENT_KEY_M();
		virtual bool RELEASE_EVENT_KEY_N();
		virtual bool RELEASE_EVENT_KEY_O();
		virtual bool RELEASE_EVENT_KEY_P();
		virtual bool RELEASE_EVENT_KEY_Q();
		virtual bool RELEASE_EVENT_KEY_R();
		virtual bool RELEASE_EVENT_KEY_S();
		virtual bool RELEASE_EVENT_KEY_T();
		virtual bool RELEASE_EVENT_KEY_U();
		virtual bool RELEASE_EVENT_KEY_V();
		virtual bool RELEASE_EVENT_KEY_W();
		virtual bool RELEASE_EVENT_KEY_X();
		virtual bool RELEASE_EVENT_KEY_Y();
		virtual bool RELEASE_EVENT_KEY_Z();
		virtual bool RELEASE_EVENT_KEY_LEFT_BRACKET();
		virtual bool RELEASE_EVENT_KEY_BACKSLASH();
		virtual bool RELEASE_EVENT_KEY_RIGHT_BRACKET();
		virtual bool RELEASE_EVENT_KEY_GRAVE_ACCENT();
		virtual bool RELEASE_EVENT_KEY_WORLD_1();
		virtual bool RELEASE_EVENT_KEY_WORLD_2();
		virtual bool RELEASE_EVENT_KEY_ESCAPE();
		virtual bool RELEASE_EVENT_KEY_ENTER();
		virtual bool RELEASE_EVENT_KEY_TAB();
		virtual bool RELEASE_EVENT_KEY_BACKSPACE();
		virtual bool RELEASE_EVENT_KEY_INSERT();
		virtual bool RELEASE_EVENT_KEY_DELETE();
		virtual bool RELEASE_EVENT_KEY_RIGHT();
		virtual bool RELEASE_EVENT_KEY_LEFT();
		virtual bool RELEASE_EVENT_KEY_DOWN();
		virtual bool RELEASE_EVENT_KEY_UP();
		virtual bool RELEASE_EVENT_KEY_PAGE_UP();
		virtual bool RELEASE_EVENT_KEY_PAGE_DOWN();
		virtual bool RELEASE_EVENT_KEY_HOME();
		virtual bool RELEASE_EVENT_KEY_END();
		virtual bool RELEASE_EVENT_KEY_CAPS_LOCK();
		virtual bool RELEASE_EVENT_KEY_SCROLL_LOCK();
		virtual bool RELEASE_EVENT_KEY_NUM_LOCK();
		virtual bool RELEASE_EVENT_KEY_PRINT_SCREEN();
		virtual bool RELEASE_EVENT_KEY_PAUSE();
		virtual bool RELEASE_EVENT_KEY_F1();
		virtual bool RELEASE_EVENT_KEY_F2();
		virtual bool RELEASE_EVENT_KEY_F3();
		virtual bool RELEASE_EVENT_KEY_F4();
		virtual bool RELEASE_EVENT_KEY_F5();
		virtual bool RELEASE_EVENT_KEY_F6();
		virtual bool RELEASE_EVENT_KEY_F7();
		virtual bool RELEASE_EVENT_KEY_F8();
		virtual bool RELEASE_EVENT_KEY_F9();
		virtual bool RELEASE_EVENT_KEY_F10();
		virtual bool RELEASE_EVENT_KEY_F11();
		virtual bool RELEASE_EVENT_KEY_F12();
		virtual bool RELEASE_EVENT_KEY_F13();
		virtual bool RELEASE_EVENT_KEY_F14();
		virtual bool RELEASE_EVENT_KEY_F15();
		virtual bool RELEASE_EVENT_KEY_F16();
		virtual bool RELEASE_EVENT_KEY_F17();
		virtual bool RELEASE_EVENT_KEY_F18();
		virtual bool RELEASE_EVENT_KEY_F19();
		virtual bool RELEASE_EVENT_KEY_F20();
		virtual bool RELEASE_EVENT_KEY_F21();
		virtual bool RELEASE_EVENT_KEY_F22();
		virtual bool RELEASE_EVENT_KEY_F23();
		virtual bool RELEASE_EVENT_KEY_F24();
		virtual bool RELEASE_EVENT_KEY_F25();
		virtual bool RELEASE_EVENT_KEY_KP_0();
		virtual bool RELEASE_EVENT_KEY_KP_1();
		virtual bool RELEASE_EVENT_KEY_KP_2();
		virtual bool RELEASE_EVENT_KEY_KP_3();
		virtual bool RELEASE_EVENT_KEY_KP_4();
		virtual bool RELEASE_EVENT_KEY_KP_5();
		virtual bool RELEASE_EVENT_KEY_KP_6();
		virtual bool RELEASE_EVENT_KEY_KP_7();
		virtual bool RELEASE_EVENT_KEY_KP_8();
		virtual bool RELEASE_EVENT_KEY_KP_9();
		virtual bool RELEASE_EVENT_KEY_KP_DECIMAL();
		virtual bool RELEASE_EVENT_KEY_KP_DIVIDE();
		virtual bool RELEASE_EVENT_KEY_KP_MULTIPLY();
		virtual bool RELEASE_EVENT_KEY_KP_SUBTRACT();
		virtual bool RELEASE_EVENT_KEY_KP_ADD();
		virtual bool RELEASE_EVENT_KEY_KP_ENTER();
		virtual bool RELEASE_EVENT_KEY_KP_EQUAL();
		virtual bool RELEASE_EVENT_KEY_LEFT_SHIFT();
		virtual bool RELEASE_EVENT_KEY_LEFT_CONTROL();
		virtual bool RELEASE_EVENT_KEY_LEFT_ALT();
		virtual bool RELEASE_EVENT_KEY_LEFT_SUPER();
		virtual bool RELEASE_EVENT_KEY_RIGHT_SHIFT();
		virtual bool RELEASE_EVENT_KEY_RIGHT_CONTROL();
		virtual bool RELEASE_EVENT_KEY_RIGHT_ALT();
		virtual bool RELEASE_EVENT_KEY_RIGHT_SUPER();
		virtual bool RELEASE_EVENT_KEY_MENU();

		virtual bool EVENT_CURSOR_POSITION(double xpos, double ypos);

		virtual bool PRESS_EVENT_MOUSE_BUTTON_LEFT(double xpos, double ypos);
		virtual bool RELEASE_EVENT_MOUSE_BUTTON_LEFT(double xpos, double ypos);

		virtual bool PRESS_EVENT_MOUSE_BUTTON_RIGHT(double xpos, double ypos);
		virtual bool RELEASE_EVENT_MOUSE_BUTTON_RIGHT(double xpos, double ypos);

		virtual bool PRESS_EVENT_MOUSE_BUTTON_MIDDLE(double xpos, double ypos);
		virtual bool RELEASE_EVENT_MOUSE_BUTTON_MIDDLE(double xpos, double ypos);

		virtual bool PRESS_EVENT_MOUSE_BUTTON_4(double xpos, double ypos);
		virtual bool RELEASE_EVENT_MOUSE_BUTTON_4(double xpos, double ypos);

		virtual bool PRESS_EVENT_MOUSE_BUTTON_5(double xpos, double ypos);
		virtual bool RELEASE_EVENT_MOUSE_BUTTON_5(double xpos, double ypos);

		virtual bool PRESS_EVENT_MOUSE_BUTTON_6(double xpos, double ypos);
		virtual bool RELEASE_EVENT_MOUSE_BUTTON_6(double xpos, double ypos);

		virtual bool PRESS_EVENT_MOUSE_BUTTON_7(double xpos, double ypos);
		virtual bool RELEASE_EVENT_MOUSE_BUTTON_7(double xpos, double ypos);

		virtual bool PRESS_EVENT_MOUSE_BUTTON_8(double xpos, double ypos);
		virtual bool RELEASE_EVENT_MOUSE_BUTTON_8(double xpos, double ypos);

		virtual bool EVNET_SCROLL(double cursor_xpos, double cursor_ypos, double xoffset, double yoffset);

	protected:
		std::vector<IBaseEventFunction*> Objs;

		bool AllEventReturn = true;
		bool AllPressEventReturn = false;
		bool AllReleaseEventReturn = false;

		bool CursorPositionEventReturn = true;
		bool MouseButtonEventReturn = true;
	};


}

#endif //!_IEVENTLAYER_
