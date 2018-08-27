class AliensGridBehaviourComponent : public Component
{
	float time_bomb_launched;
	bool change_direction;

	ObjectPool<Alien> * aliens_pool;
	ObjectPool<Bomb> * bombs_pool;

public:
	virtual ~AliensGridBehaviourComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Alien> * aliens_pool, ObjectPool<Bomb> * bombs_pool)
	{
		Component::Create(system, go, game_objects);

		this->aliens_pool = aliens_pool;
		this->bombs_pool = bombs_pool;
	}

	virtual void Init()
	{
		time_bomb_launched = -10000.f;	// time from the last time a bomb was dropped by one of the aliens

		for (auto i = 0; i < 5; i++)
			for (auto j = 0; j < 11; j++)
			{
				Alien * alien = aliens_pool->FirstAvailable();
				alien->Init(&change_direction, j * 38 + 111, i * 32 + 32 * 2);
				game_objects->insert(alien);
			}

		change_direction = false;
	}

	virtual void Update(float dt)
	{
		// check is one of tha aliens wants to change direction becase it reached the edge of the window 
		if (change_direction)
			for (auto alien = aliens_pool->pool.begin(); alien != aliens_pool->pool.end(); alien++)
				if ((*alien)->enabled)
					(*alien)->ChangeDirection();

		// is enough time passed from the last bomb, shoot another bomb from a random active alien
		if (CanFire())
		{
			Bomb * bomb = bombs_pool->FirstAvailable();
			if (bomb != NULL)
			{
				Alien * alien = aliens_pool->SelectRandom();
				if (alien != NULL)
				{
					bomb->Init(alien->horizontalPosition, alien->verticalPosition + 32);
					game_objects->insert(bomb);
				}
			}
		}

		change_direction = false;
	}

	// return true if enough time has passed from the previous bomb
	bool CanFire()
	{
		// shoot just if enough time passed by
		if ((system->getElapsedTime() - time_bomb_launched) < (BOMB_TIME_INTERVAL / game_speed))
			return false;

		// drop the bomb with 3% chance
		if ((rand() / (float)RAND_MAX) < 0.97f)
			return false;

		time_bomb_launched = system->getElapsedTime();

		SDL_Log("bomb!");
		return true;
	}
};



class AliensGrid : public GameObject
{
public:

	virtual ~AliensGrid() { SDL_Log("AliensGrid::~AliensGrid"); }



	virtual void Init()
	{
		SDL_Log("AliensGrid::Init");
		GameObject::Init();
	}


};