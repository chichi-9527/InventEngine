

#include "Invent/Invent.h"

#include <iostream>

#include <filesystem>


class MyGameInstance : public INVENT::IGameInstance<MyGameInstance>
{
	INVENT_GAMEINSTANCE(MyGameInstance)
public:
	virtual ~MyGameInstance(){}

	virtual void Begin() override
	{
		INVENT::ISquare2dPawn* pawn = new INVENT::ISquare2dPawn;
		std::cout << pawn->GetColor().r << "\n";

		std::cout << std::filesystem::current_path() << "\n";

		auto tex = INVENT::ITexture2DManagement::Instance().CreateTexture("./Assets/Textures/test.png");
	}

	virtual void Update(float delta) override
	{
		
	}

	virtual void End() override
	{
		
	}
};

class MyActor : public INVENT::ISquare2dPawn 
{
public:
	MyActor()
		: INVENT::ISquare2dPawn()
	{
		this->SetColor({ 0.5f, 0.4f,0.3f,1.0f });
		this->SetTexture(INVENT::ITexture2DManagement::Instance().CreateTexture("./Assets/Textures/test.png"));
	}
};

class MyLevel : public INVENT::IBaseLevel 
{
public:
	MyLevel()
		: IBaseLevel()
	{
		auto act = this->CreateActor<MyActor>();

		camera = new INVENT::ICamera();
	}

	virtual bool PRESS_EVENT_KEY_1() override
	{
		std::cout << "camera forward.x: "
			<< camera->GetForwardVector().x
			<< "; camera forward.y: "
			<< camera->GetForwardVector().y
			<< "; camera forward.z: "
			<< camera->GetForwardVector().z << "\n";

		camera->TurnUpWithAngle(45.0f);

		std::cout << "camera forward.x: "
			<< camera->GetForwardVector().x
			<< "; camera forward.y: "
			<< camera->GetForwardVector().y
			<< "; camera forward.z: "
			<< camera->GetForwardVector().z << "\n";

		return false;
	}

	virtual ~MyLevel()
	{

	}

private:
	INVENT::ICamera* camera;

};

class MyWindow : public INVENT::IWindow
{
public:
	MyWindow(unsigned int width = 800, unsigned int height = 600, std::string title = "title")
		: IWindow(width, height, title)
	{
		this->SetGameInstance(std::static_pointer_cast<INVENT::IBaseGameInstance>(MyGameInstance::GetGameInstancePtr()));

		this->SetLevel(new MyLevel);

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
