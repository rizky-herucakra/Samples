#include "Level.hpp"
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/AnimatorComponent.hpp>
#include <RavEngine/DebugDraw.hpp>
#include <RavEngine/MeshAssetSkinned.hpp>
#include <RavEngine/SkinnedMeshComponent.hpp>
#include <RavEngine/Utilities.hpp>
#include <RavEngine/InputManager.hpp>
#include "Character.hpp"
#include "CameraEntity.hpp"

using namespace RavEngine;
using namespace std;

struct InputNames{
	static constexpr char const
		* MoveForward = "MoveForward",
		* MoveRight = "MoveRight",
		* Sprint = "Sprint",
		* Jump = "Jump";
};

void Level::SetupInputs(){
	
	Ref<Entity> lights = make_shared<Entity>();
	lights->EmplaceComponent<AmbientLight>()->Intensity = 0.2;
	lights->transform()->LocalTranslateDelta(vector3(0,0.5,6));
	lights->EmplaceComponent<DirectionalLight>();
	lights->transform()->LocalRotateDelta(vector3(glm::radians(45.0),glm::radians(45.0),0));
	Spawn(lights);

	auto character = make_shared<Character>();
	character->transform()->LocalTranslateDelta(vector3(0,5,0));
	Spawn(character);
	
	auto camera = make_shared<CameraEntity>(character);
	camera->transform()->LocalTranslateDelta(vector3(0,0,0));
	Spawn(camera);
	
	auto im = App::inputManager = make_shared<InputManager>();
	// keyboard
	im->AddAxisMap(InputNames::MoveForward,SDL_SCANCODE_W);
	im->AddAxisMap(InputNames::MoveForward, SDL_SCANCODE_S,-1);
	im->AddAxisMap(InputNames::MoveRight, SDL_SCANCODE_A,-1);
	im->AddAxisMap(InputNames::MoveRight, SDL_SCANCODE_D);
	im->AddAxisMap(InputNames::Sprint, SDL_SCANCODE_LSHIFT);
	im->AddActionMap(InputNames::Jump, SDL_SCANCODE_SPACE);

	// controller
	im->AddAxisMap(InputNames::MoveForward, ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
	im->AddAxisMap(InputNames::MoveRight, ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
	
	// controls are sent to the Camera, which then forwards them to the character after determining which way to move
	im->BindAxis(InputNames::MoveForward, camera, &CameraEntity::MoveForward, CID::ANY);
	im->BindAxis(InputNames::MoveRight, camera, &CameraEntity::MoveRight, CID::ANY);
	im->BindAxis(InputNames::Sprint, camera, &CameraEntity::SpeedIncrement, CID::ANY);
	im->BindAction(InputNames::Jump, character, &Character::Jump, ActionState::Pressed, CID::ANY);

	// load the game level
	Ref<PBRMaterialInstance> material = make_shared<PBRMaterialInstance>(Material::Manager::AccessMaterialOfType<PBRMaterial>());
	auto physmat = make_shared<PhysicsMaterial>(0.5, 0.5, 0);
	{
		auto floorplane = make_shared<RavEngine::Entity>();
		Ref<MeshAsset> sharedMesh = make_shared<MeshAsset>("level_walkable.obj", 1.0, true);
		material->SetAlbedoColor({ 174.0 / 255,210.0 / 255,234.0 / 255,1 });
		floorplane->EmplaceComponent<StaticMesh>(sharedMesh)->SetMaterial(material);
		floorplane->EmplaceComponent<RigidBodyStaticComponent>(FilterLayers::L0, FilterLayers::L0);
		floorplane->EmplaceComponent<MeshCollider>(sharedMesh, physmat);
		Spawn(floorplane);
	}

	// load the walls
	auto walls = make_shared<Entity>();
	Ref<MeshAsset> sharedMesh = make_shared<MeshAsset>("level_walls.obj", 1.0, true);
	walls->EmplaceComponent<StaticMesh>(sharedMesh)->SetMaterial(material);
	walls->EmplaceComponent<RigidBodyStaticComponent>(FilterLayers::L1, FilterLayers::L1);	// we use L0 to determine floor vs walls
	walls->EmplaceComponent<MeshCollider>(sharedMesh, physmat);
	Spawn(walls);

	InitPhysics();
}
