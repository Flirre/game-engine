

// the main player
class Player : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 
	int hits = 0;

	virtual ~Player() { SDL_Log("Player::~Player"); }

	virtual void Init()
	{
		SDL_Log("Player::Init");
		GameObject::Init();
		lives = NUM_LIVES;
	}

	virtual void Receive(Message m)
	{
		if (m == HIT)
		{
			//SDL_Log("Player::TopHit");
		}
		if (m == SIDE_HIT) 
		{
			hits++;
			//SDL_Log("Player::SideHit! %d", hits);
			RemoveLife();

			if (lives < 0)
				Send(GAME_OVER);
		}
		if (m == ON_MAP)
		{
			hits++;
			verticalVelocity = 0;
			onGround = true;
			SetSprite(GetSprites().at(0));
		}
		if (m == NOT_ON_MAP)
		{
			onGround = false;
		}
		if (m == JUMP) 
		{
			SetSprite(GetSprites().at(1));
		}
	}

	void RemoveLife()
	{
		//lives--;
		//SDL_Log("remaining lives %d", lives);
	}
};