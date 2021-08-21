#include <RavEngine/World.hpp>

namespace RavEngine {
	struct AudioAsset;
}

struct Level : public RavEngine::World {
	void OnActivate() final;
	Level();
	std::vector<Ref<RavEngine::AudioAsset>> tracks;

};