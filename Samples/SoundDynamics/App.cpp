#include <RavEngine/App.hpp>
#include "AppInfo.hpp"
#include "Level.hpp"
#include <RavEngine/Dialogs.hpp>
#include <RavEngine/RenderEngine.hpp>

using namespace RavEngine;

struct SoundDynamics : public RavEngine::App {
	void OnStartup(int argc, char** argv) final {
		auto world = RavEngine::New<Level>();
		AddWorld(world);

		SetWindowTitle(RavEngine::StrFormat("{} | {}", APPNAME, GetRenderEngine().GetCurrentBackendName()).c_str());
	}
    
    void OnFatal(const char* msg) final{
        RavEngine::Dialog::ShowBasic("Fatal Error", msg, Dialog::MessageBoxType::Error);
    }
};

START_APP(SoundDynamics)
