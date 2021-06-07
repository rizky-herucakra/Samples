#include "App.hpp"
#include "Level.hpp"
#include <SDL_main.h>

using namespace std;

void Performance_A::OnStartup(int argc, char** argv) {
	
	//unlock tickrate
	SetMinTickTime(std::chrono::duration<double, std::milli>(0));

	// load world
	AddWorld(make_shared<PerfA_World>());

	SetWindowTitle(RavEngine::StrFormat("RavEngine Performance_A | {}", Renderer->currentBackend()).c_str());
}

START_APP(Performance_A)
