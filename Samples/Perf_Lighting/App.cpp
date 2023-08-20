#include "Level.hpp"
#include <RavEngine/App.hpp>
#include "AppInfo.hpp"
#include <RavEngine/Dialogs.hpp>
#include <RavEngine/RenderEngine.hpp>

using namespace std;

struct LightingApp : public RavEngine::App {
	void OnStartup(int argc, char** argv) final{

		App::GetRenderEngine().VideoSettings.vsync = false;
		App::GetRenderEngine().SyncVideoSettings();

		AddWorld(RavEngine::New<Level>());

		SetWindowTitle(RavEngine::StrFormat("{} | {}", APPNAME, GetRenderEngine().GetCurrentBackendName()).c_str());
	}
    void OnFatal(const char* msg) final{
        RavEngine::Dialog::ShowBasic("Fatal Error", msg, RavEngine::Dialog::MessageBoxType::Error);
    }
};

START_APP(LightingApp)