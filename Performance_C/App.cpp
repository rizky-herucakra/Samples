#include "PerfC_World.hpp"
#include "RavEngine/App.hpp"

class Performance_C : public RavEngine::App{
public:
	Performance_C() : App("RavEngine_Sample_Performance_C"){}
private:
	void OnStartup(int argc, char** argv) override;
};

using namespace RavEngine;

void Performance_C::OnStartup(int argc, char **argv){
	//setup video settings
	RenderEngine::VideoSettings.vsync = true;
	RenderEngine::VideoSettings.width = 1280;
	RenderEngine::VideoSettings.height = 720;
	
	Renderer->SyncVideoSettings();
    
    //unlock the tickrate
    SetMinTickTime(std::chrono::duration<double, std::milli>(0));
	
	AddWorld(std::make_shared<PerfC_World>());
	
	SetWindowTitle(StrFormat("RavEngine Performance_C | {}", Renderer->currentBackend()).c_str());
}

START_APP(Performance_C);
