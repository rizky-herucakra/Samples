#include "MainMenu.hpp"
#include <RavEngine/GUI.hpp>
#include <RavEngine/InputManager.hpp>
#include <RavEngine/App.hpp>
#include "GameWorld.hpp"
#include <RavEngine/Debug.hpp>

using namespace RavEngine;
using namespace std;

MainMenu::MainMenu(){
	mainMenu = CreatePrototype<RavEngine::Entity>();
	
	auto& menu = mainMenu.EmplaceComponent<GUIComponent>();
	auto doc = menu.AddDocument("mainmenu.rml");
	menu.AddDocument("loading.rml")->Hide();
	
	// shown here is an example of how to bind separate event listeners
	// You can also share event listeners by reading data from the event object
	struct QuitEventListener: public Rml::EventListener{
		void ProcessEvent(Rml::Event& event) override{
			GetApp()->Quit();
		}
	};
	struct StartEventListener: public Rml::EventListener{
		MainMenu* menu;
		StartEventListener(const decltype(menu) m) : menu(m) {};
		void ProcessEvent(Rml::Event& event) override{
			menu->LoadGame(1);
		}
	};
	struct StartMultiplayerEventListener : public Rml::EventListener{
		MainMenu* menu;
		StartMultiplayerEventListener(const decltype(menu) m) : menu(m) {};
		void ProcessEvent(Rml::Event& event) override{
			menu->LoadGame(2);
		}
	};
	struct StartZeroplayerEventListener : public Rml::EventListener{
		MainMenu* menu;
		StartZeroplayerEventListener(const decltype(menu) m) : menu(m) {};
		void ProcessEvent(Rml::Event& event) override{
			menu->LoadGame(0);
		}
	};
	
	doc->GetElementById("quitbtn")->AddEventListener(Rml::EventId::Click, new QuitEventListener());
	doc->GetElementById("playsingle")->AddEventListener(Rml::EventId::Click, new StartEventListener(this));
	doc->GetElementById("playmulti")->AddEventListener(Rml::EventId::Click, new StartMultiplayerEventListener(this));
	doc->GetElementById("playzero")->AddEventListener(Rml::EventId::Click, new StartZeroplayerEventListener(this));
	
		
	Ref<InputManager> im = RavEngine::New<InputManager>();
	im->AddAxisMap("MouseX", Special::MOUSEMOVE_X);
	im->AddAxisMap("MouseY", Special::MOUSEMOVE_Y);
    
    ComponentHandle<GUIComponent> gh(mainMenu);
	
	im->BindAxis("MouseX", gh, &GUIComponent::MouseX, CID::ANY, 0);
	im->BindAxis("MouseY", gh, &GUIComponent::MouseY, CID::ANY, 0);
	im->BindAnyAction(gh->GetData());
	
	GetApp()->inputManager = im;
	
	menu.Debug();
}

void MainMenu::LoadGame(int numplayers){
	auto& gui = mainMenu.GetComponent<GUIComponent>();
	gui.GetDocument("mainmenu.rml")->Hide();
	gui.GetDocument("loading.rml")->Show();
	
	std::thread worker([=]{
		auto g = RavEngine::New<GameWorld>(numplayers);

		GetApp()->DispatchMainThread([=]{
			GetApp()->AddReplaceWorld(shared_from_this(), g);
		});
	});
	worker.detach();
}
