#include "IEpch.h"
#include "IEventLayer.h"

#define EVENT_FUNC(NAME) bool IEventLayer::NAME(float delta){	\
		for (auto obj : Objs){obj->NAME(delta);}	\
		return AllEventReturn;}

#define PRESS_EVENT_FUNC(NAME) bool IEventLayer::PRESS_##NAME(){	\
		for (auto obj : Objs){obj->PRESS_##NAME();}	\
		return AllPressEventReturn;}

#define RELEASE_EVENT_FUNC(NAME) bool IEventLayer::RELEASE_##NAME(){	\
		for (auto obj : Objs){obj->RELEASE_##NAME();}	\
		return AllReleaseEventReturn;}

namespace INVENT
{
	IEventLayer::IEventLayer()
	{}

	IEventLayer::~IEventLayer()
	{}

	void IEventLayer::SetAllEventReturn(bool v)
	{
		AllEventReturn = v;
	}

	void IEventLayer::SetAllPressEventReturn(bool v)
	{
		AllPressEventReturn = v;
	}

	void IEventLayer::SetAllReleaseEventReturn(bool v)
	{
		AllReleaseEventReturn = v;
	}

	void IEventLayer::AddEventObj(IBaseEventFunction* obj)
	{
		if (obj)
			Objs.push_back(obj);
	}

	void IEventLayer::EraseEventObj(IBaseEventFunction * obj)
	{
		if (obj)
		{
			auto iter = std::find(Objs.begin(), Objs.end(), obj);
			if (iter != Objs.end())
			{
				Objs.erase(iter);
			}
		}
	}

