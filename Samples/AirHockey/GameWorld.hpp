#include <RavEngine/World.hpp>
#include "Puck.hpp"
#include "Table.hpp"
#include "Paddle.hpp"
#include "Player.hpp"
#include <RavEngine/GUI.hpp>
#include <RavEngine/SharedObject.hpp>

using namespace RavEngine;

class GameWorld : public RavEngine::World {
public:
	GameWorld(int numplayers);
	
	GameWorld(const GameWorld& other);
	
	void PostTick(float) override;
	
    
protected:
    GameObject hockeytable = CreatePrototype<Table>();
    GameObject puck = CreatePrototype<Puck>();
	
	GameObject cameraBoom = CreatePrototype<GameObject>();
	
	Paddle p1;
	Paddle p2;

	int numplayers;
	
	int p1score = 0, p2score = 0, numToWin = 3;
	
	Rml::Element* Scoreboard = nullptr;
	
	void Reset();
	void GameOver();
};
