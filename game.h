
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
			// bottom left
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 190, 175));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 223, 175));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 256, 175));

			// bottom right
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 33, 175));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 66, 175));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 99, 175));

			// middle 
			ledge_coordinates.push_back(std::make_pair(165, 131));
			ledge_coordinates.push_back(std::make_pair(132, 131));
			ledge_coordinates.push_back(std::make_pair(99, 131));
			ledge_coordinates.push_back(std::make_pair(66, 131));

			// lower left single 
			ledge_coordinates.push_back(std::make_pair(0, 140));

			// lower right single 
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-33, 140));

			// top left
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 190, 87));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 223, 87));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 256, 87));

			// top right
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-33, 87));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-66, 87));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-99, 87));
		}

		int i = 0;
		int j = 0;
		SDL_Log("ledge - %d", ledge_coordinates.size());
		int nr_of_ledges = ledge_coordinates.size();
		map.Create(nr_of_ledges + 16);
		{
			auto map_iterator = map.pool.begin();
			// create ledges that players and enemies walk on

			for (map_iterator; i < nr_of_ledges; map_iterator++, i++)
			{
				(*map_iterator)->horizontalPosition = ledge_coordinates.back().first;
				(*map_iterator)->verticalPosition = ledge_coordinates.back().second;
				ledge_coordinates.pop_back();
				std::vector<Sprite*> sprites;
				std::vector<std::vector<Sprite*>> all_sprites;
				sprites.push_back(system->createSprite("data/bmps/board/frame23.bmp"));
				all_sprites.push_back(sprites);
				RenderComponent * ledge_render = new RenderComponent();
				ledge_render->Create(system, *map_iterator, &game_objects, all_sprites);
				(*map_iterator)->Create();
				(*map_iterator)->AddRenderComponent(ledge_render);
				(*map_iterator)->AddReceiver(this);
				game_objects.insert(*map_iterator);
			}

			/* create bricks at the bottom of the map
			 for loop starts where ledges end.
			 16 bricks is equal to full width of map.*/
			for (auto brick = map_iterator; brick != map.pool.end(); brick++, j++)
			{
				// render bricks at the bottom of the screen, covering all of the bottom.
				(*brick)->horizontalPosition = 0 + (16 * j);
				(*brick)->verticalPosition = (WORLD_HEIGHT - 15);

				RenderComponent * brick_render = new RenderComponent();
				std::vector<Sprite*> sprites;
				std::vector<std::vector<Sprite*>> all_sprites;
				sprites.push_back(system->createSprite("data/bmps/board/frame0.bmp"));
				all_sprites.push_back(sprites);
				brick_render->Create(system, (*brick), &game_objects, all_sprites);
				(*brick)->Create();
				(*brick)->AddRenderComponent(brick_render);
				(*brick)->AddReceiver(this);
				game_objects.insert(*brick);
			}
		}




		{
			player = new Player();
			PhysicsComponent * player_physics = new PhysicsComponent();
			player_physics->Create(system, player, &game_objects, WORLD_WIDTH/2, 10, 16, 21, WORLD_WIDTH, WORLD_HEIGHT, GRAVITY, PLAYER_SPEED);
			InputComponent * player_input = new InputComponent();
			player_input->Create(system, player, &game_objects, PLAYER_SPEED);
			std::vector<std::vector<Sprite*>> sprites;
			std::vector<Sprite*> idle_sprite;
			std::vector<Sprite*> jump_sprite;
			std::vector<Sprite*> running_sprites;
			idle_sprite.push_back(system->createSprite("data/bmps/frame6.bmp"));
			jump_sprite.push_back(system->createSprite("data/bmps/frame7.bmp"));
			{
				running_sprites.push_back(system->createSprite("data/bmps/frame1.bmp"));
				running_sprites.push_back(system->createSprite("data/bmps/frame2.bmp"));
				running_sprites.push_back(system->createSprite("data/bmps/frame3.bmp"));
				running_sprites.push_back(system->createSprite("data/bmps/frame4.bmp"));
				running_sprites.push_back(system->createSprite("data/bmps/frame5.bmp"));
			}
			sprites.push_back(idle_sprite);
			sprites.push_back(jump_sprite);
			sprites.push_back(running_sprites);
			RenderComponent * player_render = new RenderComponent();
			player_render->Create(system, player, &game_objects, sprites);
			CollideComponent * player_collider = new CollideComponent();
			player_collider->Create(system, player, &game_objects, (ObjectPool<GameObject>*) &koopa_pool);
			CollideComponent * map_collider = new CollideComponent();
			map_collider->Create(system, player, &game_objects, (ObjectPool<GameObject>*) &map);

			player->Create();
			player->AddComponent(player_physics);
			player->AddComponent(player_input);
			player->AddComponent(player_collider);
			player->AddComponent(map_collider);
			player->AddRenderComponent(player_render);
			player->AddReceiver(this);
			game_objects.insert(player);
		}

		koopa_pool.Create(1);
		for(auto koopa = koopa_pool.pool.begin(); koopa != koopa_pool.pool.end(); koopa++)
		{
		KoopaBehaviourComponent * koopa_behaviour = new KoopaBehaviourComponent();
		koopa_behaviour->Create(system, *koopa, &game_objects);
		std::vector<std::vector<Sprite*>> sprites;
		std::vector<Sprite*> idle_sprite;
		idle_sprite.push_back(system->createSprite("data/bmps/frame38.bmp"));
		sprites.push_back(idle_sprite);
		RenderComponent * koopa_render = new RenderComponent();
		koopa_render->Create(system, *koopa, &game_objects, sprites);
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