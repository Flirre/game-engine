// GameObject represents objects which moves are drawn
#include <vector>

enum Message { 
	HIT, GAME_OVER, LEVEL_WIN, NO_MSG, SIDE_HIT, 
	DIE, MAP, ON_MAP, NOT_ON_MAP, JUMP, RUNNING,
	IDLE, WALK_RIGHT, WALK_LEFT, TURN_AROUND,
	SPAWN, BOUNCE};

enum Direction { RIGHT, LEFT };

class Component;
class RenderComponent;
class Sprite;


class GameObject
{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;
	RenderComponent* renderComponent;

public:
	double horizontalPosition;
	double verticalPosition;

	double horizontalVelocity;
	double verticalVelocity;
	bool onGround = false;

	double prevHPos;
	double prevVPos;
	double prevHVel;
	double prevVVel;

	int spriteWidth;
	int spriteHeight;

	bool enabled;
	bool map_object = false;

	Direction direction;

	virtual ~GameObject();

	virtual void Create();
	virtual void AddComponent(Component * component);
	virtual void AddRenderComponent(RenderComponent * component);
	virtual void SetSprites(std::vector<Sprite *> sprite_name);
	virtual std::vector<std::vector<Sprite*>> GetSprites();
	virtual std::vector<Sprite*> GetSpriteSet(int i);


	virtual void Init();
	virtual void Update(float dt);
	virtual void Destroy();
	virtual void AddReceiver(GameObject *go);
	virtual void Receive(Message m) {}
	void Send(Message m);
};