


#include "Invent/Invent.h"

class MyWindow : public INVENT::IWindow
{
public:
	MyWindow()
	{

	}

	virtual ~MyWindow()
	{

	}

};


int main()
{
	MyWindow window;
	window.Start();

	return 0;
}