
bool change_direction = false;

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/
	ObjectPool<Koopa> koopa_pool;
	ObjectPool<MapObject> map;
	ObjectPool<Brick> bricks;
	std::vector<std::pair<double, double>> ledge_coordinates;
	
	AvancezLib* system;

	Player * player;


	Sprite * life_sprite;
	bool game_over;

	unsigned int score = 0;

public:

	virtual void Create(AvancezLib* system)
	{
		SDL_Log("Game::Create");

		this->system = system;

		// create pairs of x- and y-coordinates for placement of ledges.
		{
			ledge_coordinates.push_back(std::make_pair(0, 175));
			ledge_coordinates.push_back(std::make_pair(32, 175));
			ledge_coordinates.push_back(std::make_pair(65, 175));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 33, 175));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 66, 175));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 99, 175));

			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH / 2, 131));
			ledge_coordinates.push_back(std::make_pair((WORLD_WIDTH / 2)-33, 131));
			ledge_coordinates.push_back(std::make_pair((WORLD_WIDTH / 2) + 33, 131));
			ledge_coordinates.push_back(std::make_pair((WORLD_WIDTH / 2) - 66, 131));

			ledge_coordinates.push_back(std::make_pair(0, 140));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-33, 140));

			ledge_coordinates.push_back(std::make_pair(0, 87));
			ledge_coordinates.push_back(std::make_pair(32, 87));
			ledge_coordinates.push_back(std::make_pair(65, 87));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-33, 87));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-66, 87));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-99, 87));
		}

		map.Create(ledge_coordinates.size());
		{
			// create ledges that players and enemies walk on
			for (auto map_object = map.pool.begin(); map_object != map.pool.end(); map_object++)
			{
				(*map_object)->horizontalPosition = ledge_coordinates.back().first;
				(*map_object)->verticalPosition = ledge_coordinates.back().second;
				ledge_coordinates.pop_back();
				RenderComponent * ledge_render = new RenderComponent();
				ledge_render->Create(system, *map_object, &game_objects, "data/bmps/board/frame23.bmp");
				(*map_object)->Create();
				(*map_object)->AddRenderComponent(ledge_render);
				(*map_object)->AddReceiver(this);
				game_objects.insert(*map_object);
			}

			// create bricks at the bottom of the map
			int j = 0;
			bricks.Create(16);
			for (auto brick = bricks.pool.begin(); brick != bricks.pool.end(); brick++, j++)
			{
				// render bricks at the bottom of the screen, covering all of the bottom.
				(*brick)->horizontalPosition = 0 + (16*j);
				(*brick)->verticalPosition = (WORLD_HEIGHT - 15);

				RenderComponent * bridge_render = new RenderComponent();
				bridge_render->Create(system, (*brick), &game_objects, "data/bmps/board/frame0.bmp");
				(*brick)->Create();
				(*brick)->AddRenderComponent(bridge_render);
				(*brick)->AddReceiver(this);
				game_objects.insert(*brick);
			}
		}

		{
			player = new Player();
			PlayerBehaviourComponent * player_behaviour = new PlayerBehaviourComponent();
			player_behaviour->Create(system, player, &game_objects);
			RenderComponent * player_render = new RenderComponent();
			player_render->Create(system, player, &game_objects, "data/bmps/frame6.bmp");
			CollideComponent * player_collider = new CollideComponent();
			player_collider->Create(system, player, &game_objects, (ObjectPool<GameObject>*) &koopa_pool);
			CollideComponent * map_collider = new CollideComponent();
			map_collider->Create(system, player, &game_objects, (ObjectPool<GameObject>*) &map);
			CollideComponent * brick_collider = new CollideComponent();
			brick_collider->Create(system, player, &game_objects, (ObjectPool<GameObject>*) &bricks);

			player->Create();
			player->AddComponent(player_behaviour);
			player->AddComponent(player_collider);
			player->AddComponent(map_collider);
			player->AddComponent(brick_collider);
			player->AddRenderComponent(player_render);
			player->AddReceiver(this);
			game_objects.insert(player);
		}

		koopa_pool.Create(1);
		for(auto koopa = koopa_pool.pool.begin(); koopa != koopa_pool.pool.end(); koopa++)
		{
		KoopaBehaviourComponent * koopa_behaviour = new KoopaBehaviourComponent();
		koopa_behaviour->Create(system, *koopa, &game_objects);
		RenderComponent * koopa_render = new RenderComponent();
		koopa_render->Create(system, *koopa, &game_objects, "data/bmps/frame38.bmp");
		CollideComponent * map_collider = new CollideComponent();
		map_collider->Create(system, player, &game_objects, (ObjectPool<GameObject>*) &map);
		CollideComponent * brick_collider = new CollideComponent();
		brick_collider->Create(system, player, &game_objects, (ObjectPool<GameObject>*) &bricks);

		(*koopa)->Create();
		(*koopa)->AddComponent(koopa_behaviour);
		(*koopa)->AddRenderComponent(koopa_render);
		(*koopa)->AddComponent(map_collider);
		(*koopa)->AddComponent(brick_collider);
		(*koopa)->AddReceiver(this);
		game_objects.insert(*koopa);
		}

		life_sprite = system->createSprite("data/bmps/frame0.bmp");
		score = 0;
	}

	virtual void Init()
	{
		for (auto map_object = map.pool.begin(); map_object != map.pool.end(); map_object++)
		{
				(*map_object)->Init();
		}

		for (auto brick = bricks.pool.begin(); brick != bricks.pool.end(); brick++)
		{
			(*brick)->Init();
		}

		player->Init();

		for (auto koopa = koopa_pool.pool.begin(); koopa != koopa_pool.pool.end(); koopa++)
		{
			(*koopa)->Init();
		}
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
	}
};