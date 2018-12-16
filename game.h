
bool change_direction = false;

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/
	
	AvancezLib* system;

	Player * player;
	Koopa * koopa;


	Sprite * life_sprite;
	bool game_over;

	unsigned int score = 0;

public:

	virtual void Create(AvancezLib* system)
	{
		SDL_Log("Game::Create");

		this->system = system;
		{
			player = new Player();
			PlayerBehaviourComponent * player_behaviour = new PlayerBehaviourComponent();
			player_behaviour->Create(system, player, &game_objects);
			RenderComponent * player_render = new RenderComponent();
			player_render->Create(system, player, &game_objects, "data/bmps/frame6.bmp");

			player->Create();
			player->AddComponent(player_behaviour);
			player->AddRenderComponent(player_render);
			player->AddReceiver(this);
			game_objects.insert(player);
		}
		{
		koopa = new Koopa();
		KoopaBehaviourComponent * koopa_behaviour = new KoopaBehaviourComponent();
		koopa_behaviour->Create(system, koopa, &game_objects);
		RenderComponent * koopa_render = new RenderComponent();
		koopa_render->Create(system, koopa, &game_objects, "data/bmps/frame38.bmp");

		koopa->Create();
		koopa->AddComponent(koopa_behaviour);
		koopa->AddRenderComponent(koopa_render);
		koopa->AddReceiver(this);
		game_objects.insert(koopa);
		}

		life_sprite = system->createSprite("data/bmps/frame0.bmp");
		score = 0;
	}

	virtual void Init()
	{
		player->Init();
		koopa->Init();
		//aliens_grid->Init();

		enabled = true;
		game_over = false;
	}

	virtual void Update(float dt)
	{
		if (IsGameOver()) {
			for (auto go = game_objects.begin(); go != game_objects.end(); go++) {
				(*go)->Destroy();
			}
			dt = 0.f;
		}
		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt);

		// check if there are still active aliens
		// if not, send a message to re-init the level
		//bool are_aliens_still_there = false;
		//for (auto alien = aliens_pool.pool.begin(); alien != aliens_pool.pool.end(); alien++)
		//	are_aliens_still_there |= (*alien)->enabled;
		//if (!are_aliens_still_there)
		//{
		//	// level win!
		//	game_speed += 0.4f;
		//	aliens_grid->Init();
		//}
	}

	virtual void Draw()
	{
		char msg[1024];
		sprintf(msg, "%07d", Score());
		system->drawText(172, 14, msg);
		sprintf(msg, "bonus: %.1fX", game_speed);
		system->drawText(172, 32, msg);

		for (int i = 0; i < player->lives; i++)
			life_sprite->draw(i*18+20, 16, false);

		if (IsGameOver())
		{
			sprintf(msg, "*** G A M E  O V E R ***");
			system->drawText(55, 110, msg);
		}
	}

	virtual void Receive(Message m)
	{
		if (m == GAME_OVER)
			game_over = true;

		if (m == ALIEN_HIT)
			score += POINTS_PER_ALIEN * game_speed;
	}


	bool IsGameOver()
	{
		return game_over;
	}

	unsigned int Score()
	{
		return score;
	}

	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		life_sprite->destroy();

		//delete aliens_grid;
		delete player;
		delete koopa;
	}
};