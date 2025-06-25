

#include "Invent/Invent.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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

class MyController : public INVENT::IPlayerController2D 
{
public:
	MyController()
		: INVENT::IPlayerController2D()
	{}
	virtual ~MyController(){}

};

class MyActor : public INVENT::ISquare2dPawn 
{
public:
	MyActor()
		: INVENT::ISquare2dPawn()
	{
		this->SetColor({ 0.5f, 0.4f,0.3f,1.0f });
		this->SetTexture(INVENT::ITexture2DManagement::Instance().CreateTexture("./Assets/Textures/test.png"));

		this->SetMoveSpeed(1.0f);

		this->SetScale({ 0.1f,0.1f });
	}

	virtual void Update(float delta) override
	{
		//this->SetWorldPosition(this->GetWorldPosition() + delta * glm::vec3(0.01f, 0.0f, 0.0f));
	}
};

class MyLevel : public INVENT::IBaseLevel 
{
public:
	MyLevel()
		: IBaseLevel()
	{
		auto act = this->CreateActor<MyActor>();
		auto act2 = this->CreateActor<MyActor>();
		act2->SetWorldPosition({ 0.1f,0.0f,-0.1f });

		camera = new INVENT::ICamera();
		camera->SetWorldPosition({ 0.0f,0.0f,3.0f });

		this->CreateControllerPtr<MyController>()->SetSceneCamera(camera);
		this->GetController<MyController>()->AddPlayer(act);
		this->GetController<MyController>()->AddPlayer(act2);

	}

	virtual bool PRESS_EVENT_KEY_1() override
	{
		this->GetController<MyController>()->SetControlPlayerIndex(this->GetController<MyController>()->GetControlPlayerIndex() == 0 ? 1 : 0);

		//auto actor = this->GetController<MyController>()->Get2DPlayerController<MyActor>(0);
		//actor->SetScale({ 1.0f,0.1f });

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

		this->StartThreadPool();
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
	/*MyWindow window;
	window.Start();

	INVENT::IEngine::InstancePtr()->GetIWindow();*/

	INVENT::ICollisionDetection::LineSegment line_s1({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, 0.0f, 0.0f);
	INVENT::ICollisionDetection::LineSegment line_s2({ 1.0f,0.0f,0.0f }, { -1.0f,1.0f,0.0f }, 0.0f, 0.0f);

	glm::vec3 p1{}, p2{};

	auto dist = INVENT::ICollisionDetection::TwoLineSegmentMinDistance(line_s1, line_s2, p1, p2);

	std::cout << "distance : " << dist
		<< " p1 : " << glm::to_string(p1)
		<< " p2 : " << glm::to_string(p2) << "\n";

	/*INVENT::IThreadPool pool(2, 3);
	auto back1 = pool.Submit(2, [](int a, int b) -> int {
		std::cout << b + a << "\n";
		return b + a;
		}, 1, 2);
	pool.SetThreadPriorityNum(1, 2);
	pool.Start();
	auto back2 = pool.Submit(0, [](int a, int b) -> int {
		std::cout << b - a << "\n";
		return b - a;
		}, 1, 2);

	std::cout << back1.get() << "\n";
	std::cout << back2.get() << "\n";*/

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