	EVENT_FUNC(EVENT_KEY_SPACE);
	EVENT_FUNC(EVENT_KEY_APOSTROPHE);
	EVENT_FUNC(EVENT_KEY_COMMA);
	EVENT_FUNC(EVENT_KEY_MINUS);
	EVENT_FUNC(EVENT_KEY_PERIOD);
	EVENT_FUNC(EVENT_KEY_SLASH);
	EVENT_FUNC(EVENT_KEY_0);
	EVENT_FUNC(EVENT_KEY_1);
	EVENT_FUNC(EVENT_KEY_2);
	EVENT_FUNC(EVENT_KEY_3);
	EVENT_FUNC(EVENT_KEY_4);
	EVENT_FUNC(EVENT_KEY_5);
	EVENT_FUNC(EVENT_KEY_6);
	EVENT_FUNC(EVENT_KEY_7);
	EVENT_FUNC(EVENT_KEY_8);
	EVENT_FUNC(EVENT_KEY_9);
	EVENT_FUNC(EVENT_KEY_SEMICOLON);
	EVENT_FUNC(EVENT_KEY_EQUAL);
	EVENT_FUNC(EVENT_KEY_A);
	EVENT_FUNC(EVENT_KEY_B);
	EVENT_FUNC(EVENT_KEY_C);
	EVENT_FUNC(EVENT_KEY_D);
	EVENT_FUNC(EVENT_KEY_E);
	EVENT_FUNC(EVENT_KEY_F);
	EVENT_FUNC(EVENT_KEY_G);
	EVENT_FUNC(EVENT_KEY_H);
	EVENT_FUNC(EVENT_KEY_I);
	EVENT_FUNC(EVENT_KEY_J);
	EVENT_FUNC(EVENT_KEY_K);
	EVENT_FUNC(EVENT_KEY_L);
	EVENT_FUNC(EVENT_KEY_M);
	EVENT_FUNC(EVENT_KEY_N);
	EVENT_FUNC(EVENT_KEY_O);
	EVENT_FUNC(EVENT_KEY_P);
	EVENT_FUNC(EVENT_KEY_Q);
	EVENT_FUNC(EVENT_KEY_R);
	EVENT_FUNC(EVENT_KEY_S);
	EVENT_FUNC(EVENT_KEY_T);
	EVENT_FUNC(EVENT_KEY_U);
	EVENT_FUNC(EVENT_KEY_V);
	EVENT_FUNC(EVENT_KEY_W);
	EVENT_FUNC(EVENT_KEY_X);
	EVENT_FUNC(EVENT_KEY_Y);
	EVENT_FUNC(EVENT_KEY_Z);
	EVENT_FUNC(EVENT_KEY_LEFT_BRACKET);
	EVENT_FUNC(EVENT_KEY_BACKSLASH);
	EVENT_FUNC(EVENT_KEY_RIGHT_BRACKET);
	EVENT_FUNC(EVENT_KEY_GRAVE_ACCENT);
	EVENT_FUNC(EVENT_KEY_WORLD_1);
	EVENT_FUNC(EVENT_KEY_WORLD_2);
	EVENT_FUNC(EVENT_KEY_ESCAPE);
	EVENT_FUNC(EVENT_KEY_ENTER);
	EVENT_FUNC(EVENT_KEY_TAB);
	EVENT_FUNC(EVENT_KEY_BACKSPACE);
	EVENT_FUNC(EVENT_KEY_INSERT);
	EVENT_FUNC(EVENT_KEY_DELETE);
	EVENT_FUNC(EVENT_KEY_RIGHT);
	EVENT_FUNC(EVENT_KEY_LEFT);
	EVENT_FUNC(EVENT_KEY_DOWN);
	EVENT_FUNC(EVENT_KEY_UP);
	EVENT_FUNC(EVENT_KEY_PAGE_UP);
	EVENT_FUNC(EVENT_KEY_PAGE_DOWN);
	EVENT_FUNC(EVENT_KEY_HOME);
	EVENT_FUNC(EVENT_KEY_END);
	EVENT_FUNC(EVENT_KEY_CAPS_LOCK);
	EVENT_FUNC(EVENT_KEY_SCROLL_LOCK);
	EVENT_FUNC(EVENT_KEY_NUM_LOCK);
	EVENT_FUNC(EVENT_KEY_PRINT_SCREEN);
	EVENT_FUNC(EVENT_KEY_PAUSE);
	EVENT_FUNC(EVENT_KEY_F1);
	EVENT_FUNC(EVENT_KEY_F2);
	EVENT_FUNC(EVENT_KEY_F3);
	EVENT_FUNC(EVENT_KEY_F4);
	EVENT_FUNC(EVENT_KEY_F5);
	EVENT_FUNC(EVENT_KEY_F6);
	EVENT_FUNC(EVENT_KEY_F7);
	EVENT_FUNC(EVENT_KEY_F8);
	EVENT_FUNC(EVENT_KEY_F9);
	EVENT_FUNC(EVENT_KEY_F10);
	EVENT_FUNC(EVENT_KEY_F11);
	EVENT_FUNC(EVENT_KEY_F12);
	EVENT_FUNC(EVENT_KEY_F13);
	EVENT_FUNC(EVENT_KEY_F14);
	EVENT_FUNC(EVENT_KEY_F15);
	EVENT_FUNC(EVENT_KEY_F16);
	EVENT_FUNC(EVENT_KEY_F17);
	EVENT_FUNC(EVENT_KEY_F18);
	EVENT_FUNC(EVENT_KEY_F19);
	EVENT_FUNC(EVENT_KEY_F20);
	EVENT_FUNC(EVENT_KEY_F21);
	EVENT_FUNC(EVENT_KEY_F22);
	EVENT_FUNC(EVENT_KEY_F23);
	EVENT_FUNC(EVENT_KEY_F24);
	EVENT_FUNC(EVENT_KEY_F25);
	EVENT_FUNC(EVENT_KEY_KP_0);
	EVENT_FUNC(EVENT_KEY_KP_1);
	EVENT_FUNC(EVENT_KEY_KP_2);
	EVENT_FUNC(EVENT_KEY_KP_3);
	EVENT_FUNC(EVENT_KEY_KP_4);
	EVENT_FUNC(EVENT_KEY_KP_5);
	EVENT_FUNC(EVENT_KEY_KP_6);
	EVENT_FUNC(EVENT_KEY_KP_7);
	EVENT_FUNC(EVENT_KEY_KP_8);
	EVENT_FUNC(EVENT_KEY_KP_9);
	EVENT_FUNC(EVENT_KEY_KP_DECIMAL);
	EVENT_FUNC(EVENT_KEY_KP_DIVIDE);
	EVENT_FUNC(EVENT_KEY_KP_MULTIPLY);
	EVENT_FUNC(EVENT_KEY_KP_SUBTRACT);
	EVENT_FUNC(EVENT_KEY_KP_ADD);
	EVENT_FUNC(EVENT_KEY_KP_ENTER);
	EVENT_FUNC(EVENT_KEY_KP_EQUAL);
	EVENT_FUNC(EVENT_KEY_LEFT_SHIFT);
	EVENT_FUNC(EVENT_KEY_LEFT_CONTROL);
	EVENT_FUNC(EVENT_KEY_LEFT_ALT);
	EVENT_FUNC(EVENT_KEY_LEFT_SUPER);
	EVENT_FUNC(EVENT_KEY_RIGHT_SHIFT);
	EVENT_FUNC(EVENT_KEY_RIGHT_CONTROL);
	EVENT_FUNC(EVENT_KEY_RIGHT_ALT);
	EVENT_FUNC(EVENT_KEY_RIGHT_SUPER);
	EVENT_FUNC(EVENT_KEY_MENU);

