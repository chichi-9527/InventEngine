

#include "Invent/Invent.h"

#include <iostream>

class MyGameInstance : public INVENT::IGameInstance<MyGameInstance>
{
	INVENT_GAMEINSTANCE(MyGameInstance)
public:
	virtual ~MyGameInstance(){}
	MyGameInstance(const MyGameInstance&) = delete;
	MyGameInstance& operator=(const MyGameInstance&) = delete;

	virtual void Begin() override
	{
		
	}

	virtual void Update(float delta) override
	{
		
	}

	virtual void End() override
	{
		
	}

private:
	MyGameInstance() = default;
};

class MyWindow : public INVENT::IWindow
{
public:
	MyWindow()
	{
		this->SetGameInstance(std::static_pointer_cast<INVENT::IBaseGameInstance>(MyGameInstance::GetGameInstancePtr()));
	}

	virtual ~MyWindow()
	{

	}

};

//struct MyStruct
//{
//	MyStruct() = default;
//	MyStruct(const MyStruct&) = default;
//	MyStruct(const bool& a)
//		: is(a){}
//
//	bool is = false;
//};
//
//struct MyStruct1
//{
//	MyStruct1() = default;
//	MyStruct1(const MyStruct1&) = default;
//	MyStruct1(const int& a)
//		: is(a)
//	{}
//
//	int is = 0;
//};


int main()
{
	MyWindow window;
	window.Start();

	/*INVENT::IComponentManagement icm;
	auto h = icm.Create();
	icm.Emplace< MyStruct>(h, false);
	icm.Emplace< MyStruct1>(h, 2);
	icm.Emplace< INVENT::WorldPositionComponent>(h, glm::vec3{ 0.5f });

	auto h1 = icm.Create();
	icm.Emplace< MyStruct>(h1, true);

	INVENT::IComponentManagement icm2;

	auto h2 = icm2.Create();
	icm2.Emplace< MyStruct>(h2, true);
	
	std::cout << icm.Has<MyStruct>(h) << "\n";
	std::cout << icm.Get<INVENT::WorldPositionComponent>(h)->WorldPosition.x << "\n";
	std::cout << icm2.Get<MyStruct>(h2)->is << "\n";

	INVENT::IObjectBase obj_base;
	std::cout <<  obj_base.GetComponent<INVENT::WorldPositionComponent>()->WorldPosition.y << "\n";
	obj_base.GetComponent<INVENT::WorldPositionComponent>()->WorldPosition.y = 0.5f;
	std::cout <<  obj_base.GetComponent<INVENT::WorldPositionComponent>()->WorldPosition.y << "\n";*/

	return 0;
}
