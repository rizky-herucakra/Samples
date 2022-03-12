#include "Level.hpp"
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/AnimatorComponent.hpp>
#include <RavEngine/DebugDrawer.hpp>
#include <RavEngine/MeshAssetSkinned.hpp>
#include <RavEngine/SkinnedMeshComponent.hpp>
#include <RavEngine/Utilities.hpp>
#include <RavEngine/InputManager.hpp>
#include "Character.hpp"
#include "CameraEntity.hpp"
#include <RavEngine/SceneLoader.hpp>
#include <FPSSystem.hpp>
#include <RavEngine/PhysicsBodyComponent.hpp>

using namespace RavEngine;
using namespace std;

struct InputNames{
	static constexpr char const
		* MoveForward = "MoveForward",
		* MoveRight = "MoveRight",
		* Sprint = "Sprint",
		* Jump = "Jump",
		* Pound = "Pound";
};

Level::Level(){
	
	auto lights = CreatePrototype<GameObject>();
	lights.EmplaceComponent<AmbientLight>().Intensity = 0.2f;
	lights.GetTransform().LocalTranslateDelta(vector3(0,0.5,6));
	lights.EmplaceComponent<DirectionalLight>();
	lights.GetTransform().LocalRotateDelta(vector3(glm::radians(45.0),glm::radians(45.0),0));
    auto& gui = lights.EmplaceComponent<GUIComponent>();
    gui.AddDocument("ui.rml");

	auto character = CreatePrototype<Character>();
	character.GetTransform().LocalTranslateDelta(vector3(15,5,0));
	character.GetTransform().LocalRotateDelta(vector3(0,glm::radians(90.f),0));
	
	auto camera = CreatePrototype<CameraEntity>(character);
	camera.GetTransform().LocalTranslateDelta(vector3(0,0,0));
	
	auto im = GetApp()->inputManager = RavEngine::New<InputManager>();
	// keyboard
	im->AddAxisMap(InputNames::MoveForward,SDL_SCANCODE_W);
	im->AddAxisMap(InputNames::MoveForward, SDL_SCANCODE_S,-1);
	im->AddAxisMap(InputNames::MoveRight, SDL_SCANCODE_A,-1);
	im->AddAxisMap(InputNames::MoveRight, SDL_SCANCODE_D);
	im->AddAxisMap(InputNames::Sprint, SDL_SCANCODE_LSHIFT);
	im->AddActionMap(InputNames::Jump, SDL_SCANCODE_SPACE);
	im->AddActionMap(InputNames::Pound, SDL_SCANCODE_LCTRL);

	// controller
	im->AddAxisMap(InputNames::MoveForward, ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY, -1);
	im->AddAxisMap(InputNames::MoveRight, ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
	im->AddAxisMap(InputNames::Sprint,  ControllerButton::SDL_CONTROLLER_BUTTON_X);
	im->AddActionMap(InputNames::Jump, ControllerButton::SDL_CONTROLLER_BUTTON_A);
	im->AddActionMap(InputNames::Pound, ControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	im->AddActionMap(InputNames::Pound, ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	
	// controls are sent to the Camera, which then forwards them to the character after determining which way to move
	im->BindAxis(InputNames::MoveForward, camera, &CameraEntity::MoveForward, CID::ANY);
	im->BindAxis(InputNames::MoveRight, camera, &CameraEntity::MoveRight, CID::ANY);
	im->BindAxis(InputNames::Sprint, camera, &CameraEntity::SpeedIncrement, CID::ANY);
	im->BindAction(InputNames::Jump, character, &Character::Jump, ActionState::Pressed, CID::ANY);
	im->BindAction(InputNames::Pound, character, &Character::Pound, ActionState::Pressed, CID::ANY);

	// load the game level
	Ref<PBRMaterialInstance> material = RavEngine::New<PBRMaterialInstance>(Material::Manager::Get<PBRMaterial>());
	auto physmat = RavEngine::New<PhysicsMaterial>(0.5, 0.5, 0);
    MeshAssetOptions opt;
    opt.scale = 1.5;
    opt.keepInSystemRAM = true;
	{
		auto floorplane = CreatePrototype<RavEngine::GameObject>();
		Ref<MeshAsset> sharedMesh = MeshAsset::Manager::Get("level.fbx", "ground", opt);
		material->SetAlbedoColor({ 174.f / 255,210.f / 255,234.f / 255,1 });
        floorplane.EmplaceComponent<StaticMesh>(sharedMesh,material);
		auto& r = floorplane.EmplaceComponent<RigidBodyStaticComponent>(FilterLayers::L0, FilterLayers::L0);
		r.EmplaceCollider<MeshCollider>(sharedMesh, physmat);
	}

	// load the walls
	auto walls = CreatePrototype<GameObject>();
	Ref<MeshAsset> sharedMesh = MeshAsset::Manager::Get("level.fbx", "walls", opt);
    walls.EmplaceComponent<StaticMesh>(sharedMesh,material);
	auto& s = walls.EmplaceComponent<RigidBodyStaticComponent>(FilterLayers::L1, FilterLayers::L1);	// we use L0 to determine floor vs walls
    s.EmplaceCollider<MeshCollider>(sharedMesh, physmat);

	InitPhysics();
    
    EmplaceTimedSystem<FPSSystem,GUIComponent>(std::chrono::seconds(1), "ui.rml", "metrics");    
}

void Level::PostTick(float)
{
	GetApp()->GetRenderEngine().DebugPrint(0,0x4F,"{}", GetApp()->GetRenderEngine().GetCurrentBackendName());
}
