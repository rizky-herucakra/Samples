#include "Level.hpp"
#include "NetEntity.hpp"
#include <RavEngine/GUI.hpp>
#include <RavEngine/InputManager.hpp>
#include <RavEngine/GameObject.hpp>
#include <RavEngine/CameraComponent.hpp>

using namespace RavEngine;
using namespace std;

STATIC(NetEntity::matinst);

void Level::OnActivate() {

	// create camera and lights
	auto camEntity = CreatePrototype<GameObject>();
	auto& camera = camEntity.EmplaceComponent<CameraComponent>();
	camera.SetActive(true);
	camEntity.GetTransform().LocalTranslateDelta(vector3(0, 0, 10));

	auto lightEntity = CreatePrototype<GameObject>();
	auto& ambientLight = lightEntity.EmplaceComponent<AmbientLight>();
	auto& dirLight = lightEntity.EmplaceComponent<DirectionalLight>();
	ambientLight.Intensity = 0.2;
	lightEntity.GetTransform().SetLocalRotation(vector3(0, glm::radians(45.0), glm::radians(45.0)));

	// spawn the management relay if on server
	// if on client, this will be spawned automatically
	if (App::networkManager.IsServer()) {
		SetupServer();
	}
	else {
		SetupClient();
	}
}

void Level::ServerUpdateGUI()
{
	// get the GUIComponent
	auto& gui = GetComponent<GUIComponent>();
	auto doc = gui.GetDocument("server.rml");
	auto root = doc->GetElementById("root");
	root->SetInnerRML("");	//clear element
	for (const auto& con : App::networkManager.server->GetClients()) {
		auto textNode = doc->CreateElement("p");
		auto button = doc->CreateElement("button");
		button->SetInnerRML("Kick");
		textNode->SetInnerRML(StrFormat("id = {}",con));

		struct KickEventListener : public Rml::EventListener {
			HSteamNetConnection con;
			KickEventListener(decltype(con) c) : con(c){}

			void ProcessEvent(Rml::Event& event) final {
				App::networkManager.server->DisconnectClient(con,1);
			}
		};

		button->AddEventListener(Rml::EventId::Click, new KickEventListener(con));

		textNode->AppendChild(std::move(button));
		root->AppendChild(std::move(textNode));
	}
}

void Level::SetupServer()
{
    CreatePrototype<ManagementRelay>()
	systemManager.EmplaceSystem<TweenEntities>();
	App::networkManager.server->OnClientConnected = [&](HSteamNetConnection) {
		ServerUpdateGUI();
	};
	App::networkManager.server->OnClientDisconnected = [&](HSteamNetConnection) {
		ServerUpdateGUI();
	};

	auto guientity = CreatePrototype<Entity>();
	auto& guic = guientity.EmplaceComponent<GUIComponent>();
	guic->AddDocument("server.rml");

	auto im = App::inputManager = std::make_shared<RavEngine::InputManager>();
    ComponentHandle<GUIComponent> gh(guientity);
	im->AddAxisMap("MouseX", Special::MOUSEMOVE_X);
	im->AddAxisMap("MouseY", Special::MOUSEMOVE_Y);
	im->BindAxis("MouseX", gh, &GUIComponent::MouseX, CID::ANY, 0);
	im->BindAxis("MouseY", gh, &GUIComponent::MouseY, CID::ANY, 0);
	im->BindAnyAction(gh->GetData());
}

void Level::SetupClient()
{
	App::networkManager.client->SetNetSpawnHook<ManagementRelay>([](Ref<Entity> e, Ref<World> w) -> void {
		// the management relay is here, so now we want to spawn objects with their ownership transfered here
		auto rpc = e->GetComponent<RPCComponent>().value();
		rpc->InvokeServerRPC(SpawnReq, NetworkBase::Reliability::Reliable, (int)0);
	});
	// only the clients move objects and need to push changes up
	// the server will automatically replicate changes from the other clients
	systemManager.EmplaceTimedSystem<SyncNetTransforms>(std::chrono::milliseconds(100));
	systemManager.EmplaceSystem<MoveEntities>();
}

void RelayComp::RequestSpawnObject(RavEngine::RPCMsgUnpacker& upk, HSteamNetConnection origin)
{
	// create entities
	constexpr auto num_entities =
#ifdef _DEBUG
		20
#else
		500
#endif
		;

    Array<NetEntity, num_entities> entities;
	for (int i = 0; i < entities.size(); i++) {
		entities[i] = make_shared<NetEntity>();
	}
    
	// transfer their ownership to the client
	for (const auto& entity : entities) {
		App::networkManager.server->ChangeOwnership(origin, entity->GetComponent<NetworkIdentity>().value());
	}
}
