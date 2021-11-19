#include "GameWorld.hpp"
#include <RavEngine/CameraComponent.hpp>
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/BuiltinMaterials.hpp>
#include <RavEngine/Tween.hpp>
#include <RavEngine/Light.hpp>
#include <RavEngine/InputManager.hpp>
#include <fmt/format.h>
#include "MainMenu.hpp"
#include <RavEngine/Debug.hpp>
#include <RavEngine/AudioRoom.hpp>
#include <RavEngine/Ref.hpp>

Ref<PBRMaterialInstance> Puck::material;
using namespace std;

Tween<decimalType,decimalType> t;

GameWorld::GameWorld(int numplayers) : numplayers(numplayers){}

void GameWorld::OnActivate(){
	auto cameraActor = CreatePrototype<GameObject>();
	cameraActor.EmplaceComponent<CameraComponent>().SetActive(true);
	cameraActor.EmplaceComponent<AudioListener>();
	cameraBoom.GetTransform().SetWorldPosition(vector3(0,0,0));
	
	cameraBoom.GetTransform().AddChild(ComponentHandle<Transform>(cameraActor));
	cameraActor.GetTransform().LocalTranslateDelta(vector3(0,3,3));
	cameraActor.GetTransform().LocalRotateDelta(vector3(glm::radians(-90.0),0,0));
		
	//create the puck
	puck.GetTransform().LocalTranslateDelta(vector3(0,3,0));
	
	InitPhysics();
	
	//intro animation
	t = Tween<decimalType,decimalType>([=](decimalType d, decimalType p) mutable{
		cameraBoom.GetTransform().SetLocalRotation(vector3(glm::radians(d),glm::radians(90.0),0));
		cameraActor.GetTransform().SetLocalPosition(vector3(0,p,0));
	},90,15);
	t.AddKeyframe(3, TweenCurves::QuinticInOutCurve,0,7);
	
	auto lightmain = CreatePrototype<GameObject>();
	auto& key = lightmain.EmplaceComponent<DirectionalLight>();
	key.Intensity = 1;
	key.color = {1,0.6,0.404,1};
	auto& fill = lightmain.EmplaceComponent<AmbientLight>();
	fill.Intensity=0.4;
	fill.color = {0,0,1,1};
	lightmain.GetTransform().LocalRotateDelta(vector3(glm::radians(45.0),0,glm::radians(-45.0)));
	auto& room = lightmain.EmplaceComponent<AudioRoom>();
	room.SetRoomDimensions(vector3(30,30,30));
	room.WallMaterials()[0] = RoomMat::kMarble;
		
	//inputs
	Ref<InputManager> is = make_shared<InputManager>();
	is->AddAxisMap("P1MoveUD", SDL_SCANCODE_W,-1);
	is->AddAxisMap("P1MoveUD", SDL_SCANCODE_S);
	is->AddAxisMap("P1MoveLR", SDL_SCANCODE_D,-1);
	is->AddAxisMap("P1MoveLR", SDL_SCANCODE_A);
	
	is->AddAxisMap("P2MoveUD", SDL_SCANCODE_UP,-1);
	is->AddAxisMap("P2MoveUD", SDL_SCANCODE_DOWN);
	is->AddAxisMap("P2MoveLR", SDL_SCANCODE_RIGHT,-1);
	is->AddAxisMap("P2MoveLR", SDL_SCANCODE_LEFT);
	
	p1 = CreatePrototype<Paddle>(ColorRGBA{1,0,0,1});
	auto& p1s = p1.EmplaceComponent<Player>();
	
	p2 = CreatePrototype<Paddle>(ColorRGBA{0,1,0,1});
	auto& p2s = p2.EmplaceComponent<Player>();
    ComponentHandle<Player> p2h(p2), p1h(p1);
	switch(numplayers){
		case 2:
			is->BindAxis("P2MoveUD", p2h, &Player::MoveUpDown, CID::ANY);
			is->BindAxis("P2MoveLR", p2h, &Player::MoveLeftRight, CID::ANY);
			break;
		case 0:
			//set p1 as a bot
			p1.EmplaceComponent<BotPlayer>(p1h, true);
			// no break here, want to create a bot for p2 in either case
		case 1:
			//create a bot player
			p2.EmplaceComponent<BotPlayer>(p2h, false);
			break;
		default:
			Debug::Fatal("Invalid number of players: {}", numplayers);
	}
	
	if (numplayers > 0){
		//bind inputs
		is->BindAxis("P1MoveUD", p1h, &Player::MoveUpDown, CID::ANY);
		is->BindAxis("P1MoveLR", p1h, &Player::MoveLeftRight, CID::ANY);
	}
	
	auto gamegui = CreatePrototype<Entity>();
	auto& context = gamegui.EmplaceComponent<GUIComponent>();
	auto doc = context.AddDocument("demo.rml");
	Scoreboard = doc->GetElementById("scoreboard");
	App::inputManager = is;
	
	Reset();
}

