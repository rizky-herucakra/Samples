#pragma once
#include <RavEngine/Entity.hpp>
#include <RavEngine/RPCComponent.hpp>
#include <RavEngine/Utilities.hpp>
#include <RavEngine/AccessType.hpp>
#include <RavEngine/QueryIterator.hpp>
#include <RavEngine/StaticMesh.hpp>
#include <RavEngine/Debug.hpp>
#include "NetTransform.hpp"

struct MoveEntities : public RavEngine::AutoCTTI {
	inline void Tick(float fpsScale, RavEngine::AccessRead<PathData> pd, RavEngine::AccessReadWrite<RavEngine::Transform> tr, RavEngine::AccessRead<RavEngine::NetworkIdentity> ni) {

		// use the sine of global time
		auto netid = ni.get();
		if (netid->IsOwner()) {
			auto transform = tr.get();
			auto pathdata = pd.get();

			auto t = RavEngine::App::currentTime();

			auto pos = vector3(
				std::sin(t * pathdata->xtiming + pathdata->offset) * pathdata->scale,
				std::sin(t * pathdata->ytiming + pathdata->offset) * pathdata->scale,
				std::sin(t * pathdata->ztiming + pathdata->offset) * pathdata->scale
			);
			transform->SetWorldPosition(pos);
			auto rot = quaternion(pos);
			transform->SetLocalRotation(rot);
		}
	}

	inline constexpr auto QueryTypes() const {
		return RavEngine::QueryIteratorAND <PathData, RavEngine::Transform, RavEngine::NetworkIdentity>();
	}
};


struct NetEntity : public RavEngine::Entity, public RavEngine::NetworkReplicable {

	static Ref<RavEngine::PBRMaterialInstance> matinst;
	static Ref<RavEngine::MeshAsset> mesh;

	inline void CommonInit() {
		auto rpc = EmplaceComponent<RavEngine::RPCComponent>();
		auto rpccomp = EmplaceComponent<NetTransform>();
		rpc->RegisterServerRPC(RavEngine::to_underlying(RPCs::UpdateTransform), rpccomp, &NetTransform::UpdateTransform);
		rpc->RegisterClientRPC(RavEngine::to_underlying(RPCs::UpdateTransform), rpccomp, &NetTransform::UpdateTransform);

		if (!matinst) {
			matinst = std::make_shared<RavEngine::PBRMaterialInstance>(RavEngine::Material::Manager::AccessMaterialOfType<RavEngine::PBRMaterial>());
		}
		if (!mesh) {
			mesh = std::make_shared<RavEngine::MeshAsset>("cube.obj",0.1);
		}

		EmplaceComponent<RavEngine::StaticMesh>(mesh, matinst);
		EmplaceComponent<InterpolationTransform>();
	}

	// server constructor
	NetEntity() {
		CommonInit();
		EmplaceComponent<RavEngine::NetworkIdentity>();
	}

	// invoked when spawned over the network
	NetEntity(const uuids::uuid& id) {
		CommonInit();
		EmplaceComponent<RavEngine::NetworkIdentity>(id);
		EmplaceComponent<PathData>();		// since clients control their objects, the server does not need to allocate this
	}

	RavEngine::ctti_t NetTypeID() const override {
		return RavEngine::CTTI<NetEntity>();
	}
};