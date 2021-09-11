#pragma once
#include <RavEngine/Component.hpp>
#include <RavEngine/Queryable.hpp>
#include <RavEngine/App.hpp>

struct CirculateComponent : public RavEngine::Component, public  RavEngine::Queryable<CirculateComponent> {
	float radius = RavEngine::Random::get(0.2,2.0);
	float speed = RavEngine::Random::get(0.2,1.0);
	float height = RavEngine::Random::get(0.2,1.2);
};

struct CirculateSystem : public RavEngine::AutoCTTI {
	inline void Tick(float fpsScale, Ref<CirculateComponent> cc, Ref<RavEngine::Transform> transform) {
		auto time = RavEngine::App::GetCurrentTime();
		transform->SetLocalPosition(vector3(std::cos(time * cc->speed) * cc->radius, std::sin(time * cc->speed) * 0.5 + 1, std::sin(time * cc->speed) * cc->radius));
	}
};

struct LightEntity : public RavEngine::Entity {
	LightEntity() {
		auto mat = std::make_shared<RavEngine::PBRMaterialInstance>(RavEngine::Material::Manager::GetMaterial<RavEngine::PBRMaterial>());
        RavEngine::MeshAssetOptions opt;
        opt.scale = 0.03;
        EmplaceComponent<RavEngine::StaticMesh>(RavEngine::MeshAsset::Manager::GetMesh("sphere.obj",opt),mat);
		auto cc = EmplaceComponent<CirculateComponent>();
		auto light = EmplaceComponent<RavEngine::PointLight>();
		light->Intensity = cc->radius * 1.3;
		light->color = {RavEngine::Random::get(0.f,1.f),RavEngine::Random::get(0.f,1.f),RavEngine::Random::get(0.f,1.f),1};
		mat->SetAlbedoColor(light->color);
	}
};
