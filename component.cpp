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

bool boundingBoxCollision(GameObject* go, GameObject* go0)
{
	double goLeftEdge = go->horizontalPosition;
	double goRightEdge = go->horizontalPosition + go->spriteWidth;
	double goBottomEdge = go->verticalPosition + go->spriteHeight;
	double goTopEdge = go->verticalPosition;

	double go0LeftEdge = go0->horizontalPosition;
	double go0RightEdge = go0->horizontalPosition + go0->spriteWidth;
	double go0BottomEdge = go0->verticalPosition + go0->spriteHeight;
	double go0TopEdge = go0->verticalPosition;

	bool goDirectionUp = (go->verticalVelocity < 0) ? true : false;

	return((goBottomEdge > go0TopEdge) && (goTopEdge < go0BottomEdge) && (goLeftEdge < go0RightEdge) && (goRightEdge > go0LeftEdge));
}


void CollideComponent::Update(float dt)
{
	bool goDirectionUp = (go->verticalVelocity < 0) ? true : false;
	double goLeftEdge = go->horizontalPosition;
	double goRightEdge = go->horizontalPosition + go->spriteWidth;
	double goBottomEdge = go->verticalPosition + go->spriteHeight;
	double goTopEdge = go->verticalPosition;
	bool xMovement = (go->horizontalVelocity != 0) ? true : false;

	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled) {
			double go0LeftEdge = go0->horizontalPosition;
			double go0RightEdge = go0->horizontalPosition + go0->spriteWidth;
			double go0BottomEdge = go0->verticalPosition + go0->spriteHeight;
			double go0TopEdge = go0->verticalPosition;



			if (boundingBoxCollision(go, go0)) // check for any collision AABB or whatever
			{
				if (go0->map_object)
					{

					if (xMovement && !(go->onGround) && ((goRightEdge - go0LeftEdge) > 0.1) && ((goRightEdge - go0LeftEdge) < 1))
					{
						SDL_Log("____________________________");
						SDL_Log("player bottom %f | map top %f | diff %f", goBottomEdge, go0TopEdge, goBottomEdge - go0TopEdge);
						SDL_Log("player top %f | map bottom %f | diff %f", goTopEdge, go0BottomEdge, goTopEdge - go0BottomEdge);
						SDL_Log("player left %f | map right %f | diff %f", goLeftEdge, go0RightEdge, goLeftEdge - go0RightEdge);
						SDL_Log("player right %f | map left %f | diff %f", goRightEdge, go0LeftEdge, goRightEdge - go0LeftEdge);
						SDL_Log("____________________________");
						go->horizontalVelocity = 0;
						go->horizontalPosition = go->horizontalPosition - (goRightEdge - go0LeftEdge);
						//SDL_Log("goHoz %f", go->horizontalVelocity);
						//SDL_Log("goVert %f", go->verticalVelocity);
						////go->horizontalPosition -= go0LeftEdge/200;
						//go->horizontalPosition -= (goRightEdge - goLeftEdge);
						////SDL_Log("player bottom %f | map top %f | diff %f", goBottomEdge, go0TopEdge, goBottomEdge - go0TopEdge);
						////SDL_Log("player top %f | map bottom %f | diff %f", goTopEdge, go0BottomEdge, goTopEdge - go0BottomEdge);
						////SDL_Log("player right %f | map left %f | diff %f", goRightEdge, go0LeftEdge, goRightEdge - go0LeftEdge);
						//return;
					}
					//	if ( && goDirectionUp) // collision above player
					//	{
					//		go->verticalVelocity /= 2;
					//	}
					//	if () // collision below player / landing on ground
					//	{
					//		go->Receive(ON_MAP);
					//	}
					//	if () // check for collision on the sides, stopping the player from overlapping map objects.
					//	{

					//	}
					//	
					//SDL_Log("helo %d", rand());
					else {
						go->Receive(ON_MAP);
					}
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
