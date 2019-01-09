#include "component.h"
#include "game_object.h"
#include "avancezlib.h"

void Component::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->system = system;
	this->game_objects = game_objects;
}

void RenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(system, go, game_objects);

	sprite = system->createSprite(sprite_name);
}

void RenderComponent::SetSprite(Sprite * s) {
	sprite = s;
}


void RenderComponent::Update(float dt)
{
	if (!go->enabled)
		return;

	if (sprite)
		sprite->draw(int(go->horizontalPosition), int(go->verticalPosition), go->direction);
}

void RenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}


void CollideComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}

bool collisionAbove(GameObject* go, GameObject* go0) {
	return ((go0->verticalPosition > go->verticalPosition) &&
		((go0->verticalPosition - go->verticalPosition) <= (go->spriteHeight)) &&
		((go0->verticalPosition - go->verticalPosition) >= 0));
}

bool collisionBelow(GameObject* go, GameObject* go0) {
	return ((go0->verticalPosition < go->verticalPosition) &&
		((go->verticalPosition - go0->verticalPosition) <= (go0->spriteHeight)) &&
		((go->verticalPosition - go0->verticalPosition) >= 0));
}

bool collisionLeft(GameObject* go, GameObject* go0) {
	return ((go0->horizontalPosition < go->horizontalPosition) &&
		((go0->horizontalPosition - go->horizontalPosition) >= -go0->spriteWidth) &&
		((go0->horizontalPosition - go->horizontalPosition) <= 0));
}

bool collisionRight(GameObject* go, GameObject* go0) {
	return ((go0->horizontalPosition > go->horizontalPosition) &&
		((go->horizontalPosition - go0->horizontalPosition) >= -go->spriteWidth) &&
		((go->horizontalPosition - go0->horizontalPosition) <= 0));
}


void CollideComponent::Update(float dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled) {
			if (go0->enabled)
			{
				//if ((abs(go0->horizontalPosition - go->horizontalPosition) < 12) &&
				//	(abs(go0->verticalPosition - go->verticalPosition)) < 10)
				//{
				//	if (go0->map_object)
				//	{
				//		SDL_Log("fk");
				//		go->Receive(MAP);
				//	}
				//	else
				//	{
				//		go->Receive(SIDE_HIT);
				//	}

				//}
				if ( (collisionAbove(go, go0) || collisionBelow(go, go0)) && (collisionLeft(go, go0) || collisionRight(go, go0)))
				{
					{
						if (go0->map_object)
						{
							go->Receive(MAP);
							SDL_Log("MAPO::VertPos=%d", go0->verticalPosition);
							SDL_Log("GAMO MAPO horiDIFF=%f", (go0->horizontalPosition - go->horizontalPosition));
						}
						else
						{
							SDL_Log("GAMO::VertPos=%d", go->verticalPosition);
							SDL_Log("GAMO MAPO DIFF=%d", (go0->verticalPosition - go->verticalPosition));
							go->Receive(MAP);
							go->Receive(HIT);
							go0->Receive(HIT);
						}
					}
				}
			}
		}
	}
}
