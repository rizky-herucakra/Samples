#include "Level.hpp"
#include "Player.hpp"
#include "Speaker.hpp"
#include <RavEngine/App.hpp>
#include <RavEngine/InputManager.hpp>
#include <RavEngine/GUI.hpp>
#include <RavEngine/AudioRoom.hpp>

using namespace RavEngine;
using namespace std;

struct InputNames {
	static constexpr char const
		* MoveForward = "MoveForward",
		* MoveRight = "MoveRight",
		* MoveUp = "MoveUp",
		* LookRight = "LookRight",
		* LookUp = "LookUp";
};

void Level::OnActivate() {
	// lights
	auto lightEntity = make_shared<Entity>();
	lightEntity->EmplaceComponent<AmbientLight>()->Intensity = 0.2;
	lightEntity->EmplaceComponent<DirectionalLight>();
	lightEntity->GetTransform()->LocalRotateDelta(vector3{glm::radians(35.0),glm::radians(-30.0),0});
	Spawn(lightEntity);

	// create the audio room
	auto stage = make_shared<Stage>();
	Spawn(stage);

	// load audio
	auto audio = make_shared<AudioAsset>("The Entertainer.mp3");

	// create speakers
	auto speaker1 = make_shared<Speaker>(audio);
	speaker1->GetTransform()->LocalTranslateDelta(vector3(5,0,-2));
	Spawn(speaker1);

	/*auto speaker2 = make_shared<Speaker>(audio);
	speaker2->GetTransform()->LocalTranslateDelta(vector3(0, 0, 2));
	Spawn(speaker2);*/

	// create player 
	auto player = make_shared<Player>();
	Spawn(player);
	player->GetTransform()->SetLocalRotation(vector3(0, glm::radians(-90.f), 0));
	player->GetTransform()->SetLocalPosition(vector3(-5,2,0));

	// load UI
	auto uiEntity = make_shared<Entity>();
	auto ui = uiEntity->EmplaceComponent<GUIComponent>();
	auto doc = ui->AddDocument("main.rml");
	Spawn(uiEntity);

	std::array<std::string, RoomMat::kNumMaterialNames> names{
		"Transparent",
		"Acoustic Ceiling Tiles",
		"Bare Brick",
		"Painted Brick",
		"Coarse Concrete Block",
		"Painted Concrete Block",
		"Heavy Curtain",
		"Fiberglass Insulation",
		"Thin Glass",
		"Thick Glass",
		"Grass",
		"Linoleum On Concrete",
		"Marble",
		"Metal",
		"Parquet On Concrete",
		"Rough Plaster",
		"Smooth Plaster",
		"Plywood Panel",
		"Polished Concrete or Tile",
		"Sheetrock",
		"Water or Ice Surface",
		"Wood Panel",
		"Uniform"
	};

	for (int i = 0; i < 6; i++) {
		auto sel = doc->CreateElement("select");
		
		for (int j = 0; j < RoomMat::kNumMaterialNames; j++) {
			auto opt = doc->CreateElement("option");
			if (j == 0) {
				opt->SetAttribute("selected", true);
			}
			opt->SetInnerRML(names[j]);
			sel->AppendChild(std::move(opt));
		}

		// change event listener
		struct SelectionEventListener : public Rml::EventListener {
			uint8 roomFace;
			Ref<AudioRoom> room;
			SelectionEventListener(decltype(roomFace) rf, decltype(room) room) : roomFace(rf), room(room){}

			void ProcessEvent(Rml::Event& evt) override {
				auto selbox = static_cast<Rml::ElementFormControlSelect*>(evt.GetTargetElement());
				room->WallMaterials()[roomFace] = static_cast<RoomMat>(selbox->GetSelection());
			}
		};

		sel->AddEventListener(Rml::EventId::Change, new SelectionEventListener(i,stage->GetRoom()));

		doc->GetElementById("materials")->AppendChild(std::move(sel));

	}

	// setup inputs
	auto im = App::inputManager = make_shared<InputManager>();
	
	im->AddAxisMap(InputNames::MoveForward, ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
	im->AddAxisMap(InputNames::MoveRight, ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);

	im->AddAxisMap(InputNames::MoveForward, SDL_SCANCODE_W);
	im->AddAxisMap(InputNames::MoveForward, SDL_SCANCODE_S,-1);
	im->AddAxisMap(InputNames::MoveRight, SDL_SCANCODE_D);
	im->AddAxisMap(InputNames::MoveRight, SDL_SCANCODE_A, -1);
	im->AddAxisMap(InputNames::MoveUp, SDL_SCANCODE_SPACE);
	im->AddAxisMap(InputNames::MoveUp, SDL_SCANCODE_LSHIFT, -1);

	im->AddAxisMap(InputNames::LookRight,Special::MOUSEMOVE_XVEL,-1);
	im->AddAxisMap(InputNames::LookUp,Special::MOUSEMOVE_YVEL,-1);

	im->BindAxis(InputNames::MoveForward, player, &Player::MoveForward, CID::ANY);
	im->BindAxis(InputNames::MoveRight, player, &Player::MoveRight, CID::ANY);
	im->BindAxis(InputNames::MoveUp, player, &Player::MoveUp, CID::ANY);
	im->BindAxis(InputNames::LookRight, player, &Player::LookRight, CID::ANY);
	im->BindAxis(InputNames::LookUp, player, &Player::LookUp, CID::ANY);

	// for gui
	im->AddAxisMap("MouseX", Special::MOUSEMOVE_X);
	im->AddAxisMap("MouseY", Special::MOUSEMOVE_Y);
	im->BindAxis("MouseX", ui, &GUIComponent::MouseX, CID::ANY, 0);
	im->BindAxis("MouseY", ui, &GUIComponent::MouseY, CID::ANY, 0);
	im->BindAnyAction(ui);

	// initialize physics
	InitPhysics();
}

Level::Level() {

}
