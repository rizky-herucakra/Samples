#include "Level.hpp"
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/AnimatorComponent.hpp>

using namespace RavEngine;
using namespace std;

void Level::SetupInputs(){
	
	Ref<Entity> camlights = make_shared<Entity>();
	camlights->EmplaceComponent<CameraComponent>()->setActive(true);
	camlights->EmplaceComponent<AmbientLight>()->Intensity = 0.2;
	camlights->transform()->LocalTranslateDelta(vector3(0,0,5));
	
	Ref<Entity> dirlight = make_shared<Entity>();
	dirlight->EmplaceComponent<DirectionalLight>();
	dirlight->transform()->LocalRotateDelta(vector3(glm::radians(45.0),glm::radians(45.0),0));
	
	auto cube = make_shared<Entity>();
	auto cubemesh = cube->EmplaceComponent<StaticMesh>(make_shared<MeshAsset>("cube.obj"));
	cubemesh->SetMaterial(make_shared<PBRMaterialInstance>(Material::Manager::AccessMaterialOfType<PBRMaterial>()));
	cube->transform()->LocalRotateDelta(quaternion(1,1,1,1));
	
	//setup animation
	auto skeleton = make_shared<SkeletonAsset>("");
	auto animatorComponent = cube->EmplaceComponent<AnimatorComponent>(skeleton);
	auto blendTree = make_shared<AnimBlendTree>();
	animatorComponent->SetBlendTree(blendTree);
	auto clip = make_shared<AnimationAsset>("");
	AnimBlendTree::Node node(clip, normalized_vec2(0,1));
	blendTree->InsertNode(0,node);
	
	
	Spawn(camlights);
	Spawn(dirlight);
	Spawn(cube);
}
