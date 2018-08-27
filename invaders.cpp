#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "avancezlib.h"
#include "object_pool.h"

#include <set>



const unsigned int	MAX_NUM_GAME_OBJECTS = 10000;
const unsigned int	NUM_LIVES = 2;

const unsigned int	MAX_NUM_ROCKETS = 32;
const unsigned int	MAX_NUM_BOMBS = 32;
const unsigned int	POINTS_PER_ALIEN = 100;
const float			FIRE_TIME_INTERVAL = .5f;
const float			BOMB_TIME_INTERVAL = 1.25f;
const float			PLAYER_SPEED = 160.0f;
const float			ROCKET_SPEED = 160.0f;
const float			ALIEN_SPEED = 40.0f;
const float			BOMB_SPEED = 120.0f;


float game_speed = 1.f;		// speed of the game; it is increased each time all the aliens are hit
							// it is also the score multiplier


#include "component.h"
#include "game_object.h"


//#include "collision_system.h"


#include "rocket.h"
#include "bomb.h"
#include "alien.h"
#include "aliens_grid.h"
#include "player.h"

#include "game.h"


int main(int argc, char** argv)
{

	AvancezLib system;

	system.init(640, 480);

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



