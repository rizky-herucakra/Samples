#pragma once
#include <RavEngine/GameObject.hpp>
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/Utilities.hpp>
#include "CustomMaterials.hpp"

struct InstanceEntity : public RavEngine::GameObject{
    void Create(Ref<RavEngine::MeshAsset> mesh, Ref<InstanceColorMatInstance> matinst, uint32_t count, int32_t range = 30){
        GameObject::Create();
        auto& ism = EmplaceComponent<RavEngine::InstancedStaticMesh>(mesh,matinst);
        ism.Reserve(count);
        
        for(uint32_t i = 0; i < count; i++){
            vector3 pos;
            pos.x = RavEngine::Random::get(-range, range);
            pos.y = RavEngine::Random::get(-range, range);
            pos.z = RavEngine::Random::get(-range, range);

            vector3 rot(deg_to_rad(pos.x), deg_to_rad(pos.y), deg_to_rad(pos.z));
            
            auto& inst = ism.AddInstance();
            inst.translate = pos;
            inst.rotate = rot;
            inst.scale = vector3(1,1,1);
        }
    }
};
