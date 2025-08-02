

#include "Invent/Invent.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>

#include <filesystem>

// #include <Windows.h>


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

		//auto id = INVENT::ITexture2DManagement::Instance().CreateTexture(nullptr, "./Assets/Textures/test.png");
		//std::cout << id;
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
		this->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		this->SetTexture(INVENT::ITexture2DManagement::Instance().CreateTexture("./Assets/Textures/test.png"));

		this->SetMoveSpeed(1.0f);

		collider_id = this->CreateCollider<INVENT::IColliderBox>(this, glm::vec3{ 0.0f,0.0f,0.0f }, glm::vec3{ 1.0f,1.0f,1.0f });

		this->GetCollider<INVENT::IColliderBox>(collider_id)->BindBlockCollisionFunc([](const std::unordered_set<INVENT::IColliderBase*>& colliders) {
			std::cout << "Block other Colliders num : " << colliders.size() << "\n";
			});
		//this->SetScale({ 0.1f,0.1f });

		this->SetFlip(true, false);
		
		//this->AddCollider(collider);
	}

	virtual void Update(float delta) override
	{
		//std::cout << "actor position : " << glm::to_string(this->GetWorldPosition()) << "\n";
	}

	virtual void PRESS_EVENT_KEY_2() override
	{
		std::cout << "PRESS_EVENT_KEY_2\n";
	}

	virtual void SetWorldRotation(const glm::vec3& rotation) override
	{
		ISquare2dPawn::SetWorldRotation(rotation);
		auto collider = this->GetCollider<INVENT::IColliderBox>(collider_id);
		collider->SetRotation(rotation + collider->GetRotation());
	}

private:
	INVENT::IActor2D::AColliderID collider_id;
};

class MyController : public INVENT::IPlayerController2D
{
public:
	MyController()
		: INVENT::IPlayerController2D()
	{}
	virtual ~MyController() {}

	virtual bool PRESS_EVENT_KEY_2() override
	{
		INVENT::IEventLayer::PRESS_EVENT_KEY_2();

		auto actor = (MyActor*)(this->Get2DPlayerController(0));
		actor->SetFlip(false, true);

		return false;
	}

};

class MyTileMap : public INVENT::ITileMap
{
public:
	MyTileMap()
		: ITileMap()
	{
		this->SetTileMapSize(9, 10);

		//auto sprite = this->GetSprite(5, 5);
		//if (sprite)
		//{
		//	sprite->SetColor({ 1.0f,0.0f,0.0f,1.0f });
		//}

		this->DynamicInit({ 

			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{0.0f,1.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},
			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},
			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},
			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},
			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},

			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},
			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},
			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},
			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},
			{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},	{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}},{{1.0f,0.0f,0.0f,1.0f}}
			
			});

		this->SetWorldPosition({ -5.0f,5.0f,0.0f });
		this->SetWorldRotation({ 45.0f,0.0f,0.0f });
	}

	virtual ~MyTileMap(){}

private:

};

