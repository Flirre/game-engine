#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "avancezlib.h"
#include "object_pool.h"

#include <set>
#include <utility>      // std::pair, std::make_pair

const unsigned int	WORLD_WIDTH = 256;
const unsigned int	WORLD_HEIGHT = 224;
//const unsigned int	PLAYER_SIZE = 16;


const unsigned int	NUM_LIVES = 2;
const float			SPAWN_TIME = 3.0f;
const float			PLAYER_SPEED = 60.0f;
const float			KOOPA_SPEED = 20.0f;

const float			GRAVITY = -500.0f;


const float			GROUND_POSITION = 203.0f;

float game_speed = 1.f;


#include "component.h"
#include "game_object.h"
#include "map_object.h"
#include "brick.h"


//#include "collision_system.h"


#include "rocket.h"
#include "bomb.h"
#include "koopa.h"
#include "aliens_grid.h"
#include "player.h"

#include "game.h"


int main(int argc, char** argv)
{

	AvancezLib system;

	system.init(256, 224);

	Game game;
	game.Create(&system);
	game.Init();

	float lastTime = system.getElapsedTime();
	while (system.update())
	{
		float newTime = system.getElapsedTime();
		float dt = newTime - lastTime;
		dt = dt * game_speed;
		lastTime = newTime;

		game.Update(dt);

		game.Draw();
	}

	// clean up
	game.Destroy();
	system.destroy();

	return 0;
}



