#include <set>
#include "object_pool.h"

class GameObject;
class AvancezLib;
class Sprite;

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
	Sprite* sprite;
	std::vector<Sprite*> sprites;

public:

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, std::vector<Sprite*> sprites);
	virtual void Update(float dt);
	virtual void Destroy();
	virtual void SetSprite(Sprite * sprite);

	Sprite * GetSprite() { return sprite; }
	std::vector<Sprite*> GetSprites() { return sprites; }
};


class CollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects
	bool map_object; // used to differentiate map object and game object collision.

public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
};