void GameWorld::PostTick(float f)
{
	t.Step(f);
	
	//if the puck's z position > 6 then the right side must have scored
	auto pos = puck.GetTransform().GetWorldPosition();
	if (pos.z > 6){
		p2score++;
		Reset();
	}
	else if (pos.z < -6){
		p1score++;
		Reset();
	}
}

void GameWorld::Reset(){
	puck.GetTransform().SetWorldPosition(vector3(0,2,0));
	p1.GetTransform().SetWorldPosition(vector3(2,2,3));
	p2.GetTransform().SetWorldPosition(vector3(-2,2,-3));

	//clear velocities
	auto zerovel = [](Entity e){
		e.GetComponent<RigidBodyDynamicComponent>().SetLinearVelocity(vector3(0,0,0), false);
	};
	
	zerovel(p1);
	zerovel(p2);
	zerovel(puck);
	Scoreboard->SetInnerRML(StrFormat("Score: {} - {}", p1score, p2score).c_str());
	
	if (p1score >= numToWin){
		Scoreboard->SetInnerRML("Player 1 Wins!");
		GameOver();
	}
	else if (p2score >= numToWin){
		Scoreboard->SetInnerRML("Player 2 Wins!");
		GameOver();
	}
}

void GameWorld::GameOver(){
	
	Entity gameOverMenu = CreatePrototype<Entity>();
	auto& ctx = gameOverMenu.EmplaceComponent<GUIComponent>();
	auto doc = ctx.AddDocument("gameover.rml");
	
	struct MenuEventListener: public Rml::EventListener{
		WeakRef<GameWorld> gm;
		MenuEventListener(WeakRef<GameWorld> w) : gm(w){}
		void ProcessEvent(Rml::Event& event) override{
			App::DispatchMainThread([=]{
				auto world = make_shared<MainMenu>();
				App::AddReplaceWorld(gm.lock(),world);
			});
		}
	};
	struct ReplayEventListener: public Rml::EventListener{
		WeakRef<GameWorld> gm;
		ReplayEventListener(WeakRef<GameWorld>g) : gm(g){}
		bool isLoading = false;
		void ProcessEvent(Rml::Event& event) override{
			if (!isLoading){
				isLoading = true;
				App::DispatchMainThread([=]{
					auto world = make_shared<GameWorld>(gm.lock()->numplayers);
					App::AddReplaceWorld(gm.lock(),world);
				});
			}
		}
	};
	doc->GetElementById("mainmenu")->AddEventListener(Rml::EventId::Click, new MenuEventListener(static_pointer_cast<GameWorld>(shared_from_this())));
	doc->GetElementById("replay")->AddEventListener(Rml::EventId::Click, new ReplayEventListener(static_pointer_cast<GameWorld>(shared_from_this())));
	
	//create a new input manager to stop game inputs and enable UI inputs
	Ref<InputManager> im = make_shared<InputManager>();
	
	im->AddAxisMap("MouseX", Special::MOUSEMOVE_X);
	im->AddAxisMap("MouseY", Special::MOUSEMOVE_Y);
	
    ComponentHandle<GUIComponent> gh(gameOverMenu);
	im->BindAxis("MouseX", gh, &GUIComponent::MouseX, CID::ANY, 0);
	im->BindAxis("MouseY", gh, &GUIComponent::MouseY, CID::ANY, 0);
    //TODO: FIX
	//im->BindAnyAction(ctx);
	
	App::inputManager = im;
}

GameWorld::GameWorld(const GameWorld& other) : GameWorld(other.numplayers){}