class MyLevel : public INVENT::IBaseLevel 
{
public:
	MyLevel()
		: IBaseLevel()
	{
		auto act = this->CreateActor<MyActor>();
		auto act2 = this->CreateActor<MyActor>();
		act->SetWorldPosition({ 0.0f,2.0f,0.0f });
		act2->SetWorldRotation({ 0.0f,0.0f,45.0f });

		auto tilemap = this->CreateActor<MyTileMap>();

		/*auto collider1 = new INVENT::IColliderBox(nullptr, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f });
		auto collider2 = new INVENT::IColliderBox(nullptr, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, { 0.0f,0.0f,45.0f });
		collider1->BindBlockCollisionFunc([](const std::unordered_set<INVENT::IColliderBase*>& colliders) {
			std::cout << "Block other Colliders num : " << colliders.size() << "\n";
			});
		collider1->BindBeginOverlapFunc([](const std::unordered_set<INVENT::IColliderBase*>& colliders) {
			std::cout << "begin overlap other Colliders num : " << colliders.size() << "\n";
			});

		act->AddCollider(collider1);
		act2->AddCollider(collider2);*/

		camera = new INVENT::ICamera();
		camera->SetWorldPosition({ 0.0f,0.0f,10.0f });

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
		this->StartThreadPool();

		//this->SetLevel(new MyLevel);

		std::thread create_level([this]() {
			this->SetLevel(new MyLevel);
			});

		create_level.detach();

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

//static void add(int a, int b)
//{
//	std::cout << a + b << "\n";
//}


int main()
{
	MyWindow window;
	window.Start();

	/*INVENT::Event<int, int> myevent;
	myevent.AddFunction(add);

	myevent(1, 2);*/

	//INVENT::IEngine::InstancePtr()->GetIWindow();

 //	INVENT::ITagTrie trie;
	//trie.Insert("apple.orange");
	//trie.Insert("apple.orange.banana");
	//trie.Insert("apple.pear");
	//trie.Insert("foo.bar");

	//// Search
	//auto print_search = [&](const std::string& kw) {
	//	auto res = trie.Search(kw);
	//	if (res.Id) std::cout << "Search " << kw << ": found, id=" << res.Id << " vaild=" << res.Vaild << std::endl;
	//	else std::cout << "Search " << kw << ": not found" << std::endl;
	//	};

	//// startsWith
	//auto print_startsWith = [&](const std::string& prefix) {
	//	std::vector<INVENT::ITagTrie::TrieNodeId> ids;
	//	trie.SearchAbout(prefix, ids);
	//	std::cout << "startsWith " << prefix << ": ";
	//	if (ids.empty()) std::cout << "no match";
	//	else
	//	{
	//		std::cout << "full-word ids=[";
	//		for (size_t i = 0; i < ids.size(); ++i)
	//		{
	//			if (i) std::cout << ",";
	//			std::cout << ids[i].Id;
	//		}
	//		std::cout << "]";
	//	}
	//	std::cout << std::endl;
	//	};

	//print_search("apple.orange");
	//print_search("apple.orange.banana");
	//print_search("apple.orange.banana.lemon");

	//print_startsWith("apple.orange");
	//print_startsWith("apple");
	//print_startsWith("foo");
	//print_startsWith("foo.baz");

	//// 删除测试
	//std::cout << "\n== 删除 apple.orange ==" << std::endl;
	//trie.Erase("apple.orange");
	//print_search("apple.orange");
	//print_search("apple.orange.banana");
	//print_startsWith("apple.orange");
	//print_startsWith("apple");

	//std::cout << "\n== 添加 apple.orange（应复用id）==" << std::endl;
	//auto id = trie.Insert("apple.orange");
	//std::cout << "\n== 第二次添加 apple.orange（应复用id）==" << id.Id << " vaild ==" << id.Vaild << std::endl;
	//print_search("apple.orange");
	//print_startsWith("apple.orange");
	//print_startsWith("apple");

	//INVENT::IColliderBox box1(nullptr, { 0.0f,0.0f,0.0f }, { 2.0f, 2.0f, 2.0f }, { 0.0f, 0.0f, 0.0f });
	//INVENT::IColliderBox box2(nullptr, { 0.0f,0.01f,0.0f }, { 2.0f, 2.0f, 2.0f }, { 0.0f, 0.0f, 0.0f });
	////INVENT::IColliderBall ball(nullptr, { 0.0f,2.0f,0.0f }, 0.6f);
	////INVENT::IColliderCapsule capsule1(nullptr, { 0.0f,3.0f,0.0f }, 1.0f, 8.0f, { 60.0f,0.0f,0.0f });

	//glm::vec3 diretion{};
	//float distance = 0.0f;

	//bool iscollid = INVENT::ICollisionDetection::IsCollision(&box1, &box2, diretion, distance);
	//if(iscollid)
	//	std::cout << "direction: " << glm::to_string(diretion) << " distance: " << distance << "\n";

	/*INVENT::ICollisionDetection::LineSegment line_s1({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, 0.0f, 0.0f);
	INVENT::ICollisionDetection::LineSegment line_s2({ 1.0f,0.0f,0.0f }, { -1.0f,1.0f,0.0f }, 0.0f, 0.0f);

	glm::vec3 p1{}, p2{};*/

	/*auto dist = INVENT::ICollisionDetection::TwoLineSegmentMinDistance(line_s1, line_s2, p1, p2);

	std::cout << "distance : " << dist
		<< " p1 : " << glm::to_string(p1)
		<< " p2 : " << glm::to_string(p2) << "\n";*/

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
