#pragma once

#include <RavEngine/Entity.hpp>
#include <RavEngine/BuiltinMaterials.hpp>
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/ScriptComponent.hpp>
#include <RavEngine/Material.hpp>
#include <RavEngine/PhysicsCollider.hpp>
#include <RavEngine/PhysicsBodyComponent.hpp>
#include <RavEngine/ChildEntityComponent.hpp>
#include <RavEngine/Light.hpp>

//marker for querying
class PuckComponent : public RavEngine::Component, public RavEngine::Queryable<PuckComponent>{};

class Puck : public RavEngine::Entity{
public:
    virtual ~Puck(){};
    static Ref<RavEngine::PBRMaterialInstance> material;
    Puck(){
        auto puckmesh = AddComponent<RavEngine::StaticMesh>(new RavEngine::StaticMesh(new RavEngine::MeshAsset("HockeyPuck.obj",0.03)));
        if(material == nullptr){
			material = new RavEngine::PBRMaterialInstance(RavEngine::Material::Manager::AccessMaterialOfType<RavEngine::PBRMaterial>());
			material->SetAlbedoColor({0.2,0.2,0.2,1});
        }
        puckmesh->SetMaterial(material);
        auto dyn = AddComponent<RavEngine::RigidBodyDynamicComponent>(new RavEngine::RigidBodyDynamicComponent());
		AddComponent<RavEngine::SphereCollider>(new RavEngine::SphereCollider(0.3,new RavEngine::PhysicsMaterial(0,0,1),vector3(0,0.3,0)));
		
		//prevent puck from falling over
		dyn->SetAxisLock(RavEngine::RigidBodyDynamicComponent::AxisLock::Angular_X | RavEngine::RigidBodyDynamicComponent::AxisLock::Angular_Z);
		dyn->SetMass(1.0);
		
		Ref<Entity> lightEntity = new Entity();
		
		AddComponent<RavEngine::ChildEntityComponent>(new RavEngine::ChildEntityComponent(lightEntity));
		
		auto light = lightEntity->AddComponent<RavEngine::PointLight>(new RavEngine::PointLight());
		light->color = {0,0,1,1};
		
		transform()->AddChild(lightEntity->transform());
		
		lightEntity->transform()->LocalTranslateDelta(vector3(0,1,0));
		
		AddComponent<PuckComponent>(new PuckComponent());
    }
};
