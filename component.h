#include <set>
#include "object_pool.h"
#include "avancezlib.h"

class GameObject;
class AvancezLib;
class Sprite;
enum Direction;

class Component
{
protected:
	AvancezLib * system;	// used to access the system
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void Receive(int message) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
	Sprite * sprite;
	std::vector<Sprite*> spriteSet;
	std::vector<std::vector<Sprite*>> sprites;

public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, std::vector<std::vector<Sprite*>> sprites);
	virtual void Update(float dt);
	virtual void Destroy();
	virtual void SetSprites(std::vector<Sprite *> sprite);

	Sprite * GetSprite() { return sprite; }
	std::vector<Sprite *> GetCurrentSpriteSet() { return spriteSet; }
	std::vector<Sprite*> GetSpriteSet(int i) { return sprites.at(i); }
	std::vector<std::vector<Sprite*>> GetSprites() { return sprites; }
};

class PhysicsComponent : public Component
{
	unsigned int WORLD_WIDTH;
	unsigned int WORLD_HEIGHT;
	float GRAVITY;
	float SPEED;

public: 
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, float horizontalPosition, float verticalPosition, int spriteWidth, int spriteHeight, unsigned int WORLD_WIDTH, unsigned int WORLD_HEIGHT, float GRAVITY, float speed);
	virtual void Update(float dt);

	virtual void CheckBounds(unsigned int WORLD_WIDTH, GameObject* go);
	virtual void UpdatePosition(float dt, GameObject* go, float GRAVITY);
};

class InputComponent : public Component
{
	bool is_walking_left;
	bool is_walking_right;
	bool is_jumping;
	bool space_released;
	float goSPEED;

public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, float SPEED, Direction direction);
	virtual void Update(float dt);

	virtual void WalkLeft();
	virtual void WalkRight();
	virtual void Jump();
	virtual void Stop();
	virtual void UpdateMovement(AvancezLib::KeyStatus keys);
};


class CollideComponent : public Component
{
	bool map_object; // used to differentiate map object and game object collision.

public:
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);

	bool boundingBoxCollision(GameObject * go, GameObject * go0);
	void ResolveCollision(GameObject * go, GameObject * go0, float dt);
};

