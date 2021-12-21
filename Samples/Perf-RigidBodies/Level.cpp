#include "Level.hpp"
#include "Objects.hpp"
#include <RavEngine/App.hpp>
#include <RavEngine/PhysicsLinkSystem.hpp>
#include <FPSSystem.hpp>
#include <RavEngine/CameraComponent.hpp>

using namespace std;
using namespace RavEngine;

static inline vector3 GenSpawnpoint(){
	constexpr auto dist = 30;
	return vector3(Random::get(-dist,dist),10,Random::get(-dist,dist));
}

struct RotationSystem : public RavEngine::AutoCTTI{
	
	inline void operator()(float fpsScale, const RotationComponent& rotation, Transform& transform) const{
		auto time = GetApp()->GetCurrentTime();
		decimalType xrot = std::sin(time * rotation.xspeed) * rotation.maxDeg;
		decimalType yrot = std::sin(time * rotation.yspeed) * rotation.maxDeg;
		decimalType zrot = std::sin(time * rotation.zspeed) * rotation.maxDeg;
		
		transform.SetLocalRotation(vector3(glm::radians(xrot),glm::radians(yrot),glm::radians(zrot)));
	}
};

struct RespawnSystem : public RavEngine::AutoCTTI{
	inline void operator()(float fpsScale, RigidBodyDynamicComponent& rigid, Transform& transform) const{
		if (transform.GetWorldPosition().y < -20){
			transform.SetWorldPosition(GenSpawnpoint());
				rigid.SetLinearVelocity(vector3(0,0,0), false);
		}
	}
};

struct SpawnerMarker : public AutoCTTI{};

struct SpawnerSystem : public RavEngine::AutoCTTI{
	Level* ownWorld;
	Ref<MeshAsset> mesh;
	Ref<PBRMaterialInstance> mat;
	Ref<PhysicsMaterial> physmat;
	Ref<Texture> texture;
    ComponentHandle<GUIComponent> gh;
	
	SpawnerSystem(decltype(ownWorld) world, decltype(gh) gh_i) :
		mat(std::make_shared<PBRMaterialInstance>(Material::Manager::Get<PBRMaterial>())),
		physmat(std::make_shared<PhysicsMaterial>(0.3, 0.3, 0.1)),
		texture(Texture::Manager::Get("checkerboard.png")),
		ownWorld(world),
        gh(gh_i)
	{
		mat->SetAlbedoTexture(texture);
        MeshAssetOptions opt;
        opt.keepInSystemRAM = true;
		mesh = RavEngine::MeshAsset::Manager::Get("sphere.obj",opt);
	}
	
    static constexpr auto total = 5000;
	int count = total;
    
    int totalLostTicks = 0;
    static constexpr auto maxLostTicks = 100;
	
	inline void operator()(float fpsScale, SpawnerMarker&){
		if (count > 0){
			// spawn rigid bodies
			GetApp()->DispatchMainThread([&](){
                auto rigid = ownWorld->CreatePrototype<RigidBody>(mat,mesh, physmat, RigidBody::BodyType::Sphere);
                
                rigid.GetTransform().LocalTranslateDelta(GenSpawnpoint());
                rigid.GetTransform().SetLocalScale(vector3(0.5,0.5,0.5));
            });
            
			count--;
            
            auto lastTPS = GetApp()->CurrentTPS();
            auto lastFPS = GetApp()->GetRenderEngine().GetCurrentFPS();
            auto& guic = ownWorld->GetComponent<GUIComponent>();
            if (total - count > 30 && ( lastTPS < 30 && lastFPS < 30)){
                totalLostTicks++;
            }
            if(totalLostTicks < maxLostTicks){
                guic.EnqueueUIUpdate([=]{
                    gh->GetDocument("ui.rml")->GetElementById("readout")->SetInnerRML(StrFormat("{}/{} balls", total - count,total));
                });
            }
            else{
                auto spawned = total - count;
                guic.EnqueueUIUpdate([=]{
                    gh->GetDocument("ui.rml")->GetElementById("readout")->SetInnerRML(StrFormat("{}/{} balls (Detected dip in performance, stopping)", spawned,total));
                });
                ownWorld->DispatchAsync([](){
                    Debug::Log("Ran dispatched fun");
                }, 3);
                count = 0;
            }
		}
	}
};

void Level::OnActivate(){
	
	// create camera and lights
	auto camEntity = CreatePrototype<GameObject>();
	auto& camera = camEntity.EmplaceComponent<CameraComponent>();
	camera.SetActive(true);
	camera.farClip = 1000;
	camEntity.GetTransform().LocalTranslateDelta(vector3(0,10*5,20*5));
	camEntity.GetTransform().LocalRotateDelta(vector3(glm::radians(-30.0f),0,0));
    auto& gui = camEntity.EmplaceComponent<GUIComponent>();
    gui.AddDocument("ui.rml");
	
	auto lightEntity = CreatePrototype<GameObject>();
	auto& ambientLight = lightEntity.EmplaceComponent<AmbientLight>();
	auto& dirLight = lightEntity.EmplaceComponent<DirectionalLight>();
	lightEntity.EmplaceComponent<SpawnerMarker>();
	dirLight.Intensity = 1.0;
	ambientLight.Intensity = 0.2;
	lightEntity.GetTransform().SetLocalRotation(vector3(0,glm::radians(45.0),glm::radians(45.0)));
	
	// create ground
	auto ground = CreatePrototype<Ground>();
	
	// initialize physics
	InitPhysics();
	
	// load systems
	EmplaceSystem<RotationSystem,RotationComponent,Transform>();
    
    CreateDependency<RotationSystem, PhysicsLinkSystemRead>();
    CreateDependency<RotationSystem, PhysicsLinkSystemWrite>();
    
	EmplaceSystem<RespawnSystem,RigidBodyDynamicComponent,Transform>();
    CreateDependency<RespawnSystem, PhysicsLinkSystemRead>();
    CreateDependency<RespawnSystem, PhysicsLinkSystemWrite>();
    
	EmplaceTimedSystem<SpawnerSystem,SpawnerMarker>(std::chrono::milliseconds(50),this,ComponentHandle<GUIComponent>(camEntity));
    EmplaceTimedSystem<FPSSystem,GUIComponent>(std::chrono::seconds(1),"ui.rml","metrics");

}
