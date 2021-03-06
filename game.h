
bool change_direction = false;

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/
	ObjectPool<Koopa> koopa_pool;
	ObjectPool<MapObject> map;
	ObjectPool<Brick> bricks;
	GameObject * top_left_pipe;
	GameObject * top_right_pipe;
	GameObject * bottom_left_pipe;
	GameObject * bottom_right_pipe;
	MapObject * spawn_ledge;
	std::vector<std::pair<double, double>> ledge_coordinates;
	std::vector<std::pair<double, double>> koopa_start_coordinates;
	
	AvancezLib* system;

	Player * player;


	Sprite * life_sprite;
	bool game_over;

	float spawn_timer = -10;
	unsigned int score = 0;
	int koopas_killed = 0;

public:

	virtual void Create(AvancezLib* system)
	{
		SDL_Log("Game::Create");

		this->system = system;

		// create pairs of x- and y-coordinates for placement of ledges.
		{
			// bottom left
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 190, 160));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 223, 160));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 256, 160));

			// bottom right
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 33, 160));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 66, 160));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 99, 160));

			// middle 
			ledge_coordinates.push_back(std::make_pair(165, 110));
			ledge_coordinates.push_back(std::make_pair(132, 110));
			ledge_coordinates.push_back(std::make_pair(99, 110));
			ledge_coordinates.push_back(std::make_pair(66, 110));

			// lower left single 
			ledge_coordinates.push_back(std::make_pair(0, 118));

			// lower right single 
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-33, 118));

			// top left
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 190, 60));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 223, 60));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH - 256, 60));

			// top right
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-33, 60));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-66, 60));
			ledge_coordinates.push_back(std::make_pair(WORLD_WIDTH-99, 60));
		}

		int i = 0;
		int j = 0;
		int nr_of_ledges = ledge_coordinates.size();
		map.Create(nr_of_ledges + 16 + 1); // 16 = bottom bricks, 1 = spawn ledge
		{
			auto map_iterator = map.pool.begin();
			// create ledges that players and enemies walk on

			for (map_iterator; i < nr_of_ledges; map_iterator++, i++)
			{
				(*map_iterator)->horizontalPosition = ledge_coordinates.back().first;
				(*map_iterator)->verticalPosition = ledge_coordinates.back().second;
				ledge_coordinates.pop_back();
				std::vector<Sprite*> sprites;
				std::vector<Sprite*> activated_sprite;
				std::vector<std::vector<Sprite*>> all_sprites;
				sprites.push_back(system->createSprite("data/bmps/board/frame23.bmp"));

				activated_sprite.push_back(system->createSprite("data/bmps/board/frame25.bmp"));
				activated_sprite.push_back(system->createSprite("data/bmps/board/frame26.bmp"));
				activated_sprite.push_back(system->createSprite("data/bmps/board/frame27.bmp"));
				activated_sprite.push_back(system->createSprite("data/bmps/board/frame23.bmp"));
				all_sprites.push_back(sprites);
				all_sprites.push_back(activated_sprite);
				RenderComponent * ledge_render = new RenderComponent();
				ledge_render->Create(system, *map_iterator, &game_objects, all_sprites);
				(*map_iterator)->Create(system);
				(*map_iterator)->AddRenderComponent(ledge_render);
				(*map_iterator)->AddReceiver(this);
				game_objects.insert(*map_iterator);
			}

			/* create bricks at the bottom of the map
			 for loop starts where ledges end.
			 16 bricks is equal to full width of map.*/
			for (map_iterator;i < nr_of_ledges+16; map_iterator++, j++, i++)
			{
				auto brick = map_iterator;
				// render bricks at the bottom of the screen, covering all of the bottom.
				(*brick)->horizontalPosition = 0 + (16 * j);
				(*brick)->verticalPosition = (WORLD_HEIGHT - 15);

				RenderComponent * brick_render = new RenderComponent();
				std::vector<Sprite*> sprites;
				std::vector<std::vector<Sprite*>> all_sprites;
				sprites.push_back(system->createSprite("data/bmps/board/frame0.bmp"));
				all_sprites.push_back(sprites);
				brick_render->Create(system, (*brick), &game_objects, all_sprites);
				(*brick)->Create(system);
				(*brick)->AddRenderComponent(brick_render);
				(*brick)->AddReceiver(this);
				game_objects.insert(*brick);
			}

			{
				auto spawn_ledge = map_iterator;
				std::vector<Sprite*> sprites;
				std::vector<std::vector<Sprite*>> all_sprites;
				sprites.push_back(system->createSprite("data/bmps/board/frame269.bmp"));
				all_sprites.push_back(sprites);
				RenderComponent * spawn_render = new RenderComponent();
				spawn_render->Create(system, (*spawn_ledge), &game_objects, all_sprites);
				(*spawn_ledge)->Create(system);
				(*spawn_ledge)->AddRenderComponent(spawn_render);
				(*spawn_ledge)->horizontalPosition = 123;
				(*spawn_ledge)->verticalPosition = 44;
				(*spawn_ledge)->AddReceiver(this);
				this->AddReceiver(*spawn_ledge);
				(*spawn_ledge)->Init(15, 6);
				game_objects.insert(*spawn_ledge);
			}
		}

		{
			top_left_pipe = new MapObject();
			std::vector<Sprite*> sprites;
			std::vector<std::vector<Sprite*>> all_sprites;
			sprites.push_back(system->createSprite("data/bmps/board/frame12.bmp"));
			all_sprites.push_back(sprites);
			RenderComponent * pipe_render = new RenderComponent();
			pipe_render->Create(system, top_left_pipe, &game_objects, all_sprites);
			top_left_pipe->Create();
			top_left_pipe->AddRenderComponent(pipe_render);
			top_left_pipe->horizontalPosition = 0;
			top_left_pipe->verticalPosition = 12;
			game_objects.insert(top_left_pipe);
		}

		{
			top_right_pipe = new MapObject();
			std::vector<Sprite*> sprites;
			std::vector<std::vector<Sprite*>> all_sprites;
			sprites.push_back(system->createSprite("data/bmps/board/frame9.bmp"));
			all_sprites.push_back(sprites);
			RenderComponent * pipe_render = new RenderComponent();
			pipe_render->Create(system, top_right_pipe, &game_objects, all_sprites);
			top_right_pipe->Create();
			top_right_pipe->AddRenderComponent(pipe_render);
			top_right_pipe->horizontalPosition = WORLD_WIDTH-33;
			top_right_pipe->verticalPosition = 12;
			game_objects.insert(top_right_pipe);
		}

		{
			bottom_left_pipe = new MapObject();
			std::vector<Sprite*> sprites;
			std::vector<std::vector<Sprite*>> all_sprites;
			sprites.push_back(system->createSprite("data/bmps/board/frame12.bmp"));
			all_sprites.push_back(sprites);
			RenderComponent * pipe_render = new RenderComponent();
			pipe_render->Create(system, bottom_left_pipe, &game_objects, all_sprites);
			bottom_left_pipe->Create();
			bottom_left_pipe->AddRenderComponent(pipe_render);
			bottom_left_pipe->horizontalPosition = -15;
			bottom_left_pipe->verticalPosition = 192;
			game_objects.insert(bottom_left_pipe);
		}

		{
			bottom_right_pipe = new MapObject();
			std::vector<Sprite*> sprites;
			std::vector<std::vector<Sprite*>> all_sprites;
			sprites.push_back(system->createSprite("data/bmps/board/frame9.bmp"));
			all_sprites.push_back(sprites);
			RenderComponent * pipe_render = new RenderComponent();
			pipe_render->Create(system, bottom_right_pipe, &game_objects, all_sprites);
			bottom_right_pipe->Create();
			bottom_right_pipe->AddRenderComponent(pipe_render);
			bottom_right_pipe->horizontalPosition = WORLD_WIDTH - 18;
			bottom_right_pipe->verticalPosition = 192;
			game_objects.insert(bottom_right_pipe);
		}


		


		{
			player = new Player();
			PhysicsComponent * player_physics = new PhysicsComponent();
			player_physics->Create(system, player, &game_objects, WORLD_WIDTH/2 - 6, WORLD_HEIGHT - 38, 16, 21, WORLD_WIDTH, WORLD_HEIGHT, GRAVITY, PLAYER_SPEED);
			InputComponent * player_input = new InputComponent();
			player_input->Create(system, player, &game_objects, PLAYER_SPEED, RIGHT);
			std::vector<std::vector<Sprite*>> sprites;
			std::vector<Sprite*> idle_sprite;
			std::vector<Sprite*> jump_sprite;
			std::vector<Sprite*> running_sprites;
			std::vector<Sprite*> death_sprite;
			idle_sprite.push_back(system->createSprite("data/bmps/frame6.bmp"));
			jump_sprite.push_back(system->createSprite("data/bmps/frame7.bmp"));
			{
				running_sprites.push_back(system->createSprite("data/bmps/frame1.bmp"));
				running_sprites.push_back(system->createSprite("data/bmps/frame2.bmp"));
				running_sprites.push_back(system->createSprite("data/bmps/frame3.bmp"));
				running_sprites.push_back(system->createSprite("data/bmps/frame4.bmp"));
				running_sprites.push_back(system->createSprite("data/bmps/frame5.bmp"));
			}
			death_sprite.push_back(system->createSprite("data/bmps/frame14.bmp"));
			sprites.push_back(idle_sprite);
			sprites.push_back(jump_sprite);
			sprites.push_back(running_sprites);
			sprites.push_back(death_sprite);
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

		// create pairs of x- and y-coordinates for placement of ledges.
		{
			koopa_start_coordinates.push_back(std::make_pair(WORLD_WIDTH-32, 40));
			koopa_start_coordinates.push_back(std::make_pair(16, 40));
			koopa_start_coordinates.push_back(std::make_pair(WORLD_WIDTH - 32, 138));
			koopa_start_coordinates.push_back(std::make_pair(16, 138));
		}
		
		int nr_of_koopas = koopa_start_coordinates.size();
		koopa_pool.Create(nr_of_koopas);
		i = 0;
		for(auto koopa = koopa_pool.pool.begin(); koopa != koopa_pool.pool.end(); koopa++, i++)
		{
			KoopaPhysicsComponent * koopa_physics = new KoopaPhysicsComponent();
			koopa_physics->Create(system, *koopa, &game_objects, koopa_start_coordinates.back().first, koopa_start_coordinates.back().second, 16, 16, WORLD_WIDTH, WORLD_HEIGHT, GRAVITY, KOOPA_SPEED);
			koopa_start_coordinates.pop_back();
			std::vector<std::vector<Sprite*>> sprites;
			std::vector<Sprite*> walking_sprite;
			std::vector<Sprite*> flipping_sprite;
			{
				walking_sprite.push_back(system->createSprite("data/bmps/frame38.bmp"));
				walking_sprite.push_back(system->createSprite("data/bmps/frame39.bmp"));
				walking_sprite.push_back(system->createSprite("data/bmps/frame40.bmp"));
				walking_sprite.push_back(system->createSprite("data/bmps/frame41.bmp"));
			}
			{
				flipping_sprite.push_back(system->createSprite("data/bmps/frame40.bmp"));
				flipping_sprite.push_back(system->createSprite("data/bmps/frame54.bmp"));
				flipping_sprite.push_back(system->createSprite("data/bmps/frame40.bmp"));
				flipping_sprite.push_back(system->createSprite("data/bmps/frame40.bmp"));
				flipping_sprite.push_back(system->createSprite("data/bmps/frame40.bmp"));
			}
			sprites.push_back(walking_sprite);
			sprites.push_back(flipping_sprite);
			RenderComponent * koopa_render = new RenderComponent();
			koopa_render->Create(system, *koopa, &game_objects, sprites);
			CollideComponent * map_collider = new CollideComponent();
			map_collider->Create(system, *koopa, &game_objects, (ObjectPool<GameObject>*) &map);
			KoopaCollideComponent * friend_collider = new KoopaCollideComponent();
			friend_collider->Create(system, *koopa, &game_objects, (ObjectPool<GameObject>*) &koopa_pool);
			KoopaInputComponent * koopa_input = new KoopaInputComponent();
			koopa_input->Create(system, *koopa, &game_objects, static_cast<Direction> (i % 2));


			(*koopa)->Create();
			(*koopa)->AddComponent(koopa_input);
			(*koopa)->AddComponent(koopa_physics);
			(*koopa)->AddRenderComponent(koopa_render);
			(*koopa)->AddComponent(map_collider);
			(*koopa)->AddComponent(friend_collider);
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
		top_left_pipe->Init();
		top_right_pipe->Init();
		bottom_left_pipe->Init();
		bottom_right_pipe->Init();

		enabled = true;
		game_over = false;
	}

	virtual void Update(float dt)
	{
		if (IsGameOver()) 
		{
			for (auto go = game_objects.begin(); go != game_objects.end(); go++) {
				(*go)->Destroy();
			}
			dt = 0.f;
		}

		if (PlayerWin())
		{
			for (auto go = game_objects.begin(); go != game_objects.end(); go++) {
				(*go)->Destroy();
			}
			dt = 0.f;
		}
		// updating map objects first, so that movable characters are rendered in front of them.
		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			if ((*go)->map_object) {
				(*go)->Update(dt);
			}

		// possible optimization would be tracking what objects aren't being rendered and then rendering
		// just those objects instead of doing two passes.
		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			if (!(*go)->map_object) {
				(*go)->Update(dt);
			}

		// when SPAWN_TIME time has elapsed, remove the spawn ledge
		if (((system->getElapsedTime() - spawn_timer) > SPAWN_TIME) && (map.pool.at(34)->enabled))
		{
			map.pool.at(34)->enabled = false;
		}


	}

	virtual void Draw()
	{
		char msg[1024];
		if (!(IsGameOver() || PlayerWin())) {
			sprintf(msg, "MARIO %07d", Score());
			system->drawText(82, 2, msg);
		}
		//for (int i = 0; i < player->lives; i++)
		//	life_sprite->draw(i*18+20, 16, false);

		if (IsGameOver())
		{
			sprintf(msg, "*** G A M E  O V E R ***");
			system->drawText(55, 100, msg);
			sprintf(msg, "%07d", Score());
			system->drawText(100, 120, msg);
		}

		if (PlayerWin())
		{
			sprintf(msg, "FINAL SCORE %07d", Score());
			system->drawText(52, 120, msg);
			sprintf(msg, "CONGRATULATIONS, YOU WON!", Score());
			system->drawText(32, 100, msg);
		}
	}

	virtual void Receive(Message m)
	{
		if (m == GAME_OVER)
			game_over = true;

		if (m == HIT) {
			score += 400;
			koopas_killed++;
		}
		// messages to control spawn pad.
		if (m == SPAWN) {
			map.pool.at(34)->Init(15, 6);
			map.pool.at(34)->enabled = true;
			spawn_timer = system->getElapsedTime();
		}
	}


	bool IsGameOver()
	{
		return game_over;
	}

	bool PlayerWin()
	{
		return koopas_killed == 4;
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