	bool IEventLayer::PRESS_EVENT(int key)
	{
		for (auto obj : Objs) 
		{ 
			obj->PRESS_EVENT(key); 
		}
		return AllPressEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT(int key)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT(key);
		}
		return AllReleaseEventReturn;
	}

	bool IEventLayer::EVENT_CURSOR_POSITION(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->EVENT_CURSOR_POSITION(xpos, ypos);
		}
		return CursorPositionEventReturn;
	}

	bool IEventLayer::PRESS_EVENT_MOUSE_BUTTON_LEFT(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->PRESS_EVENT_MOUSE_BUTTON_LEFT(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT_MOUSE_BUTTON_LEFT(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT_MOUSE_BUTTON_LEFT(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::PRESS_EVENT_MOUSE_BUTTON_RIGHT(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->PRESS_EVENT_MOUSE_BUTTON_RIGHT(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT_MOUSE_BUTTON_RIGHT(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT_MOUSE_BUTTON_RIGHT(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::PRESS_EVENT_MOUSE_BUTTON_MIDDLE(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->PRESS_EVENT_MOUSE_BUTTON_MIDDLE(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT_MOUSE_BUTTON_MIDDLE(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT_MOUSE_BUTTON_MIDDLE(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::PRESS_EVENT_MOUSE_BUTTON_4(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->PRESS_EVENT_MOUSE_BUTTON_4(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT_MOUSE_BUTTON_4(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT_MOUSE_BUTTON_4(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::PRESS_EVENT_MOUSE_BUTTON_5(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->PRESS_EVENT_MOUSE_BUTTON_5(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT_MOUSE_BUTTON_5(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT_MOUSE_BUTTON_5(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::PRESS_EVENT_MOUSE_BUTTON_6(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->PRESS_EVENT_MOUSE_BUTTON_6(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT_MOUSE_BUTTON_6(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT_MOUSE_BUTTON_6(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::PRESS_EVENT_MOUSE_BUTTON_7(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->PRESS_EVENT_MOUSE_BUTTON_7(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT_MOUSE_BUTTON_7(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT_MOUSE_BUTTON_7(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::PRESS_EVENT_MOUSE_BUTTON_8(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->PRESS_EVENT_MOUSE_BUTTON_8(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::RELEASE_EVENT_MOUSE_BUTTON_8(double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->RELEASE_EVENT_MOUSE_BUTTON_8(xpos, ypos);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::EVNET_SCROLL(double cursor_xpos, double cursor_ypos, double xoffset, double yoffset)
	{
		for (auto obj : Objs)
		{
			obj->EVNET_SCROLL(cursor_xpos, cursor_ypos, xoffset, yoffset);
		}
		return MouseButtonEventReturn;
	}

	bool IEventLayer::EVENT_CURSOR_POSITION_FRAME(float delta, bool cursor_inside_window, double xpos, double ypos)
	{
		for (auto obj : Objs)
		{
			obj->EVENT_CURSOR_POSITION_FRAME(delta, cursor_inside_window, xpos, ypos);
		}
		return AllEventReturn;
	}

}


  